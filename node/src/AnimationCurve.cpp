//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor, a program for creating visual effects
//in the NetImmerse format.
//
//SVFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SVFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SVFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "AnimationCurve.h"
#include "widget_types.h"

using namespace nif;

constexpr gui::ColRGBA ACTIVE_COL{ 1.0f, 1.0f, 1.0f, 1.0f };
constexpr gui::ColRGBA SELECTED_COL{ 1.0f, 0.8f, 0.0f, 1.0f };
constexpr gui::ColRGBA UNSELECTED_COL{ 0.0f, 0.0f, 0.0f, 1.0f };

constexpr float HANDLE_LENGTH = 30.0f;
constexpr float HANDLE_RADIUS = 3.0f;

//We make use of Eigen::Array methods for rounding
static_assert(TO_PIXEL == static_cast<decltype(TO_PIXEL)>(&std::floor));

class EraseOp final : public gui::ICommand
{
	const ni_ptr<Vector<Key<float>>> m_target;
	std::vector<int> m_keys;
	std::vector<Key<float>> m_storage;

public:
	EraseOp(ni_ptr<Vector<Key<float>>>&& target, std::vector<int>&& keys) :
		m_target{ std::move(target) }, m_keys{ std::move(keys) }
	{
		assert(m_target);

		std::sort(m_keys.begin(), m_keys.end());

		m_storage.resize(m_keys.size());
		for (size_t i = 0; i < m_keys.size(); i++) {
			assert(m_keys[i] >= 0 && (size_t)m_keys[i] < m_target->size());
			m_storage[i].time.set(m_target->at(m_keys[i]).time.get());
			m_storage[i].value.set(m_target->at(m_keys[i]).value.get());
			m_storage[i].fwdTan.set(m_target->at(m_keys[i]).fwdTan.get());
			m_storage[i].bwdTan.set(m_target->at(m_keys[i]).bwdTan.get());
			m_storage[i].tension.set(m_target->at(m_keys[i]).tension.get());
			m_storage[i].bias.set(m_target->at(m_keys[i]).bias.get());
			m_storage[i].continuity.set(m_target->at(m_keys[i]).continuity.get());
		}
	}

	virtual void execute() override
	{
		//erase back to front, so indices stay correct
		for (auto rit = m_keys.rbegin(); rit != m_keys.rend(); ++rit) {
			m_target->erase(*rit);
		}
	}
	virtual void reverse() override
	{
		//insert front to back
		for (size_t i = 0; i < m_keys.size(); i++) {
			m_target->insert(m_keys[i]);
			m_target->at(m_keys[i]).time.set(m_storage[i].time.get());
			m_target->at(m_keys[i]).value.set(m_storage[i].value.get());
			m_target->at(m_keys[i]).fwdTan.set(m_storage[i].fwdTan.get());
			m_target->at(m_keys[i]).bwdTan.set(m_storage[i].bwdTan.get());
			m_target->at(m_keys[i]).tension.set(m_storage[i].tension.get());
			m_target->at(m_keys[i]).bias.set(m_storage[i].bias.get());
			m_target->at(m_keys[i]).continuity.set(m_storage[i].continuity.get());
		}
	}
	virtual bool reversible() const override
	{
		return !m_keys.empty();
	}
};

class InsertOp final : public gui::ICommand
{
	const ni_ptr<Vector<Key<float>>> m_target;
	const int m_index;
	const gui::Floats<2> m_position;

public:
	InsertOp(ni_ptr<Vector<Key<float>>>&& target, int index, const gui::Floats<2>& pos) :
		m_target{ std::move(target) }, m_index{ index }, m_position{ pos }
	{
		assert(m_target);
	}

	virtual void execute() override
	{
		m_target->insert(m_index);
		m_target->at(m_index).time.set(m_position[0]);
		m_target->at(m_index).value.set(m_position[1]);
	}
	virtual void reverse() override
	{
		m_target->erase(m_index);
	}
	virtual bool reversible() const override
	{
		return true;
	}
};

class CentreMoveOp final : public node::AnimationCurve::MoveOperation
{
	const ni_ptr<Vector<Key<float>>> m_target;

	std::vector<int> m_initI;
	std::vector<gui::Floats<2>> m_initPos;

	std::vector<int> m_currentI;
	std::vector<gui::Floats<2>> m_currentPos;

	std::vector<int> m_finalI;

	const gui::Floats<2> m_start;
	gui::Floats<2> m_move{ 0.0f, 0.0f };

	bool m_dirty{ false };

public:
	CentreMoveOp(const ni_ptr<Vector<Key<float>>>& target, std::vector<int>&& indices, const gui::Floats<2>& init_pos) :
		m_target{ target }, m_initI{ std::move(indices) }, m_start{ init_pos }
	{
		assert(target);

		//Keep our index list sorted, will make order changes easier to track
		std::sort(m_initI.begin(), m_initI.end());

		m_initPos.reserve(m_initI.size());
		for (int i : m_initI) {
			m_initPos.push_back({ target->at(i).time.get(), target->at(i).value.get() });
		}
		m_currentI = m_initI;
		m_currentPos = m_initPos;

		m_finalI = m_currentI;
	}

	virtual void execute() override
	{
		if (m_dirty) {
			//Determine direction of last move increment
			int dir = 0;
			for (size_t i = 0; i < m_finalI.size(); i++) {
				if (m_finalI[i] < m_currentI[i]) {
					dir = -1;
					break;
				}
				else if (m_finalI[i] > m_currentI[i]) {
					dir = 1;
					break;
				}
			}
			//Reorder keys if needed
			if (dir == 1) {
				for (int i = (int)m_currentI.size() - 1; i >= 0; i--) {
					//assert(m_finalI[i] >= m_currentI[i]);
					m_target->move(m_currentI[i], m_finalI[i]);
					m_currentI[i] = m_finalI[i];
				}
			}
			else if (dir == -1) {
				for (size_t i = 0; i < m_currentI.size(); i++) {
					//assert(m_finalI[i] <= m_currentI[i]);
					m_target->move(m_currentI[i], m_finalI[i]);
					m_currentI[i] = m_finalI[i];
				}
			}

			for (size_t i = 0; i < m_currentI.size(); i++) {
				m_target->at(m_currentI[i]).time.set(m_currentPos[i][0]);
				m_target->at(m_currentI[i]).value.set(m_currentPos[i][1]);
			}

			m_dirty = false;
		}
	}
	virtual void reverse() override
	{
		for (size_t i = 0; i < m_currentI.size(); i++) {
			m_target->at(m_currentI[i]).time.set(m_initPos[i][0]);
			m_target->at(m_currentI[i]).value.set(m_initPos[i][1]);
		}

		if (m_move[0] < 0.0f) {
			for (int i = (int)m_currentI.size() - 1; i >= 0; i--) {
				//assert(m_initI[i] >= m_currentI[i]);
				m_target->move(m_currentI[i], m_initI[i]);
				m_currentI[i] = m_initI[i];
			}
		}
		else if (m_move[0] > 0.0f) {
			for (size_t i = 0; i < m_currentI.size(); i++) {
				//assert(m_initI[i] <= m_currentI[i]);
				m_target->move(m_currentI[i], m_initI[i]);
				m_currentI[i] = m_initI[i];
			}
		}
		m_dirty = true;
	}
	virtual bool reversible() const override
	{
		return m_move[0] != 0.0f || m_move[1] != 0.0f;
	}

	virtual void update(const gui::Floats<2>& local_pos) override
	{
		static_assert(SINGLE_THREAD);

		if (gui::Floats<2> local_move = local_pos - m_start; local_move.matrix() != m_move.matrix()) {
			if (local_move[0] - m_move[0] >= 0.0f) {
				//moving toward non-negative time, go through keys in descending order
				for (int i = (int)m_currentI.size() - 1; i >= 0; i--) {
					m_currentPos[i] = m_initPos[i] + local_move;

					//look for order changes:
					//Increase our current index until the next key is not less than us, or we reach the next key in our selection
					//(keep in mind that the keys we are moving have not had their times updated yet - hence the backwards iteration)
					for (; m_finalI[i] < (int)m_target->size() - 1; m_finalI[i]++) {
						//is the next key part of this move op?
						if (i < (int)m_finalI.size() - 1 && m_finalI[i] + 1 == m_finalI[i + 1])
							break;
						//is the next key at a greater or equal time to ours?
						else if (m_target->at(m_finalI[i] + 1).time.get() >= m_currentPos[i][0])
							break;
						//else we should be moved up (at least) one step
					}
				}
			}
			else {
				//go through keys in ascending order
				for (size_t i = 0; i < m_finalI.size(); i++) {
					m_currentPos[i] = m_initPos[i] + local_move;

					//as above, but directions are inverted
					for (; m_finalI[i] > 0; m_finalI[i]--) {
						//is the prev key part of this move op?
						if (i > 0 && m_finalI[i] - 1 == m_finalI[i - 1])
							break;
						//is the next key at a greater or equal time to ours?
						else if (m_target->at(m_finalI[i] - 1).time.get() <= m_currentPos[i][0])
							break;
						//else we should be moved down (at least) one step
					}
				}
			}
			m_move = local_move;
			m_dirty = true;
			execute();
		}
	}
};

class FwdMoveOp final : public node::AnimationCurve::MoveOperation
{
	const ni_ptr<Vector<Key<float>>> m_target;
	const int m_index;

	float m_init;
	float m_current;
	float m_bwdInit;

	const bool m_align;

public:
	FwdMoveOp(const ni_ptr<Vector<Key<float>>>& target, int index, bool align = false) :
		m_target{ target }, m_index{ index }, m_align{ align }
	{
		assert(m_target && m_index >= 0 && (size_t)m_index < m_target->size());
		m_init = target->at(m_index).fwdTan.get();
		m_current = m_init;
		if (m_align)
			m_bwdInit = target->at(m_index).bwdTan.get();
	}

	virtual void execute() override
	{
		m_target->at(m_index).fwdTan.set(m_current);
		if (m_align)
			m_target->at(m_index).bwdTan.set(m_current);
	}
	virtual void reverse() override
	{
		m_target->at(m_index).fwdTan.set(m_init);
		if (m_align)
			m_target->at(m_index).bwdTan.set(m_bwdInit);
	}
	virtual bool reversible() const override
	{
		return m_current != m_init;
	}

	virtual void update(const gui::Floats<2>& local_pos) override
	{
		float dv = local_pos[1] - m_target->at(m_index).value.get();
		float dt = local_pos[0] - m_target->at(m_index).time.get();
		m_current = dt > 0.0f ? dv / dt : std::copysign(1000.0f, dv);

		execute();
	}
};

class BwdMoveOp final : public node::AnimationCurve::MoveOperation
{
	const ni_ptr<Vector<Key<float>>> m_target;
	const int m_index;

	float m_init;
	float m_current;
	float m_fwdInit;

	const bool m_align;

public:
	BwdMoveOp(const ni_ptr<Vector<Key<float>>>& target, int index, bool align = false) :
		m_target{ target }, m_index{ index }, m_align{ align }
	{
		assert(m_target&& m_index >= 0 && (size_t)m_index < m_target->size());
		m_init = target->at(m_index).bwdTan.get();
		m_current = m_init;
		if (m_align)
			m_fwdInit = target->at(m_index).fwdTan.get();
	}

	virtual void execute() override
	{
		m_target->at(m_index).bwdTan.set(m_current);
		if (m_align)
			m_target->at(m_index).fwdTan.set(m_current);
	}
	virtual void reverse() override
	{
		m_target->at(m_index).bwdTan.set(m_init);
		if (m_align)
			m_target->at(m_index).fwdTan.set(m_fwdInit);
	}
	virtual bool reversible() const override
	{
		return m_current != m_init;
	}

	virtual void update(const gui::Floats<2>& local_pos) override
	{
		float dv = local_pos[1] - m_target->at(m_index).value.get();
		float dt = local_pos[0] - m_target->at(m_index).time.get();
		m_current = dt < 0.0f ? dv / dt : std::copysign(1000.0f, -dv);

		execute();
	}
};


node::AnimationCurve::AnimationCurve(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data) :
	m_ctlr{ ctlr }, m_data{ data }
{
	assert(m_data);
	m_data->keys.addListener(*this);

	int i = 0;
	for (auto&& key : m_data->keys)
		newChild<AnimationKey>(*this, i++);
}

node::AnimationCurve::~AnimationCurve()
{
	m_data->keys.removeListener(*this);
	//we need the children destroyed while we still exist
	clearChildren();
}

void node::AnimationCurve::frame(gui::FrameDrawer& fd)
{
	assert(m_data && m_ctlr);

	//We can optimise later, just get this working first.
	//Later we might want to cache these values.
	//We should also do clip checking in y.

	//Should be recalculated on setting axis lims and transforms
	gui::Floats<2> lims = { (m_axisLims[0] - m_translation[0]) / m_scale[0], (m_axisLims[1] - m_translation[0]) / m_scale[0] };

	buildClip(lims, (fd.toGlobal(m_scale) - fd.toGlobal({ 0.0f, 0.0f })).abs());

	//Map out the clips to the time line
	int offset;//number of clips to offset
	int N;//repetitions

	bool clamp = m_ctlr->flags.hasRaised(CTLR_LOOP_CLAMP);
	float startTime = m_ctlr->startTime.get();
	float stopTime = m_ctlr->stopTime.get();

	if (clamp) {
		offset = 0;
		N = 1;
	}
	else {
		offset = static_cast<int>(std::floor((lims[0] - startTime) / m_clipLength));
		N = static_cast<int>(std::ceil((lims[1] - startTime) / m_clipLength)) - offset;
	}

	if (m_clipLength > 0.0f && lims[0] != lims[1]) {
		auto popper = fd.pushTransform(m_translation, m_scale);

		//To hold the final curve
		std::vector<gui::Floats<2>> curve;
		curve.reserve(N * m_clipPoints.size() + 2);

		//Only valid/relevant on Clamp
		if (clamp && lims[0] < startTime) {
			curve.push_back(fd.toGlobal({ lims[0], m_clipPoints.front()[1] }).floor());
			curve.push_back(fd.toGlobal(m_clipPoints.front()).floor());
		}

		for (int i = 0; i < N; i++) {
			float t_offset = (i + offset) * m_clipLength;

			//push t_offset as a transform and input points in global space directly?

			//transform limits to this clip space
			float lim0 = lims[0] - t_offset;
			float lim1 = lims[1] - t_offset;

			if (lim0 < startTime && lim1 > stopTime) {
				//Whole curve is visible (in x at least, let's worry about y later).
				for (int i = 0; i < (int)m_clipPoints.size(); i++) {
					gui::Floats<2> p = fd.toGlobal({ m_clipPoints[i][0] + t_offset, m_clipPoints[i][1] }).floor();
					if (curve.empty() || curve.back().matrix() != p.matrix())
						curve.push_back(p);
				}
			}
			else if (lim0 >= startTime) {
				//This is the start point of the curve (may also be the end!)
				//Skip if next is less than lim0

				int i = 0;
				//skip clipped beginning
				for (; i < (int)m_clipPoints.size() - 1 && m_clipPoints[i + 1][0] <= lim0; i++) {}

				//May also be clipped in upper end; include only as long as previous is less than lim1
				for (; i < (int)m_clipPoints.size(); i++) {
					if (i != 0 && m_clipPoints[i - 1][0] >= lim1)
						break;

					gui::Floats<2> p = fd.toGlobal({ m_clipPoints[i][0] + t_offset, m_clipPoints[i][1] }).floor();
					if (curve.empty() || curve.back().matrix() != p.matrix())
						curve.push_back(p);
				}
			}
			else {
				//This is the end of the curve (lim1 <= clip.length)
				//Include as long as previous is less than lim1
				//We skip the first point if it is a duplicate
				for (int i = 0; i < (int)m_clipPoints.size(); i++) {

					if (i != 0 && m_clipPoints[i - 1][0] >= lim1)
						break;

					gui::Floats<2> p = fd.toGlobal({ m_clipPoints[i][0] + t_offset, m_clipPoints[i][1] }).floor();
					if (curve.empty() || curve.back().matrix() != p.matrix())
						curve.push_back(p);
				}
			}
		}

		//Only valid/relevant on Clamp
		if (clamp && lims[1] > stopTime) {
			curve.push_back(fd.toGlobal({ lims[1], m_clipPoints.back()[1] }).floor());
		}

		gui::Floats<2> tl1{ fd.toGlobal({ lims[0], 0.0f })[0], std::numeric_limits<float>::max() };
		gui::Floats<2> br1{ fd.toGlobal({ startTime, 0.0f })[0], -std::numeric_limits<float>::max() };
		gui::Floats<2> tl2{ fd.toGlobal({ stopTime, 0.0f })[0], std::numeric_limits<float>::max() };
		gui::Floats<2> br2{ fd.toGlobal({ lims[1], 0.0f })[0], -std::numeric_limits<float>::max() };

		gui::ColRGBA lineCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		float lineWidth = 3.0f;
		if (curve.size() >= 2)
			fd.curve(curve, lineCol, lineWidth, true);

		fd.rectangle(tl1.floor(), br1.floor(), { 0.0f, 0.0f, 0.0f, 0.075f }, true);
		fd.rectangle(tl2.floor(), br2.floor(), { 0.0f, 0.0f, 0.0f, 0.075f }, true);

#ifdef _DEBUG
		gui::DebugWindow::print("Curve points: %d", curve.size());
#endif
	}
	else {
		fd.rectangle({ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() }, 
			{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() }, 
			{ 0.0f, 0.0f, 0.0f, 0.075f }, true);
	}

	Composite::frame(fd);
}

void node::AnimationCurve::onInsert(int pos)
{
	assert(pos >= 0 && size_t(pos) <= getChildren().size());

	//Insert handle at pos
	insertChild(pos, std::make_unique<AnimationKey>(*this, pos));

	//Handles after pos must update their index
	for (int i = pos + 1; (size_t)i < getChildren().size(); i++)
		static_cast<AnimationKey*>(getChildren()[i].get())->setIndex(i);

	//The handle right before pos must refresh its interpolation
	if (pos != 0)
		static_cast<AnimationKey*>(getChildren()[pos - 1].get())->setDirty();
}

void node::AnimationCurve::onErase(int pos)
{
	assert(pos >= 0 && size_t(pos) < getChildren().size());

	//The handle at i = pos is invalidated
	static_cast<AnimationKey*>(getChildren()[pos].get())->invalidate();

	//Erase invalidated handle
	eraseChild(pos);

	//Handles at i > pos must update their index
	for (int i = pos; (size_t)i < getChildren().size(); i++)
		static_cast<AnimationKey*>(getChildren()[i].get())->setIndex(i);

	//The handle at i = pos - 1 must refresh
	if (pos != 0)
		static_cast<AnimationKey*>(getChildren()[pos - 1].get())->setDirty();
}

void node::AnimationCurve::onMove(int from, int to)
{
	moveChild(from, to);

	//Handles in [from, to] must update their index
	assert(from != to);
	int low;
	int high;
	if (from > to) {
		low = to;
		high = from;
	}
	else {
		low = from;
		high = to;
	}

	for (int i = low; i < high + 1; i++)
		static_cast<AnimationKey*>(getChildren()[i].get())->setIndex(i);

	//The handle before min(from, to) must refresh
	if (low != 0)
		static_cast<AnimationKey*>(getChildren()[low - 1].get())->setDirty();
	//The handle at max(from, to) must refresh
	static_cast<AnimationKey*>(getChildren()[high].get())->setDirty();
}

node::AnimationKey* node::AnimationCurve::getActive() const
{
	AnimationKey* result = nullptr;
	for (auto&& child : getChildren()) {
		if (auto key = static_cast<AnimationKey*>(child.get()); key->getSelectionState() == SelectionState::ACTIVE) {
			result = key;
			break;
		}
	}
	return result;
}

std::vector<node::AnimationKey*> node::AnimationCurve::getSelected() const
{
	std::vector<AnimationKey*> result;
	for (auto&& child : getChildren()) {
		if (auto key = static_cast<AnimationKey*>(child.get()); key->getSelectionState() != SelectionState::NOT_SELECTED) {
			result.push_back(key);
		}
	}
	return result;
}

node::AnimationKey& node::AnimationCurve::animationKey(int i) const
{
	assert(i >= 0 && (size_t)i < getChildren().size());
	return *static_cast<AnimationKey*>(getChildren()[i].get());
}

ni_ptr<Vector<Key<float>>> node::AnimationCurve::getKeysPtr() const
{
	return make_ni_ptr(m_data, &NiFloatData::keys);
}

ni_ptr<Property<KeyType>> node::AnimationCurve::getTypePtr() const
{
	return make_ni_ptr(m_data, &NiFloatData::keyType);
}

gui::Floats<2> node::AnimationCurve::getBounds() const
{
	gui::Floats<2> result = { 0.0f, 0.0f };
	for (auto&& key : m_data->keys) {
		if (float val = key.value.get(); val < result[0])
			result[0] = key.value.get();
		else if (val > result[1])
			result[1] = key.value.get();
	}
	return result;
}

std::unique_ptr<gui::ICommand> node::AnimationCurve::getInsertOp(const gui::Floats<2>& pos) const
{
	//Locate the first key with larger time and insert before it.
	int index = 0;
	for (; index < (int)m_data->keys.size() && m_data->keys.at(index).time.get() < pos[0]; index++) {}

	return std::make_unique<InsertOp>(make_ni_ptr(m_data, &NiFloatData::keys), index, pos);
}

std::unique_ptr<gui::ICommand> node::AnimationCurve::getEraseOp(const std::vector<AnimationKey*>& keys) const
{
	std::vector<int> indices(keys.size());
	int i = 0;
	for (AnimationKey* key : keys) {
		assert(key && getChildren()[key->getIndex()].get() == key);//it must be one of our keys
		indices[i++] = key->getIndex();
	}

	return std::make_unique<EraseOp>(make_ni_ptr(m_data, &NiFloatData::keys), std::move(indices));
}

void node::AnimationCurve::buildClip(const gui::Floats<2>& lims, const gui::Floats<2>& resolution)
{
	KeyType type = m_data->keyType.get();
	bool reverse = m_ctlr->flags.hasRaised(CTLR_LOOP_REVERSE);
	float tStart = m_ctlr->startTime.get();
	float tStop = m_ctlr->stopTime.get();

	m_clipLength = reverse ? 2.0f * (tStop - tStart) : tStop - tStart;

	m_clipPoints.clear();

	if (m_clipLength <= 0.0f) {
		m_clipLength = 0.0f;
	}
	else if (m_data->keys.size() == 0) {
		m_clipPoints.push_back({ tStart, 0.0f });
		m_clipPoints.push_back({ reverse ? tStart + m_clipLength : tStop, 0.0f });
	}
	else {
		int stopReverse = -1;

		//if the first key is greater than tStart, special treatment is required
		if (float t = m_data->keys.front().time.get(); t > tStart) {
			m_clipPoints.push_back({ tStart, 0.0f });
			m_clipPoints.push_back({ std::min(t, tStop), 0.0f });
			stopReverse = 1;
		}

		//if the first key is greater than tStop, we're done
		int i = 0;
		if (m_data->keys.at(i).time.get() < tStop) {

			//Now proceed until the next key is greater than tStart, or we reach the last key.
			//This is the first relevant key.
			for (; i < (int)m_data->keys.size() - 1 && m_data->keys.at(i + 1).time.get() <= tStart; i++) {}

			//keys.at(i) is less than tStop

			for (; i < (int)m_data->keys.size(); i++) {

				float t = m_data->keys.at(i).time.get();

				float tBegin;
				float tauBegin;
				if (t < tStart) {
					tBegin = tStart;
					if (i < (int)m_data->keys.size() - 1)
						tauBegin = (tStart - t) / (m_data->keys.at(i + 1).time.get() - t);
					else
						tauBegin = 0.0f;
				}
				else {
					tBegin = t;
					tauBegin = 0.0f;
				}

				if (type == KEY_QUADRATIC) {
					//push from tBegin up to (not including) tEnd
					//if tEnd == tStop, push them too and break

					float tEnd;
					float tauEnd;

					if (i == (int)m_data->keys.size() - 1) {
						tEnd = tBegin;
						tauEnd = tauBegin;
					}
					else if (m_data->keys.at(i + 1).time.get() > tStop) {
						tEnd = tStop;
						tauEnd = (tStop - t) / (m_data->keys.at(i + 1).time.get() - t);
					}
					else {
						tEnd = m_data->keys.at(i + 1).time.get();
						tauEnd = 1.0f;
					}

					auto key = static_cast<AnimationKey*>(getChildren()[i].get());

					if (tEnd > tBegin) {

						assert(i < (int)m_data->keys.size() - 1);//sanity check
						float tNext = m_data->keys.at(i + 1).time.get();

						gui::Floats<2> tauExtr = key->getExtrema();

						//we may have 1, 2 or 3 intervals:
						std::array<float, 3> tauHigh{ tauExtr[0], tauExtr[1], tauEnd };
						float tauLow = tauBegin;
						float tLow = tBegin;
						for (int n = 0; n < 3; n++) {
							if (n == 2 || (!std::isnan(tauHigh[n]) && tauHigh[n] > tauLow && tauHigh[n] < tauEnd)) {
								float tHigh = t + tauHigh[n] * (tNext - t);

								float resolution_factor = 0.5f;//roughly twice the number of segments per pixel, on average
								float half_delta_t = 0.5f * (tHigh - tLow);
								float half_interval_pixels = half_delta_t * resolution[0];
								int segments = static_cast<int>(std::ceil(half_interval_pixels * resolution_factor));

								float half_delta_tau = 0.5f * (tauHigh[n] - tauLow);
								float disp_tau = 0.5f * (tauLow + tauHigh[n]);
								float disp_t = 0.5f * (tLow + tHigh);

								for (int j = 0; j < segments; j++) {
									//jth Chebyshev point (distributes more points around the extrema, but might be overkill)
									float t_j = std::cos(j * math::pi<float> / segments);

									//linspaced
									//float t_j = 1.0f - 2.0f * j / segments;

									float v = key->eval(disp_tau - half_delta_tau * t_j);
									m_clipPoints.push_back({ disp_t - half_delta_t * t_j, v });
								}

								tauLow = tauHigh[n];
								tLow = tHigh;
							}
						}
					}
					else {
						//we only need at tBegin
						m_clipPoints.push_back({ tBegin, key->eval(tauBegin) });
					}

					if (tEnd == tStop) {
						m_clipPoints.push_back({ tEnd, key->eval(tauEnd) });
						break;
					}
				}
				else if (type == KEY_LINEAR) {
					//push at tBegin
					//if t >= tStop, this is the last relevant key
					float v;

					if (tBegin >= tStop) {
						assert(i > 0);//sanity check
						tauBegin = (tStop - m_data->keys.at(i - 1).time.get()) / (tBegin - m_data->keys.at(i - 1).time.get());
						tBegin = tStop;
						v = static_cast<AnimationKey*>(getChildren()[i - 1].get())->eval(tauBegin);
					}
					else
						v = static_cast<AnimationKey*>(getChildren()[i].get())->eval(tauBegin);

					//we only need this check on the first push? No, the keys could be equal.
					if (m_clipPoints.empty() || m_clipPoints.back()[0] != tBegin || m_clipPoints.back()[1] != v)
						m_clipPoints.push_back({ tBegin, v });

					if (tBegin == tStop)
						break;
				}
				else {
					//Constant or unsupported
					//push at tBegin and at tEnd (unless equal, or last key)
					if (tBegin < tStop) {

						float v = m_data->keys.at(i).value.get();
						if (m_clipPoints.empty() || m_clipPoints.back()[0] != tBegin || m_clipPoints.back()[1] != v)
							m_clipPoints.push_back({ tBegin, v });

						if (i < (int)m_data->keys.size() - 1) {
							float t_next = m_data->keys.at(i + 1).time.get();
							if (t_next > tBegin && t_next < tStop) {
								m_clipPoints.push_back({ t_next, v });
							}
						}
					}
					else
						break;
				}
			}

			//if the last point is less than tStop, clamp it to the end
			if (m_clipPoints.back()[0] < tStop)
				m_clipPoints.push_back({ tStop, m_clipPoints.back()[1] });
		}

		if (reverse) {
			for (i = m_clipPoints.size() - 2; i > stopReverse; i--)
				m_clipPoints.push_back({ 2.0f * tStop - m_clipPoints[i][0], m_clipPoints[i][1] });

			if (i != -1)
				m_clipPoints.push_back({ m_clipLength + tStart, m_clipPoints.back()[1] });
		}
	}

}


node::AnimationKey::AnimationKey(AnimationCurve& curve, int index) :
	m_curve{ &curve },
	m_index{ index }
{
	if (key().fwdTan.get() != key().bwdTan.get())
		m_handleType = HandleType::FREE;

	key().time.addListener(*this);
	key().value.addListener(*this);
	m_translation = { key().time.get(), key().value.get() };

	newChild<CentreHandle>(*this);

	m_curve->keyType().addListener(*this);
	onSet(m_curve->keyType().get());
}

node::AnimationKey::~AnimationKey()
{
	m_curve->keyType().removeListener(*this);

	//Children need to unregister while we are still alive
	clearChildren();

	//We should have been invalidated if our key has been erased.
	if (!m_invalid) {
		key().time.removeListener(*this);
		key().value.removeListener(*this);
	}
}

void node::AnimationKey::setTranslation(const gui::Floats<2>& t)
{
	//is anyone actually calling this?
	key().time.set(t[0]);
	key().value.set(t[1]);
}

void node::AnimationKey::onSet(const float&)
{
	//We're listening to time and value. Don't care which one was set.
	m_translation = { key().time.get(), key().value.get() };
	setDirty();
	if (m_index != 0)
		m_curve->animationKey(m_index - 1).setDirty();
}

void node::AnimationKey::onSet(const KeyType& val)
{
	//Add/remove tangent handles
	assert(getChildren().size() >= 1);
	if (val == KEY_QUADRATIC) {
		if (getChildren().size() == 1) {
			newChild<BwdTangentHandle>(*this);
			newChild<FwdTangentHandle>(*this);
		}
	}
	else {
		//remove tangent handles, if any
		for (int i = (int)getChildren().size() - 1; i > 0; i--)
			eraseChild(i);
	}
	setDirty();
}

void node::AnimationKey::setIndex(int i)
{
	m_index = i;
	setDirty();
}

float node::AnimationKey::eval(float t)
{
	if (KeyType type = m_curve->keyType().get(); type == KEY_LINEAR) {
		if (t <= 0.0f) {
			return key().value.get();
		}
		else if (t >= 1.0f) {
			return m_index < (int)m_curve->keys().size() - 1 ? m_curve->keys().at(m_index + 1).value.get() : key().value.get();
		}
		else {
			float v0 = key().value.get();
			return m_index < (int)m_curve->keys().size() - 1 ? v0 + t * (m_curve->keys().at(m_index + 1).value.get() - v0) : v0;
		}
	}
	else if (type == KEY_QUADRATIC) {
		if (m_dirty)
			recalculate();

		if (t <= 0.0f) {
			return m_pLo[0];
		}
		else if (t < 0.5f) {
			return m_pLo[0] + t * (m_pLo[1] + t * m_pLo[2]);
		}
		else if (t < 1.0f) {
			return m_pHi[0] + t * (m_pHi[1] + t * m_pHi[2]);
		}
		else {
			return m_pHi[0] + m_pHi[1] + m_pHi[2];
		}
	}
	else //Constant or unsupported
		return key().value.get();
}

gui::Floats<2> node::AnimationKey::getExtrema()
{
	if (KeyType type = m_curve->keyType().get(); type == KEY_QUADRATIC) {
		if (m_dirty)
			recalculate();

		gui::Floats<2> result{ std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };
		int found = 0;

		if (float lo = -m_pLo[1] / (2 * m_pLo[2]); lo >= 0.0f && lo <= 0.5f)
			result[found++] = lo;

		if (float hi = -m_pHi[1] / (2 * m_pHi[2]); hi > 0.5f && hi <= 1.0f)
			result[found] = hi;

		return result;
	}
	else
		return { std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };
}

void node::AnimationKey::recalculate()
{
	m_pLo[0] = key().value.get();

	if (m_index < (int)m_curve->keys().size() - 1) {
		float h = m_curve->keys().at(m_index + 1).time.get() - key().time.get();
		float y1 = m_curve->keys().at(m_index + 1).value.get();
		float yp1 = m_curve->keys().at(m_index + 1).bwdTan.get() * h;

		m_pLo[1] = key().fwdTan.get() * h;

		m_pHi[1] = 4 * (y1 - m_pLo[0]) - m_pLo[1] - 2 * yp1;
		m_pHi[2] = 0.5f * (yp1 - m_pHi[1]);
		m_pHi[0] = y1 - m_pHi[1] - m_pHi[2];

		m_pLo[2] = m_pHi[1] + m_pHi[2] - m_pLo[1];
	}

	m_dirty = false;
}


//This will be sent to the command queue, and can safely be accessed after the widget is destroyed
struct KeyProperty
{
	const ni_ptr<Vector<Key<float>>> keys;
	int index;
	Property<float> Key<float>::* member;
};

template<>
struct util::property_traits<KeyProperty>
{
	using property_type = KeyProperty;
	using value_type = float;
	using get_type = float;

	static float get(property_type p)
	{
		return (p.keys->at(p.index).*p.member).get();
	}
	static void set(property_type p, float val)
	{
		(p.keys->at(p.index).*p.member).set(val);
	}
};


//Input widgets need to follow order changes, so it fetches the key from the parent component
struct KeyInputProperty
{
	node::KeyWidget* widget;
	Property<float> Key<float>::* member;
};

template<>
struct util::property_traits<KeyInputProperty>
{
	using property_type = KeyInputProperty;
	using value_type = float;
	using get_type = float;

	static float get(property_type p)
	{
		return (p.widget->key().*p.member).get();
	}
	static void set(property_type p, float val)
	{
		(p.widget->key().*p.member).set(val);
	}
};

template<>
struct gui::DefaultEventSink<KeyInputProperty>
{
	using value_type = float;

	void begin(const KeyInputProperty& p, IComponent*)
	{
		m_init = util::property_traits<KeyInputProperty>::get(p);
	}
	void update(KeyInputProperty& p, IComponent*, const value_type& val)
	{
		//updates will use whatever key the parent component is currently pointing at
		util::property_traits<KeyInputProperty>::set(p, val);
	}
	void end(KeyInputProperty& p, IComponent* source)
	{
		//Lock us to a specific key before queueing
		if (IInvoker* inv = source->getInvoker())
			inv->queue(std::make_unique<DefaultSetCommand<KeyProperty>>(
				KeyProperty{ p.widget->getKeysPtr(), p.widget->getIndex(), p.member },
				util::property_traits<KeyInputProperty>::get(p),
				m_init));
	}

	float m_init;
};

//The time input should use the same move operation as mouse dragging, to handle order changes.
struct KeyTimeProperty
{
	node::KeyWidget* widget;
};

template<>
struct util::property_traits<KeyTimeProperty>
{
	using property_type = KeyTimeProperty;
	using value_type = float;
	using get_type = float;

	static float get(property_type p)
	{
		return p.widget->key().time.get();
	}
	static void set(property_type p, float val)
	{
		p.widget->key().time.set(val);
	}
};

template<>
struct gui::DefaultEventSink<KeyTimeProperty>
{
	using value_type = float;

	void begin(const KeyTimeProperty& p, IComponent*)
	{
		m_init = p.widget->key().time.get();
		m_op = std::make_unique<CentreMoveOp>(
			p.widget->getKeysPtr(), 
			std::vector<int>{ p.widget->getIndex(), },
			gui::Floats<2>{ m_init, p.widget->key().value.get() });
	}
	void update(KeyTimeProperty& p, IComponent*, const value_type& val)
	{
		//here we let the move operation itself keep track of the key
		if (m_op)
			m_op->update({ val, p.widget->key().value.get() });
	}
	void end(KeyTimeProperty&, IComponent* source)
	{
		if (IInvoker* inv = source->getInvoker())
			inv->queue(std::move(m_op));
	}

	float m_init{ 0.0f };
	std::unique_ptr<CentreMoveOp> m_op;
};

//So should the tangents (this design is becoming very impractical...)
struct KeyFwdTanProperty
{
	node::KeyWidget* widget;
};

template<>
struct util::property_traits<KeyFwdTanProperty>
{
	using property_type = KeyFwdTanProperty;
	using value_type = float;
	using get_type = float;

	static float get(property_type p)
	{
		return p.widget->key().fwdTan.get();
	}
	static void set(property_type p, float val)
	{
		p.widget->key().fwdTan.set(val);
	}
};

template<>
struct gui::DefaultEventSink<KeyFwdTanProperty>
{
	using value_type = float;

	void begin(const KeyFwdTanProperty& p, IComponent*)
	{
		m_init = p.widget->key().fwdTan.get();
		m_op = std::make_unique<FwdMoveOp>(
			p.widget->getKeysPtr(),
			p.widget->getIndex(),
			p.widget->getHandleType() == node::AnimationKey::HandleType::ALIGNED);
	}
	void update(KeyFwdTanProperty& p, IComponent*, const value_type& val)
	{
		if (m_op)
			m_op->update({ p.widget->key().time.get() + 1.0f, p.widget->key().value.get() + val });
	}
	void end(KeyFwdTanProperty&, IComponent* source)
	{
		if (IInvoker* inv = source->getInvoker())
			inv->queue(std::move(m_op));
	}

	float m_init{ 0.0f };
	std::unique_ptr<FwdMoveOp> m_op;
};

struct KeyBwdTanProperty
{
	node::KeyWidget* widget;
};

template<>
struct util::property_traits<KeyBwdTanProperty>
{
	using property_type = KeyBwdTanProperty;
	using value_type = float;
	using get_type = float;

	static float get(property_type p)
	{
		return p.widget->key().bwdTan.get();
	}
	static void set(property_type p, float val)
	{
		p.widget->key().bwdTan.set(val);
	}
};

template<>
struct gui::DefaultEventSink<KeyBwdTanProperty>
{
	using value_type = float;

	void begin(const KeyBwdTanProperty& p, IComponent*)
	{
		m_init = p.widget->key().fwdTan.get();
		m_op = std::make_unique<BwdMoveOp>(
			p.widget->getKeysPtr(),
			p.widget->getIndex(),
			p.widget->getHandleType() == node::AnimationKey::HandleType::ALIGNED);
	}
	void update(KeyBwdTanProperty& p, IComponent*, const value_type& val)
	{
		if (m_op)
			m_op->update({ p.widget->key().time.get() - 1.0f, p.widget->key().value.get() - val });
	}
	void end(KeyBwdTanProperty&, IComponent* source)
	{
		if (IInvoker* inv = source->getInvoker())
			inv->queue(std::move(m_op));
	}

	float m_init{ 0.0f };
	std::unique_ptr<BwdMoveOp> m_op;
};

struct HandleTypeProperty
{
	node::KeyWidget* widget;
};

template<>
struct util::property_traits<HandleTypeProperty>
{
	using property_type = HandleTypeProperty;
	using value_type = bool;
	using get_type = bool;

	static bool get(property_type p)
	{
		return p.widget->getHandleType() == node::AnimationKey::HandleType::ALIGNED;
	}
	static void set(property_type p, bool aligned)
	{
		p.widget->setHandleType(
			aligned ? node::AnimationKey::HandleType::ALIGNED : node::AnimationKey::HandleType::FREE);
	}
};

node::KeyWidget::KeyWidget(AnimationKey& key) :
	m_key{ &key },
	m_type{ m_key->curve().getTypePtr() },
	m_keys{ m_key->curve().getKeysPtr() },
	m_index{ m_key->getIndex() }
{
	assert(m_type && m_keys);
	m_type->addListener(*this);
	m_keys->addListener(*this);

	newChild<gui::Text>("Key");

	auto time = newChild<gui::DragInput<float, 1, KeyTimeProperty>>(KeyTimeProperty{ this }, "Time");
	time->setSensitivity(0.01f);
	time->setNumberFormat("%.2f");

	auto val = newChild<gui::DragInput<float, 1, KeyInputProperty>>(KeyInputProperty{ this, &Key<float>::value }, "Value");
	val->setSensitivity(0.01f);
	val->setNumberFormat("%.2f");

	onSet(m_type->get());
}

node::KeyWidget::~KeyWidget()
{
	m_type->removeListener(*this);
	m_keys->removeListener(*this);
}

void node::KeyWidget::onInsert(int i)
{
	if (i <= m_index)
		m_index++;
}

void node::KeyWidget::onErase(int i)
{
	if (i < m_index)
		m_index--;
	else if (i == m_index) {
		m_key = nullptr;
		m_index = -1;
		clearChildren();
		m_type->removeListener(*this);
		m_keys->removeListener(*this);
	}
}

void node::KeyWidget::onMove(int from, int to)
{
	if (from == m_index)
		m_index = to;
	else if (from < m_index) {
		if (to >= m_index)
			m_index--;
	}
	else if (to <= m_index)
		m_index++;
}

void node::KeyWidget::onSet(const KeyType& type)
{
	//Title, time and value should never be removed
	assert(getChildren().size() >= 3);

	if (type == KEY_QUADRATIC) {
		//add tangents
		if (getChildren().size() == 3) {
			auto fwd = newChild<gui::DragInput<float, 1, KeyFwdTanProperty>>(KeyFwdTanProperty{ this }, "Fwd tangent");
			fwd->setSensitivity(0.01f);
			fwd->setNumberFormat("%.2f");

			auto bwd = newChild<gui::DragInput<float, 1, KeyBwdTanProperty>>(KeyBwdTanProperty{ this }, "Bwd tangent");
			bwd->setSensitivity(0.01f);
			bwd->setNumberFormat("%.2f");

			newChild<gui::Checkbox<bool, 1, HandleTypeProperty, node::Converter, gui::SyncEventSink>>(
				HandleTypeProperty{ this }, "Align tangents");
		}
	}
	else {
		//remove tangents
		for (int i = (int)getChildren().size() - 1; i > 2; i--)
			eraseChild(i);
	}
}


std::unique_ptr<gui::IComponent> node::KeyHandle::getWidget()
{
	return std::make_unique<KeyWidget>(*m_root);
}


void node::CentreHandle::frame(gui::FrameDrawer& fd)
{
	//We want to scale with PlotArea, not Axes. Not so easy right now. 
	//We'll just not scale at all.
	gui::Brush brush;
	if (auto state = getSelectionState(); state == SelectionState::ACTIVE)
		brush.colour = ACTIVE_COL;
	else if (state == SelectionState::SELECTED)
		brush.colour = SELECTED_COL;
	else
		brush.colour = UNSELECTED_COL;

	fd.setBrush(&brush);
	fd.drawCircle(fd.toGlobal(m_translation), 3.0f, true);
	fd.setBrush(nullptr);
}

std::unique_ptr<node::AnimationCurve::MoveOperation> node::CentreHandle::getMoveOp(
	std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos)
{
	std::vector<int> indices(keys.size());

	int i = 0;
	for (auto&& key : keys) {
		indices[i] = key->getIndex();
		i++;
	}

	return std::make_unique<CentreMoveOp>(m_root->curve().getKeysPtr(), std::move(indices), pos);
}

node::BwdTangentHandle::BwdTangentHandle(AnimationKey& root) : KeyHandle(root)
{
	assert(m_root);
	m_root->key().bwdTan.addListener(*this);
}

node::BwdTangentHandle::~BwdTangentHandle()
{
	if (m_root->valid())
		m_root->key().bwdTan.removeListener(*this);
}

void node::BwdTangentHandle::frame(gui::FrameDrawer& fd)
{
	//(t, v) of this key is (0, 0) in our space
	gui::Floats<2> tmp1 = fd.toGlobal({ 0.0f, 0.0f });
	gui::Floats<2> tmp2 = fd.toGlobal({ -1.0f, -m_root->key().bwdTan.get() });
	//tmp2 - tmp1 is some vector parallel with our desired handle
	gui::Floats<2> tmp3 = (tmp2 - tmp1).matrix().normalized().array();
	gui::Floats<2> tmp4 = tmp1 + HANDLE_LENGTH * (tmp2 - tmp1).matrix().normalized().array();
	gui::Floats<2> tmp5 = tmp1 + (HANDLE_LENGTH - HANDLE_RADIUS) * (tmp2 - tmp1).matrix().normalized().array();
	//need to set translation so we can be clicked
	m_translation = fd.toLocal(tmp4);

	//We want to scale with PlotArea, not Axes. Not so easy right now. 
	//We'll just not scale at all.
	gui::Pen pen;
	pen.width = 1.0f;
	if (auto state = getSelectionState(); state == SelectionState::ACTIVE)
		pen.colour = ACTIVE_COL;
	else if (state == SelectionState::SELECTED)
		pen.colour = SELECTED_COL;
	else
		pen.colour = UNSELECTED_COL;

	fd.setPen(&pen);
	fd.drawLine(tmp1, tmp5, true);
	fd.drawCircle(tmp4, HANDLE_RADIUS, true);
	fd.setPen(nullptr);
}

std::unique_ptr<node::AnimationCurve::MoveOperation> node::BwdTangentHandle::getMoveOp(
	std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos)
{
	//We ignore other selected items and only move ourselves
	return std::make_unique<BwdMoveOp>(
		m_root->curve().getKeysPtr(), m_root->getIndex(), m_root->getHandleType() == AnimationKey::HandleType::ALIGNED);
}

void node::BwdTangentHandle::onSet(const float&)
{
	if (int i = m_root->getIndex(); i != 0)
		m_root->curve().animationKey(i - 1).setDirty();
}


node::FwdTangentHandle::FwdTangentHandle(AnimationKey& root) : KeyHandle(root)
{
	assert(m_root);
	m_root->key().fwdTan.addListener(*this);
}

node::FwdTangentHandle::~FwdTangentHandle()
{
	if (m_root->valid())
		m_root->key().fwdTan.removeListener(*this);
}

void node::FwdTangentHandle::frame(gui::FrameDrawer& fd)
{
	//(t, v) of this key is (0, 0) in our space
	gui::Floats<2> tmp1 = fd.toGlobal({ 0.0f, 0.0f });
	gui::Floats<2> tmp2 = fd.toGlobal({ 1.0f, m_root->key().fwdTan.get() });
	//tmp2 - tmp1 is some vector parallel with our desired handle
	gui::Floats<2> tmp3 = (tmp2 - tmp1).matrix().normalized().array();
	gui::Floats<2> tmp4 = tmp1 + HANDLE_LENGTH * (tmp2 - tmp1).matrix().normalized().array();
	gui::Floats<2> tmp5 = tmp1 + (HANDLE_LENGTH - HANDLE_RADIUS) * (tmp2 - tmp1).matrix().normalized().array();
	//need to set translation so we can be clicked
	m_translation = fd.toLocal(tmp4);

	//We want to scale with PlotArea, not Axes. Not so easy right now. 
	//We'll just not scale at all.
	gui::Pen pen;
	pen.width = 1.0f;
	if (auto state = getSelectionState(); state == SelectionState::ACTIVE)
		pen.colour = ACTIVE_COL;
	else if (state == SelectionState::SELECTED)
		pen.colour = SELECTED_COL;
	else
		pen.colour = UNSELECTED_COL;

	fd.setPen(&pen);
	fd.drawLine(tmp1, tmp5, true);
	fd.drawCircle(tmp4, HANDLE_RADIUS, true);
	fd.setPen(nullptr);
}

std::unique_ptr<node::AnimationCurve::MoveOperation> node::FwdTangentHandle::getMoveOp(
	std::vector<AnimationKey*>&&, const gui::Floats<2>&)
{
	//We ignore other selected items and only move ourselves
	return std::make_unique<FwdMoveOp>(
		m_root->curve().getKeysPtr(), m_root->getIndex(), m_root->getHandleType() == AnimationKey::HandleType::ALIGNED);
}

void node::FwdTangentHandle::onSet(const float&)
{
	m_root->setDirty();
}
