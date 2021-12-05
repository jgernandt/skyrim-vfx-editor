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

class MoveOp final : public node::AnimationCurve::MoveOperation
{
	const ni_ptr<Vector<Key<float>>> m_target;

	std::vector<gui::Floats<2>> m_initPos;
	std::vector<int> m_initI;

	std::vector<gui::Floats<2>> m_currentPos;
	std::vector<int> m_currentI;

	std::vector<int> m_finalI;

	gui::Floats<2> m_move{ 0.0f, 0.0f };
	bool m_dirty{ false };

public:
	MoveOp(const ni_ptr<Vector<Key<float>>>& target, std::vector<int>&& indices) :
		m_target{ target }, m_initI{ std::move(indices) }
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

	virtual void update(const gui::Floats<2>& local_move) override
	{
		static_assert(SINGLE_THREAD);

		if (local_move.matrix() != m_move.matrix()) {
			if (float delta = local_move[0] - m_move[0]; delta >= 0.0f) {
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

	buildClip(lims, 0.0f);

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

	if (m_clipLength <= 0.0f || lims[0] == lims[1])
		return;//nothing to draw
	else {

		//To hold the final curve
		std::vector<gui::Floats<2>> curve;
		curve.reserve(N * m_clipPoints.size() + 2);

		//Only valid/relevant on Clamp
		if (clamp && lims[0] < startTime) {
			curve.push_back({ lims[0], m_clipPoints.front()[1] });
			curve.push_back(m_clipPoints.front());
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
					float t = m_clipPoints[i][0] + t_offset;
					float v = m_clipPoints[i][1];
					if (curve.empty() || curve.back()[0] != t || curve.back()[1] != v)
						curve.push_back({ t, v });
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

					float t = m_clipPoints[i][0] + t_offset;
					float v = m_clipPoints[i][1];
					if (curve.empty() || curve.back()[0] != t || curve.back()[1] != v)
						curve.push_back({ t, v });
				}
			}
			else {
				//This is the end of the curve (lim1 <= clip.length)
				//Include as long as previous is less than lim1
				//We skip the first point if it is a duplicate
				for (int i = 0; i < (int)m_clipPoints.size(); i++) {

					if (i != 0 && m_clipPoints[i - 1][0] >= lim1)
						break;

					float t = m_clipPoints[i][0] + t_offset;
					float v = m_clipPoints[i][1];
					if (curve.empty() || curve.back()[0] != t || curve.back()[1] != v)
						curve.push_back({ t, v });
				}
			}
		}

		//Only valid/relevant on Clamp
		if (clamp && lims[1] > stopTime) {
			curve.push_back({ lims[1], m_clipPoints.back()[1] });
		}

		gui::Floats<2> tl1;
		gui::Floats<2> br1;
		gui::Floats<2> tl2;
		gui::Floats<2> br2;

		{
			//should round to pixel and remove duplicates?

			auto popper = fd.pushTransform(m_translation, m_scale);
			for (auto&& p : curve)
				p = fd.toGlobal(p);

			tl1 = { fd.toGlobal({ lims[0], 0.0f })[0], std::numeric_limits<float>::max() };
			br1 = { fd.toGlobal({ m_ctlr->startTime.get(), 0.0f })[0], -std::numeric_limits<float>::max() };
			tl2 = { fd.toGlobal({ m_ctlr->stopTime.get(), 0.0f })[0], std::numeric_limits<float>::max() };
			br2 = { fd.toGlobal({ lims[1], 0.0f })[0], -std::numeric_limits<float>::max() };
		}
		gui::ColRGBA lineCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		float lineWidth = 3.0f;
		if (curve.size() >= 2)
			fd.curve(curve, lineCol, lineWidth, true);

		fd.rectangle(tl1, br1, { 0.0f, 0.0f, 0.0f, 0.075f }, true);
		fd.rectangle(tl2, br2, { 0.0f, 0.0f, 0.0f, 0.075f }, true);
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
		static_cast<AnimationKey*>(getChildren()[low].get())->setDirty();
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

void node::AnimationCurve::buildClip(const gui::Floats<2>& lims, float resolution)
{
	bool reverse = m_ctlr->flags.hasRaised(CTLR_LOOP_REVERSE);
	float tStart = m_ctlr->startTime.get();
	float tStop = m_ctlr->stopTime.get();

	m_clipLength = reverse ? 2.0f * (tStop - tStart) : tStop - tStart;

	m_clipPoints.clear();

	if (m_data->keys.size() == 0) {
		m_clipPoints.push_back({ tStart, 0.0f });
		m_clipPoints.push_back({ reverse ? tStart + m_clipLength : tStop, 0.0f });
	}
	else {
		//We assume strictly increasing time values. Might enforce that later.
		//Come to think of it, we should probably just assume non-decreasing.
		//There may be valid uses for equal-time keys.

		int stopReverse = -1;

		//if the first key is greater than tStart, special treatment is required
		if (float t = m_data->keys.front().time.get(); t > tStart) {
			m_clipPoints.push_back({ tStart, 0.0f });
			m_clipPoints.push_back({ std::min(t, tStop), 0.0f });
			stopReverse = 1;
		}

		//Now proceed until the next key is not less than tStart, or we reach the last key
		int i = 0;
		for (; i < (int)m_data->keys.size() - 1 && m_data->keys.at(i + 1).time.get() < tStart; i++) {}
		int first = i;//save for mirroring

		//evaluate this and every following key, clamped to tStart (only really needed for the first)
		//break if we pass tStop
		for (; i < (int)m_data->keys.size(); i++) {
			if (float t = m_data->keys.at(i).time.get(); t < tStop) {
				//if this is not the last key and t is less than tStart, interpolate
				float tau;
				if (i < (int)m_data->keys.size() - 1 && t < tStart) {
					tau = (tStart - t) / (m_data->keys.at(i + 1).time.get() - t);
				}
				else
					tau = 0.0f;

				t = std::max(t, tStart);

				//Feels stupid with this indirection here, but it will help with quadratics
				float v = static_cast<AnimationKey*>(getChildren()[i].get())->eval(tau);

				if (m_clipPoints.empty() || m_clipPoints.back()[0] != t || m_clipPoints.back()[1] != v)
					m_clipPoints.push_back({ t, v });
			}
			else {
				//evaluate clamped to tStop
				//if this is the first key, we have what we need already
				if (i > 0) {
					float tau;
					if (t > tStop) {
						tau = (tStop - m_data->keys.at(i - 1).time.get()) / (t - m_data->keys.at(i - 1).time.get());
						t = tStop;
					}
					else
						tau = 1.0f;

					float v = static_cast<AnimationKey*>(getChildren()[i - 1].get())->eval(tau);

					if (m_clipPoints.empty() || m_clipPoints.back()[0] != t || m_clipPoints.back()[1] != v)
						m_clipPoints.push_back({ t, v });
				}
				break;
			}
		}

		//if the last point is less than tStop, clamp it to the end
		if (m_clipPoints.back()[0] < tStop)
			m_clipPoints.push_back({ tStop, m_clipPoints.back()[1] });

		if (reverse) {
			for (i = m_clipPoints.size() - 2; i > stopReverse; i--)
				m_clipPoints.push_back({ 2.0f * tStop - m_clipPoints[i][0], m_clipPoints[i][1] });

			if (i != -1)
				m_clipPoints.push_back({ m_clipLength + tStart, m_clipPoints.back()[1] });
		}

		//if we were quadratic we might instead:
		//*Recalculate the limits to global and determine the resolution of the interval.
		// From it, decide how many line segments we want to split it into.
		//*Call the Handle to evaluate the interpolant at the given points

		//If we were constant we should not produce a continuous curve at all. 
		//We should produce a list of lines.
	}

}


node::AnimationKey::AnimationKey(AnimationCurve& curve, int index) :
	m_curve{ &curve },
	m_index{ index }
{
	key().time.addListener(*this);
	key().value.addListener(*this);
	m_translation = { key().time.get(), key().value.get() };

	newChild<CentreHandle>(*this);
}

node::AnimationKey::~AnimationKey()
{
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
	//We're listening to both time and value. Don't care which one was set.
	m_translation = { key().time.get(), key().value.get() };
	m_dirty = true;
}

float node::AnimationKey::eval(float t)
{
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
		m_op = std::make_unique<MoveOp>(p.widget->getKeysPtr(), std::vector<int>{ p.widget->getIndex() });
	}
	void update(KeyTimeProperty&, IComponent*, const value_type& val)
	{
		//here we let the move operation itself keep track of the key
		if (m_op)
			m_op->update({ val - m_init, 0.0f });
	}
	void end(KeyTimeProperty&, IComponent* source)
	{
		if (IInvoker* inv = source->getInvoker())
			inv->queue(std::move(m_op));
	}

	float m_init{ 0.0f };
	std::unique_ptr<MoveOp> m_op;
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
		if (getChildren().size() < 4) {
			auto fwd = newChild<gui::DragInput<float, 1, KeyInputProperty>>(KeyInputProperty{ this, &Key<float>::fwdTan }, "Fwd tangent");
			fwd->setSensitivity(0.01f);
			fwd->setNumberFormat("%.2f");

			auto bwd = newChild<gui::DragInput<float, 1, KeyInputProperty>>(KeyInputProperty{ this, &Key<float>::bwdTan }, "Bwd tangent");
			bwd->setSensitivity(0.01f);
			bwd->setNumberFormat("%.2f");
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
	gui::ColRGBA col;
	if (auto state = getSelectionState(); state == SelectionState::ACTIVE)
		col = ACTIVE_COL;
	else if (state == SelectionState::SELECTED)
		col = SELECTED_COL;
	else
		col = UNSELECTED_COL;

	fd.circle(fd.toGlobal(m_translation), 3.0f, col, true);
}

std::unique_ptr<node::AnimationCurve::MoveOperation> node::CentreHandle::getMoveOp(
	std::vector<AnimationKey*>&& keys)
{
	std::vector<int> indices(keys.size());

	int i = 0;
	for (auto&& key : keys) {
		indices[i] = key->getIndex();
		i++;
	}

	return std::make_unique<MoveOp>(m_root->curve().getKeysPtr(), std::move(indices));
}
