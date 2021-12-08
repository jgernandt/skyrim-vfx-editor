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
	assert(m_data && m_ctlr);
	m_data->keys.addListener(*this);
	m_ctlr->flags.addListener(*this);
	m_ctlr->startTime.addListener(*this);
	m_ctlr->stopTime.addListener(*this);

	int i = 0;
	for (auto&& key : m_data->keys)
		newChild<AnimationKey>(*this, i++);
}

node::AnimationCurve::~AnimationCurve()
{
	m_data->keys.removeListener(*this);
	m_ctlr->flags.removeListener(*this);
	m_ctlr->startTime.removeListener(*this);
	m_ctlr->stopTime.removeListener(*this);

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

	if (lims[0] == lims[1])
		return;

	//Rebuild clip if
	//*start/stop time or loop type has changed
	//*our global scale has changed
	//*any of our keys is dirty
	bool rebuild = false;
	gui::Floats<2> scale = fd.getCurrentScale() * m_scale;
	if (m_clipDirty)
		rebuild = true;
	else if (scale.matrix() != m_calcScale.matrix()) {
		rebuild = true;
	}
	else if (getChildren().empty())
		rebuild = true;
	else {
		for (auto&& child : getChildren()) {
			if (static_cast<AnimationKey*>(child.get())->getDirty()) {
				rebuild = true;
				break;
			}
		}
	}

	if (rebuild) {
		m_clipDirty = false;
		m_calcScale = scale;
		buildClip(lims, (fd.toGlobal(m_scale) - fd.toGlobal({ 0.0f, 0.0f })).abs());
	}

	if (m_clipLength > 0.0f) {

		//pixels per unit
		gui::Floats<2> resolution = (fd.toGlobal(m_scale) - fd.toGlobal({ 0.0f, 0.0f })).abs();

		auto popper = fd.pushTransform(m_translation, m_scale);

		float startTime = m_ctlr->startTime.get();
		float stopTime = m_ctlr->stopTime.get();

		//Rebuild curve if
		//*clip was rebuilt
		//*our global position has changed
		gui::Floats<2> global_pos = fd.getCurrentTranslation();
		if (rebuild || global_pos.matrix() != m_calcPos.matrix()) {
			m_calcPos = global_pos;
			m_curvePoints.clear();

			//Map out the clips to the time line
			int offset;//number of clips to offset
			int N;//repetitions

			bool clamp = m_ctlr->flags.hasRaised(CTLR_LOOP_CLAMP);

			if (clamp) {
				offset = 0;
				N = 1;
			}
			else {
				offset = static_cast<int>(std::floor((lims[0] - startTime) / m_clipLength));
				N = static_cast<int>(std::ceil((lims[1] - startTime) / m_clipLength)) - offset;
			}

			//Only valid/relevant on Clamp
			if (clamp && lims[0] < startTime) {
				float v = m_segments.front().key < 0 ? 0.0f : animationKey(m_segments.front().key).eval(m_segments.front().tauBegin);
				m_curvePoints.push_back(fd.toGlobal({ lims[0], v }).floor());
				m_curvePoints.push_back(fd.toGlobal({ m_segments.front().tBegin, v }).floor());
			}

			for (int clip = 0; clip < N; clip++) {
				float t_offset = (clip + offset) * m_clipLength;

				//push t_offset as a transform and input points in global space directly?
				auto popper2 = fd.pushTransform({ t_offset, 0.0f }, { 1.0f, 1.0f });

				//transform limits to this clip space
				float lim0 = lims[0] - t_offset;
				float lim1 = lims[1] - t_offset;

				if (lim0 < startTime && lim1 > startTime + m_clipLength) {
					//Whole curve is visible (in x at least, let's worry about y later).
					for (int i = 0; (size_t)i < m_segments.size(); i++) {
						addCurvePoints(fd, i, { lim0, lim1 }, resolution);
					}
				}
				else {
					//some segments are clipped
					for (int i = 0; (size_t)i < m_segments.size(); i++) {
						if (m_segments[i].tBegin < lim1 && m_segments[i].tEnd > lim0)
							addCurvePoints(fd, i, { lim0, lim1 }, resolution);
					}
				}
			}

			//Only valid/relevant on Clamp
			if (clamp && lims[1] > stopTime) {
				float v = m_segments.back().key < 0 ? 0.0f : animationKey(m_segments.back().key).eval(m_segments.back().tauEnd);
				if (m_curvePoints.empty())
					m_curvePoints.push_back(fd.toGlobal({ lims[0], v }).floor());
				m_curvePoints.push_back(fd.toGlobal({ lims[1], v }).floor());
			}

			//If the last key is exactly at the stop time (common), it will not get evaluated.
			//We do this just to clean it (preventing it from triggering a recalulation).
			if (keys().size() > 0 && keys().back().time.get() == stopTime)
				animationKey(keys().size() - 1).eval(0.0f);
		}

		gui::Floats<2> tl1{ fd.toGlobal({ lims[0], 0.0f })[0], std::numeric_limits<float>::max() };
		gui::Floats<2> br1{ fd.toGlobal({ startTime, 0.0f })[0], -std::numeric_limits<float>::max() };
		gui::Floats<2> tl2{ fd.toGlobal({ stopTime, 0.0f })[0], std::numeric_limits<float>::max() };
		gui::Floats<2> br2{ fd.toGlobal({ lims[1], 0.0f })[0], -std::numeric_limits<float>::max() };

		gui::ColRGBA lineCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		float lineWidth = 3.0f;
		if (m_curvePoints.size() >= 2)
			fd.curve(m_curvePoints, lineCol, lineWidth, true);

		fd.rectangle(tl1.floor(), br1.floor(), { 0.0f, 0.0f, 0.0f, 0.075f }, true);
		fd.rectangle(tl2.floor(), br2.floor(), { 0.0f, 0.0f, 0.0f, 0.075f }, true);

#ifdef _DEBUG
		gui::DebugWindow::print("Clip segments: %d", m_segments.size());
		gui::DebugWindow::print("Curve points: %d", m_curvePoints.size());
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

void node::AnimationCurve::onSet(const float&)
{
	m_clipDirty = true;
}

void node::AnimationCurve::onRaise(ControllerFlags flags)
{
	if (flags & CTLR_LOOP_MASK)
		m_clipDirty = true;
}

void node::AnimationCurve::onClear(ControllerFlags flags)
{
	if (flags & CTLR_LOOP_MASK)
		m_clipDirty = true;
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

	m_segments.clear();

	if (m_clipLength <= 0.0f) {
		m_clipLength = 0.0f;
	}
	else if (m_data->keys.size() == 0) {
		m_segments.push_back({ -1, tStart, reverse ? tStart + m_clipLength : tStop, 0.0f, 0.0f });
	}
	else {
		//if the first key is greater than tStart, special treatment is required
		if (float t = m_data->keys.front().time.get(); t > tStart)
			m_segments.push_back({ -1, tStart, std::min(t, tStop), 0.0f, 0.0f });

		//if the first key is greater than tStop, we're done
		int i = 0;
		if (m_data->keys.at(i).time.get() < tStop) {

			//Now proceed until the next key is greater than tStart, or we reach the last key.
			//This is the first relevant key.
			for (; i < (int)m_data->keys.size() - 1 && m_data->keys.at(i + 1).time.get() <= tStart; i++) {}

			//keys.at(i) is less than tStop

			for (; i < (int)m_data->keys.size(); i++) {

				float t = m_data->keys.at(i).time.get();

				//Determine limits (might need interpolation)
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

				float tEnd;
				float tauEnd;
				if (i == (int)m_data->keys.size() - 1) {
					tEnd = tStop;
					tauEnd = 1.0f;
				}
				else if (m_data->keys.at(i + 1).time.get() > tStop) {
					tEnd = tStop;
					tauEnd = (tStop - t) / (m_data->keys.at(i + 1).time.get() - t);
				}
				else {
					tEnd = m_data->keys.at(i + 1).time.get();
					tauEnd = 1.0f;
				}

				//If quadratic, split segments into subsegments at extrema
				if (type == KEY_QUADRATIC && i < (int)m_data->keys.size() - 1 && tEnd > tBegin) {

					gui::Floats<2> tauExtr = static_cast<AnimationKey*>(getChildren()[i].get())->getExtrema();

					//we may have 1, 2 or 3 intervals:
					std::array<float, 3> tauHigh{ tauExtr[0], tauExtr[1], tauEnd };
					float tauLow = tauBegin;
					float tLow = tBegin;
					for (int n = 0; n < 3; n++) {
						if (n == 2 || (!std::isnan(tauHigh[n]) && tauHigh[n] > tauLow && tauHigh[n] < tauEnd)) {

							float tHigh = t + tauHigh[n] * (m_data->keys.at(i + 1).time.get() - t);
							m_segments.push_back({ i, tLow, tHigh, tauLow, tauHigh[n] });

							tauLow = tauHigh[n];
							tLow = tHigh;
						}
					}
				}
				else
					m_segments.push_back({ i, tBegin, tEnd, tauBegin, tauEnd });

				if (tEnd == tStop)
					break;
			}
		}

		assert(!m_segments.empty());

		if (reverse) {
			for (i = (int)m_segments.size() - 1; i >= 0; i--) {
				m_segments.push_back({
					m_segments[i].key,
					2.0f * tStop - m_segments[i].tEnd,
					2.0f * tStop - m_segments[i].tBegin,
					m_segments[i].tauEnd,
					m_segments[i].tauBegin });
			}

			//Deal with the special segment we added when the first key was greater than tStart
			if (m_segments.back().key < 0 && m_segments.size() > 1) {
				m_segments.back().key = m_segments[1].key;
				m_segments.back().tauBegin = m_segments[1].tauBegin;
				m_segments.back().tauEnd = m_segments[1].tauBegin;
			}
		}
	}

}

void node::AnimationCurve::addCurvePoints(gui::FrameDrawer& fd, int i, const gui::Floats<2>& lims, const gui::Floats<2>& resolution)
{
	if (keyType().get() == KEY_QUADRATIC && 
		m_segments[i].key >= 0 &&
		m_segments[i].key < (int)keys().size() - 1)
	{
		assert(m_segments[i].tBegin < lims[1] && m_segments[i].tEnd > lims[0]);

		float t0 = std::max(m_segments[i].tBegin, lims[0]);
		float t1 = std::min(m_segments[i].tEnd, lims[1]);

		float tau0;
		float tau1;
		if (t0 != m_segments[i].tBegin) {
			float k = (lims[0] - m_segments[i].tBegin) / (m_segments[i].tEnd - m_segments[i].tBegin);
			tau0 = m_segments[i].tauBegin + k * (m_segments[i].tauEnd - m_segments[i].tauBegin);
		}
		else
			tau0 = m_segments[i].tauBegin;

		if (t1 != m_segments[i].tEnd) {
			float k = (lims[1] - m_segments[i].tBegin) / (m_segments[i].tEnd - m_segments[i].tBegin);
			tau1 = m_segments[i].tauBegin + k * (m_segments[i].tauEnd - m_segments[i].tauBegin);
		}
		else
			tau1 = m_segments[i].tauEnd;

		float resolution_factor = 0.25f;//roughly the number of segments per pixel
		float tInterval = (t1 - t0);
		float interval_pixels = std::abs(tInterval * resolution[0]);
		int segments = static_cast<int>(std::ceil(interval_pixels * resolution_factor));
		float delta_t = tInterval / segments;
		float delta_tau = (tau1 - tau0) / segments;

		assert(segments > 0);
		float v0 = animationKey(m_segments[i].key).eval(tau0);
		gui::Floats<2> p = fd.toGlobal({ t0, v0 }).floor();
		if (m_curvePoints.empty() || m_curvePoints.back().matrix() != p.matrix())
			m_curvePoints.push_back(p);

		for (int j = 1; j < segments + 1; j++) {
			float v = animationKey(m_segments[i].key).eval(tau0 + j * delta_tau);
			m_curvePoints.push_back(fd.toGlobal({ t0 + j * delta_t, v }).floor());
		}
	}
	else {
		float v0 = m_segments[i].key < 0 ? 0.0f : animationKey(m_segments[i].key).eval(m_segments[i].tauBegin);
		gui::Floats<2> p = fd.toGlobal({ m_segments[i].tBegin, v0 }).floor();
		if (m_curvePoints.empty() || m_curvePoints.back().matrix() != p.matrix())
			m_curvePoints.push_back(p);

		if (m_segments[i].tEnd != m_segments[i].tBegin) {
			float v1 = m_segments[i].key < 0 ? 0.0f : animationKey(m_segments[i].key).eval(m_segments[i].tauEnd);
			m_curvePoints.push_back(fd.toGlobal({ m_segments[i].tEnd, v1 }).floor());
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
	KeyType type = m_curve->keyType().get();
	if (type == KEY_QUADRATIC) {
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
	else {
		m_dirty = false;
		if (type == KEY_LINEAR) {
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
		else
			return key().value.get();
	}
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
	else {
		m_pLo[1] = 0.0f;
		m_pLo[2] = 0.0f;
		m_pHi[0] = m_pLo[0];
		m_pHi[1] = 0.0f;
		m_pHi[2] = 0.0f;
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
