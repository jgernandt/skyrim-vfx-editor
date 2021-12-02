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

class InsertOp final : public gui::ICommand
{
	const ni_ptr<Vector<Key<float>>> m_target;
	const int m_index;
	const gui::Floats<2> m_position;

public:
	InsertOp(const ni_ptr<Vector<Key<float>>>& target, int index, const gui::Floats<2>& pos) :
		m_target{ target }, m_index{ index }, m_position{ pos }
	{
		assert(target);
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

class MoveOp final : public gui::ICommand
{
	const ni_ptr<Vector<Key<float>>> m_target;
	const std::vector<int> m_indices;

	const std::vector<Eigen::Vector2f> m_to;
	const std::vector<Eigen::Vector2f> m_from;

public:
	MoveOp(const ni_ptr<Vector<Key<float>>>& target, std::vector<int>&& indices,
		std::vector<Eigen::Vector2f>&& to, std::vector<Eigen::Vector2f>&& from) :
		m_target{ target }, m_indices{ std::move(indices) }, m_to{ std::move(to) }, m_from{ std::move(from) }
	{
		assert(target);
	}

	virtual void execute() override
	{
		assert(m_indices.size() == m_to.size() && m_from.size() == m_to.size());
		for (size_t i = 0; i < m_indices.size(); i++) {
			m_target->at(m_indices[i]).time.set(m_to[i][0]);
			m_target->at(m_indices[i]).value.set(m_to[i][1]);
		}
	}
	virtual void reverse() override
	{
		for (size_t i = 0; i < m_indices.size(); i++) {
			m_target->at(m_indices[i]).time.set(m_from[i][0]);
			m_target->at(m_indices[i]).value.set(m_from[i][1]);
		}
	}
	virtual bool reversible() const override
	{
		return m_to != m_from;
	}
};

node::KeyHandle::KeyHandle(ni_ptr<Vector<Key<float>>>&& keys, int index) :
	m_timeLsnr{ &m_translation[0] },
	m_valueLsnr{ &m_translation[1] },
	m_keys{ std::move(keys) },
	//m_ctlr{ ctlr },
	m_index{ index }
{
	assert(m_keys);
	m_keys->at(m_index).time.addListener(m_timeLsnr);
	m_keys->at(m_index).value.addListener(m_valueLsnr);
	m_translation = { m_keys->at(m_index).time.get(), m_keys->at(m_index).value.get() };

	//In order to lock editing of the time of start/stop keys:
	//*listen to the start/stop time properties and set our time
	//*do not send an input widget for time with the active widget
	//*do not set translation in getMoveOp

	//if (m_index == 0)
	//	m_ctlr->startTime.addListener(*this);
	//if (m_index == m_keys->size() - 1)
	//	m_ctlr->stopTime.addListener(*this);
}

node::KeyHandle::~KeyHandle()
{
	assert(m_keys);

	//We should have been invalidated if our key has been erased.
	if (!m_invalid) {
		m_keys->at(m_index).time.removeListener(m_timeLsnr);
		m_keys->at(m_index).value.removeListener(m_valueLsnr);
	}

	//if (m_index == 0)
	//	m_ctlr->startTime.removeListener(*this);
	//if (m_index == m_keys->size() - 1)
	//	m_ctlr->stopTime.removeListener(*this);
}

void node::KeyHandle::frame(gui::FrameDrawer& fd)
{
	assert(m_keys);

	if (m_dirty) {
		//Recalculate our interpolation (if we were quadratic)
		m_dirty = false;
	}

	//We want to scale with PlotArea, not Axes. Not so easy right now. 
	//We'll just not scale at all.
	fd.circle(
		fd.toGlobal(m_translation),
		3.0f,
		m_selected ? m_active ? gui::ColRGBA{ 1.0f, 1.0f, 0.0f, 1.0f } : nif::COL_WHITE : nif::COL_BLACK,
		true);
}

void node::KeyHandle::setTranslation(const gui::Floats<2>& t)
{
	assert(m_keys);
	m_keys->at(m_index).time.set(t[0]);
	m_keys->at(m_index).value.set(t[1]);
}

void node::KeyHandle::onSet(const float& val)
{
	assert(m_keys);
	m_keys->at(m_index).time.set(val);
}

template<>
struct util::property_traits<node::KeyHandle::KeyProperty>
{
	using property_type = node::KeyHandle::KeyProperty;
	using value_type = float;
	using get_type = float;

	static float get(const property_type& t)
	{
		return (t.keys->at(t.index).*t.member).get();
	}
	static void set(property_type& t, float val)
	{
		(t.keys->at(t.index).*t.member).set(val);
	}
};

std::unique_ptr<gui::IComponent> node::KeyHandle::getActiveWidget() const
{
	assert(m_keys);

	auto root = std::make_unique<gui::Composite>();

	root->newChild<gui::Text>("Linear key");

	/*if (isStartKey()) {
		auto item = root->newChild<gui::Item>(std::make_unique<gui::MarginAlign>());
		item->newChild<gui::Text>("Time");
		item->newChild<gui::Number<float, ni_ptr<Property<float>>>>(make_ni_ptr(m_ctlr, &NiTimeController::startTime), "%.2f");
	}
	else if (isStopKey()) {
		auto item = root->newChild<gui::Item>(std::make_unique<gui::MarginAlign>());
		item->newChild<gui::Text>("Time");
		item->newChild<gui::Number<float, ni_ptr<Property<float>>>>(make_ni_ptr(m_ctlr, &NiTimeController::stopTime), "%.2f");
	}
	else {*/
	auto time = root->newChild<gui::DragInput<float, 1, KeyProperty>>(KeyProperty{ m_keys, m_index, &Key<float>::time }, "Time");
	time->setSensitivity(0.01f);
	time->setNumberFormat("%.2f");
	//}

	auto val = root->newChild<gui::DragInput<float, 1, KeyProperty>>(KeyProperty{ m_keys, m_index, &Key<float>::value }, "Value");
	val->setSensitivity(0.01f);
	val->setNumberFormat("%.2f");

	return root;
}

std::unique_ptr<gui::ICommand> node::KeyHandle::getMoveOp(
	const std::vector<std::pair<KeyHandle*, gui::Floats<2>>>& initial) const
{
	//Collect the indices from each selected key
	std::vector<int> indices(initial.size());
	//collect current values
	std::vector<Eigen::Vector2f> to(initial.size());
	//collect initial state
	std::vector<Eigen::Vector2f> from(initial.size());

	int i = 0;
	for (auto&& item : initial) {
		indices[i] = item.first->getIndex();
		to[i] = item.first->getTranslation();
		from[i] = item.second;

		//Forbid editing the time of start/stop keys
		//if (item.first->isStartKey() || item.first->isStopKey())
		//	to[i][0] = from[i][0];

		i++;
	}

	return std::make_unique<MoveOp>(m_keys, std::move(indices), std::move(to), std::move(from));
}

node::Interpolant node::KeyHandle::getInterpolant()
{
	assert(m_keys);

	float v0 = m_keys->at(m_index).value.get();

	return Interpolant(v0,
		m_index < (int)m_keys->size() - 1 ? m_keys->at(m_index + 1).value.get() - v0 : 0.0f);
}


node::AnimationCurve::AnimationCurve(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data) :
	m_ctlr{ ctlr }, m_data{ data }
{
	assert(m_data);
	m_data->keys.addListener(*this);

	int i = 0;
	for (auto&& key : m_data->keys)
		newChild<KeyHandle>(make_ni_ptr(m_data, &NiFloatData::keys), i++);
}

node::AnimationCurve::~AnimationCurve()
{
	m_data->keys.removeListener(*this);
}

void node::AnimationCurve::frame(gui::FrameDrawer& fd)
{
	assert(m_data && m_ctlr);

	struct Clip
	{
		int size()
		{
			return mirrored ? 2 * points.size() - 1 : points.size();
		}

		float time(int i) const
		{
			assert(i >= 0);
			if (mirrored && (size_t)i >= points.size()) {
				assert((size_t)i < 2 * points.size() - 1);
				return length() - points[2 * points.size() - i - 2][0];
			}
			else
				return points[i][0];
		}

		float value(int i) const
		{
			assert(i >= 0);
			if (mirrored && (size_t)i >= points.size()) {
				assert((size_t)i < 2 * points.size() - 1);
				return points[2 * points.size() - i - 2][1];
			}
			else
				return points[i][1];
		}

		gui::Floats<2>& front() { return points.front(); }
		gui::Floats<2>& back()
		{
			return points.back();
		}

		float length() const { return mirrored ? 2.0f * m_length : m_length; }
		void setLength(float f) { m_length = f; }

		std::vector<gui::Floats<2>> points;
		bool mirrored{ false };

	private:
		float m_length{ 0.0f };
	};

	float startTime = m_ctlr->startTime.get();
	float stopTime = m_ctlr->stopTime.get();

	Clip clip;
	clip.setLength(stopTime - startTime);

	//We want to draw from low axis limit to upper.
	//The phase/frequency should be our transform from our axes, so we work in clip space.
	gui::Floats<2> lims = { (m_axisLims[0] - m_translation[0]) / m_scale[0], (m_axisLims[1] - m_translation[0]) / m_scale[0] };

	if (m_data->keys.size() == 0 || clip.length() <= 0.0f || lims[0] == lims[1])
		return;//nothing to draw
	else if (m_data->keys.size() > 1) {

		//We can optimise later, just get this working first.
		//Later we might want to cache these values.
		//We should also do clip checking in y.

		//Work with the assumption that there is a key at start and stop. It's how it should be.
		//Start and stop time thus need to adjust to the time of the keys.
		//We'll make it work.

		clip.points.resize(m_data->keys.size());
		clip.points[0] = { 0.0f, m_data->keys.front().value.get() };

		for (int i = 1; i < (int)m_data->keys.size(); i++) {

			clip.points[i] = { m_data->keys.at(i).time.get() - startTime, m_data->keys.at(i).value.get() };

			//if we were quadratic we might instead:
			//*Recalculate the limits to global and determine the resolution of the interval.
			// From it, decide how many line segments we want to split it into.
			//*Call the Handle to evaluate the interpolant at the given points

			//If we were constant we should not produce a continuous curve at all. 
			//We should produce a list of lines.
		}

		//To hold the final curve
		std::vector<gui::Floats<2>> curve;

		//Map out the clips to the time line
		float time;//time of first clip
		int N;//repetitions

		ControllerFlags loop = m_ctlr->flags.raised() & CTLR_LOOP_MASK;
		clip.mirrored = loop == CTLR_LOOP_REVERSE;

		if (loop == CTLR_LOOP_CLAMP) {
			time = startTime;
			N = 1;
			curve.reserve(clip.size() + 2);
		}
		else {
			float d = lims[0] + std::fmod(startTime - lims[0], clip.length());

			time = d >= lims[0] ? d - clip.length() : d;
			N = static_cast<int>(std::ceil((lims[1] - time) / clip.length()));

			curve.reserve(clip.size() * N);
		}

		gui::ColRGBA lineCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		float lineWidth = 3.0f;

		//Only valid/relevant on Clamp
		if (loop == CTLR_LOOP_CLAMP && lims[0] < time) {
			curve.push_back({ lims[0], clip.front()[1] });
			curve.push_back(clip.front());
		}

		for (int i = 0; i < N; i++) {
			float begin = time + i * clip.length();
			float end = time + (i + 1) * clip.length();

			float lim0 = lims[0] - begin;//clip time of lim0
			float lim1 = lims[1] - begin;//clip time of lim1

			if (lim0 < 0.0f && lim1 > clip.length()) {
				//Whole curve is visible (in x at least, let's worry about y later).
				//We skip the first point if it is a duplicate
				assert(!curve.empty() && clip.size() > 0);
				int i = curve.back()[0] == begin + clip.time(0) && curve.back()[1] == clip.value(0) ? 1 : 0;
				for (; i < (int)clip.size(); i++)
					curve.push_back({ begin + clip.time(i), clip.value(i) });
			}
			else if (lim0 >= 0.0f) {
				//This is the start point of the curve (may also be the end!)
				//Skip if next is less than lim0

				int i = 0;
				//skip clipped beginning
				for (; i < (int)clip.size() - 1 && clip.time(i + 1) <= lim0; i++) {}

				//May also be clipped in upper end; include only as long as previous is less than lim1
				for (; i < (int)clip.size(); i++) {
					curve.push_back({ begin + clip.time(i), clip.value(i) });
					if (i != 0 && clip.time(i - 1) >= lim1)
						break;
				}
			}
			else {
				//This is the end of the curve (lim1 <= clip.length())
				//Include as long as previous is less than lim1
				//We skip the first point if it is a duplicate
				assert(!curve.empty() && clip.size() > 0);
				int i = curve.back()[0] == begin + clip.time(0) && curve.back()[1] == clip.value(0) ? 1 : 0;
				for (; i < (int)clip.size(); i++) {
					curve.push_back({ begin + clip.time(i), clip.value(i) });
					if (i != 0 && clip.time(i - 1) >= lim1)
						break;
				}
			}
		}

		//Only valid/relevant on Clamp
		if (loop == CTLR_LOOP_CLAMP && lims[1] > time + clip.length()) {
			curve.push_back({ lims[1], clip.back()[1] });
		}

		assert(curve.size() >= 2);

		gui::Floats<2> tl1;
		gui::Floats<2> br1;
		gui::Floats<2> tl2;
		gui::Floats<2> br2;

		{
			auto popper = fd.pushTransform(m_translation, m_scale);
			for (auto&& p : curve)
				p = fd.toGlobal(p);

			tl1 = { fd.toGlobal({ lims[0], 0.0f })[0], std::numeric_limits<float>::max() };
			br1 = { fd.toGlobal({ startTime, 0.0f })[0], -std::numeric_limits<float>::max() };
			tl2 = { fd.toGlobal({ stopTime, 0.0f })[0], std::numeric_limits<float>::max() };
			br2 = { fd.toGlobal({ lims[1], 0.0f })[0], -std::numeric_limits<float>::max() };
		}
		fd.curve(curve, lineCol, lineWidth, true);

		fd.rectangle(tl1, br1, { 0.0f, 0.0f, 0.0f, 0.075f }, true);
		fd.rectangle(tl2, br2, { 0.0f, 0.0f, 0.0f, 0.075f }, true);
	}

	Composite::frame(fd);
}

void node::AnimationCurve::onInsert(int pos)
{
	assert(pos >= 0 && size_t(pos) <= getChildren().size());

	//Handles after pos must update their index
	for (int i = pos; (size_t)i < getChildren().size(); i++)
		static_cast<KeyHandle*>(getChildren()[i].get())->setIndex(i + 1);

	//Insert handle at pos
	insertChild(pos, std::make_unique<KeyHandle>(make_ni_ptr(m_data, &NiFloatData::keys), pos));

	//The handle right before pos must refresh its interpolation
	if (pos != 0)
		static_cast<KeyHandle*>(getChildren()[pos - 1].get())->recalcIpln();
}

void node::AnimationCurve::onErase(int pos)
{
	assert(pos >= 0 && size_t(pos) < getChildren().size());

	//The handle at i = pos is invalidated
	static_cast<KeyHandle*>(getChildren()[pos].get())->invalidate();

	//Handles at i > pos must update their index
	for (int i = pos + 1; (size_t)i < getChildren().size(); i++)
		static_cast<KeyHandle*>(getChildren()[i].get())->setIndex(i - 1);

	//Erase invalidated handle
	eraseChild(pos);

	//The handle at i = pos - 1 must refresh
	if (pos != 0)
		static_cast<KeyHandle*>(getChildren()[pos - 1].get())->recalcIpln();
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
	//Forbid inserting at either end.
	int index = 0;
	for (; index < (int)m_data->keys.size() && m_data->keys.at(index).time.get() < pos[0]; index++) {}

	return std::make_unique<InsertOp>(make_ni_ptr(m_data, &NiFloatData::keys), index, pos);
}

