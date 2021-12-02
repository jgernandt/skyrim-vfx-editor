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
#include "KeyEditor.h"
#include "CompositionActions.h"
#include "widget_types.h"

constexpr float DRAG_THRESHOLD = 5.0f;

constexpr float SCALE_BASE = 1.1f;
constexpr float SCALE_SENSITIVITY = 0.1f;

using namespace nif;

//Move somewhere else?
template<typename T, T Width, T Offset>
struct BitSetWrapper
{
	constexpr static T MASK = ~(~T(0) << Width) << Offset;
	nif::ni_ptr<nif::FlagSet<T>> ptr;

	T get() const
	{
		return ptr ? (ptr->raised() & MASK) >> Offset : T();
	}
	void set(T val)
	{
		if (ptr) {
			ptr->clear(~(val << Offset) & MASK);
			ptr->raise((val << Offset) & MASK);
		}
	}
};

template<typename T, T Width, T Offset>
struct util::property_traits<BitSetWrapper<T, Width, Offset>>
{
	using property_type = BitSetWrapper<T, Width, Offset>;
	using value_type = T;
	using get_type = T;

	static T get(const BitSetWrapper<T, Width, Offset>& p) { return p.get(); }
	static void set(BitSetWrapper<T, Width, Offset>& p, T val) { p.set(val); }
};

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

//Locate a component whose center is in the vicinity of the cursor
class ClickSelector final : public gui::DescendingVisitor
{
public:
	//expects a point in global space
	ClickSelector(const gui::Floats<2>& globalPoint)
	{
		m_rect << globalPoint - 5.0f, globalPoint + 5.0f;
		m_rect = m_rect.floor();
	}

	static_assert(TO_PIXEL == static_cast<decltype(TO_PIXEL)>(&std::floor));

	virtual void visit(gui::Composite& c) override
	{
		if (m_current.empty()) {
			m_current.push({});
			m_current.top() << c.fromGlobalSpace(m_rect.head(2)), c.fromGlobalSpace(m_rect.tail(2));
		}
		else {
			const gui::Floats<4>& prev = m_current.top();
			m_current.push({});
			m_current.top() << c.fromParentSpace(prev.head(2)), c.fromParentSpace(prev.tail(2));
		}

		gui::Floats<2> size = c.getSize();
		gui::Floats<2> pos = 0.5f * gui::Floats<2>(m_current.top()[2] + m_current.top()[0], m_current.top()[3] + m_current.top()[1]).abs();
		if ((size[0] == 0.0f || size[1] == 0.0f) || //Treat 0 size as infinite. Makes sense?
			(pos[0] >= 0.0f && pos[0] <= size[0] && pos[1] >= 0.0f && pos[1] <= size[1])) {
			for (auto&& child : c.getChildren()) {
				child->accept(*this);
				if (result != nullptr)
					break;
			}
		}
		m_current.pop();
	}
	virtual void visit(gui::Component& c) override
	{
		gui::Floats<2> pos = c.getTranslation();
		const gui::Floats<4>& rect = m_current.empty() ? m_rect : m_current.top();

		float minx;
		float maxx;
		if (rect[0] < rect[2]) {
			minx = rect[0];
			maxx = rect[2];
		}
		else {
			minx = rect[2];
			maxx = rect[0];
		}
		float miny;
		float maxy;
		if (rect[1] < rect[3]) {
			miny = rect[1];
			maxy = rect[3];
		}
		else {
			miny = rect[3];
			maxy = rect[1];
		}
		if (maxx >= pos[0] && minx <= pos[0] && maxy >= pos[1] && miny <= pos[1])
			result = &c;
	}

	gui::IComponent* result{ nullptr };
private:
	std::stack<gui::Floats<4>> m_current;
	gui::Floats<4> m_rect;
};

//Locate all the components whose centres are inside the rectangle
class BoxSelector final : public gui::DescendingVisitor
{
public:
	BoxSelector(const gui::Floats<4>& rect) {}

	virtual void visit(gui::Composite& c) override {}
	virtual void visit(gui::Component& c) override {}

	std::vector<gui::IComponent*> found;
};

node::FloatKeyEditor::FloatKeyEditor(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data) :
	m_ctlr{ ctlr }, m_freqLsnr{ ctlr ? &ctlr->phase : nullptr }, m_phaseLsnr{ ctlr ? &ctlr->frequency : nullptr }
{
	assert(ctlr && data);

	setSize({ 800.0f, 480.0f });

	auto plot_panel = newChild<gui::Subwindow>();
	plot_panel->setSize({ 640.0f, 464.0f });
	plot_panel->setTranslation({ 8.0f, 16.0f });
	m_plot = plot_panel->newChild<gui::Plot>();

	auto series = std::make_unique<DataSeries>(ctlr, data);
	m_data = series.get();
	m_plot->getPlotArea().getAxes().addChild(std::move(series));
	m_data->addListener(*this);

	//determine limits
	gui::Floats<2> xlims = { ctlr->startTime.get(), ctlr->stopTime.get() };
	if (float diff = xlims[1] - xlims[0]; diff > 0.0f) {
		xlims[0] -= 0.25f * diff;
		xlims[1] += 0.25f * diff;
	}
	else if (diff == 0.0f) {
		//start time == stop time
		//this may be valid (but stupid)
		xlims[0] -= 0.5f;
		xlims[1] += 0.5f;
	}
	else {
		//stop time < start time (error)
		//it's not really our business to fix this, we just need to deal with it
		float tmp = xlims[0];
		xlims[0] = xlims[1] - 0.25f * diff;
		xlims[1] = tmp + 0.25f * diff;
	}
	m_plot->getPlotArea().setXLimits(xlims);

	if (m_data) {
		gui::Floats<2> ylims = m_data->getBounds();
		if (float diff = ylims[1] - ylims[0]; diff > 0.0f) {
			ylims[0] -= 0.25f * diff;
			ylims[1] += 0.25f * diff;
		}
		else {
			assert(diff == 0.0f);
			ylims[0] -= 0.5f;
			ylims[1] += 0.5f;
		}
		m_plot->getPlotArea().setYLimits(ylims);
	}

	updateAxisUnits();
	m_data->setAxisLimits(xlims);

	m_plot->getPlotArea().setMouseHandler(this);

	//Side panel(s)
	auto side_panel = newChild<gui::Subwindow>();
	side_panel->setSize({ 142.0f, 200.0f });
	side_panel->setTranslation({ 648.0f, 16.0f });

	side_panel->newChild<gui::Text>("Interpolation");
	using selector_type = gui::Selector<KeyType, ni_ptr<Property<KeyType>>>;
	auto selector = side_panel->newChild<selector_type>(make_ni_ptr(data, &NiFloatData::keyType), std::string(),
		selector_type::ItemList{
			{ KEY_CONSTANT, "Constant" },
			{ KEY_LINEAR, "Linear" },
			{ KEY_QUADRATIC, "Quadratic" } });

	side_panel->newChild<gui::VerticalSpacing>();

	side_panel->newChild<gui::Text>("Cycle");
	using selector_type2 = gui::Selector<ControllerFlags, BitSetWrapper<ControllerFlags, 2, 1>>;
	side_panel->newChild<selector_type2>(
		BitSetWrapper<ControllerFlags, 2, 1>{ make_ni_ptr(ctlr, &NiTimeController::flags) },
		std::string(),
		selector_type2::ItemList{ { 0, "Repeat" }, { 1, "Reverse" }, { 2, "Clamp" } });

	auto fr = side_panel->newChild<DragFloat>(make_ni_ptr(ctlr, &NiTimeController::frequency), "Frequency");
	fr->setSensitivity(0.01f);
	fr->setLowerLimit(0.0f);
	fr->setAlwaysClamp(true);
	fr->setNumberFormat("%.2f");

	auto ph = side_panel->newChild<DragFloat>(make_ni_ptr(ctlr, &NiTimeController::phase), "Phase");
	ph->setSensitivity(0.01f);
	ph->setNumberFormat("%.2f");

	side_panel->newChild<gui::VerticalSpacing>();

	auto st = side_panel->newChild<DragFloat>(make_ni_ptr(ctlr, &NiTimeController::startTime), "Start time");
	st->setSensitivity(0.01f);
	st->setNumberFormat("%.2f");

	auto sp = side_panel->newChild<DragFloat>(make_ni_ptr(ctlr, &NiTimeController::stopTime), "Stop time");
	sp->setSensitivity(0.01f);
	sp->setNumberFormat("%.2f");

	//Active component panel
	m_activePanel = newChild<gui::Subwindow>();
	m_activePanel->setSize({ 142.0f, 200.0f });
	m_activePanel->setTranslation({ 648.0f, 220.0f });
	
	//Clip-space transform listeners
	m_freqLsnr.setTarget(m_data);
	ctlr->frequency.addListener(m_freqLsnr);
	m_freqLsnr.onSet(ctlr->frequency.get());

	m_phaseLsnr.setTarget(m_data);
	ctlr->phase.addListener(m_phaseLsnr);
	m_phaseLsnr.onSet(ctlr->phase.get());
}

node::FloatKeyEditor::~FloatKeyEditor()
{
	assert(m_ctlr);
	m_ctlr->phase.removeListener(m_phaseLsnr);

	if (m_data)
		m_data->removeListener(*this);
}

void node::FloatKeyEditor::onClose()
{
	asyncInvoke<gui::RemoveChild>(this, getParent(), false);
}

void node::FloatKeyEditor::setActiveKey(Selection::iterator key)
{
	//To continue what we've done so far, we should send all composition
	//changes to the invoker.

	assert(m_activePanel);
	if (m_activeItem != m_selection.end()) {
		m_activePanel->clearChildren();
		(*m_activeItem)->setActive(false);
	}

	if (key != m_selection.end()) {
		m_activePanel->addChild((*key)->getActiveWidget());
		(*key)->setActive(true);
	}
	m_activeItem = key;
}

bool node::FloatKeyEditor::onMouseDown(gui::Mouse::Button button)
{
	assert(m_plot);

	if (m_currentOp != Op::NONE)
		return true;
	else if (button == gui::Mouse::Button::LEFT) {
		if (gui::Keyboard::isDown(gui::Keyboard::Key::CTRL)) {
			//InsertOp

			//clicked point in key space
			gui::Floats<2> p = m_data->fromGlobalSpace(gui::Mouse::getPosition());

			if (gui::IInvoker* inv = getInvoker())
				inv->queue(m_data->getInsertOp(p));

			return true;
		}
		else {
			//MoveOp
			
			//locate the clicked object
			ClickSelector v(gui::Mouse::getPosition());
			m_plot->getPlotArea().accept(v);

			//I'm not sure how much type info we want to have (or require).
			//Let's do a downcast for now
			KeyHandle* object = dynamic_cast<KeyHandle*>(v.result);

			if (object) {
				if (gui::Keyboard::isDown(gui::Keyboard::Key::SHIFT)) {
					if (auto res = m_selection.insert(object); !res.second) {
						//shift+clicked selected object
						//remove from selection
						if (m_activeItem == res.first)
							setActiveKey(m_selection.end());
						m_selection.erase(res.first);
						object->setSelected(false);
					}
					else {
						//shift+clicked non-selected object
						//add to selection
						object->setSelected(true);
						setActiveKey(res.first);
					}
				}
				else {
					if (auto it = m_selection.find(object); it != m_selection.end()) {
						//clicked selected object
						//start dragging selection (if threshold is passed)

						//if we release before reaching the drag threshold, should clear
						//other selected objects
						setActiveKey(it);
					}
					else {
						//clicked non-selected object
						//set selection and start dragging (if threshold is passed)
						for (auto&& o : m_selection)
							o->setSelected(false);
						setActiveKey(m_selection.end());
						m_selection.clear();
						setActiveKey(m_selection.insert(object).first);
						object->setSelected(true);
					}
					//start dragging
					gui::Mouse::setCapture(&m_plot->getPlotArea());
					m_clickPoint = gui::Mouse::getPosition();
					m_dragThresholdPassed = false;
					m_currentOp = Op::DRAG;
				}
			}
			//else clear selection? Or not?

			return true;
		}
	}
	else if (button == gui::Mouse::Button::MIDDLE) {
		assert(gui::Mouse::getCapture() == nullptr);
		m_clickPoint = m_plot->getPlotArea().fromGlobalSpace(gui::Mouse::getPosition());
		gui::Mouse::setCapture(&m_plot->getPlotArea());
		if (gui::Keyboard::isDown(gui::Keyboard::Key::CTRL)) {
			m_currentOp = Op::ZOOM;
			m_startS = m_plot->getPlotArea().getAxes().getScale();
			m_startT = m_plot->getPlotArea().getAxes().getTranslation();
		}
		else {
			m_currentOp = Op::PAN;
			m_startT = m_plot->getPlotArea().getAxes().getTranslation();
		}

		return true;
	}
	else
		return false;
}

bool node::FloatKeyEditor::onMouseUp(gui::Mouse::Button button)
{
	bool result = false;

	if (button == gui::Mouse::Button::LEFT) {
		if (result = m_currentOp == Op::DRAG) {
			if (!m_dragThresholdPassed) {
				//they released before reaching the drag threshold, should clear
				//other selected objects
				Selection newSelection;
				m_activeItem = newSelection.insert(m_selection.extract(m_activeItem)).position;
				for (auto&& obj : m_selection) {
					assert(obj);
					obj->setSelected(false);
				}
				m_selection = std::move(newSelection);
			}
			else {
				//send final move op
				if (gui::IInvoker* inv = getInvoker())
					inv->queue((*m_activeItem)->getMoveOp(m_initialState));
			}

			m_currentOp = Op::NONE;
			assert(m_plot && gui::Mouse::getCapture() == &m_plot->getPlotArea());
			gui::Mouse::setCapture(nullptr);
			m_initialState.clear();
		}
	}
	else if (button == gui::Mouse::Button::MIDDLE) {
		if (result = (m_currentOp == Op::PAN || m_currentOp == Op::ZOOM)) {
			assert(m_plot && gui::Mouse::getCapture() == &m_plot->getPlotArea());
			gui::Mouse::setCapture(nullptr);
			m_currentOp = Op::NONE;
		}
	}
	
	return result;
}

bool node::FloatKeyEditor::onMouseWheel(float delta)
{
	assert(m_plot);

	float scaleFactor = std::pow(SCALE_BASE, delta);

	gui::Floats<2> pivot = m_plot->getPlotArea().fromGlobalSpace(gui::Mouse::getPosition());
	gui::Floats<2> T = m_plot->getPlotArea().getAxes().getTranslation();
	m_plot->getPlotArea().getAxes().setTranslation(pivot - (pivot - T) * scaleFactor);
	m_plot->getPlotArea().getAxes().scale({ scaleFactor, scaleFactor });

	updateAxisUnits();
	m_data->setAxisLimits(m_plot->getPlotArea().getXLimits());

	return true;
}

void node::FloatKeyEditor::onMouseMove(const gui::Floats<2>& pos)
{
	switch (m_currentOp) {
	case Op::DRAG:
		drag(pos);
		break;
	case Op::PAN:
		pan(pos);
		break;
	case Op::ZOOM:
		zoom(pos);
		break;
	}
}

void node::FloatKeyEditor::drag(const gui::Floats<2>& pos)
{
	assert(!m_selection.empty());//should be checked before starting a move op

	if (!m_dragThresholdPassed &&
		(std::abs(pos[0] - m_clickPoint[0]) >= DRAG_THRESHOLD ||
		std::abs(pos[1] - m_clickPoint[1]) >= DRAG_THRESHOLD))
	{
		m_dragThresholdPassed = true;

		//save initial state
		assert(m_initialState.empty());
		m_initialState.reserve(m_selection.size());
		for (KeyHandle* obj : m_selection) {
			assert(obj);
			m_initialState.push_back({ obj, obj->getTranslation() });
		}
	}

	if (m_dragThresholdPassed) {
		gui::Floats<2> delta = pos - m_clickPoint;

		for (auto&& item : m_initialState) {
			if (IComponent* parent = item.first->getParent()) {
				//Do we forego our previous design of sending everything to the Invoker?
				//If we do any synchronous access to the data, the Property themselves should guard.
				//Then again, we don't know if a listener wants to make changes to the graph in response.
				//Let's forego it, but make sure we can easily find everywhere we have done so:
				static_assert(FOREGO_ASYNC);

				gui::Floats<2> global = parent->toGlobalSpace(item.second);

				//Forbid editing the time of start/stop keys
				//if (item.first->isStartKey() || item.first->isStopKey())
				//	item.first->setTranslation(parent->fromGlobalSpace(global + gui::Floats<2>{ 0.0f, delta[1] }));
				//else
					item.first->setTranslation(parent->fromGlobalSpace(global + delta));
			}
		}
	}
}

void node::FloatKeyEditor::pan(const gui::Floats<2>& pos)
{
	assert(m_plot);
	gui::Floats<2> delta = m_plot->getPlotArea().fromGlobalSpace(pos) - m_clickPoint;
	m_plot->getPlotArea().getAxes().setTranslation(m_startT + delta);

	m_data->setAxisLimits(m_plot->getPlotArea().getXLimits());
}

void node::FloatKeyEditor::zoom(const gui::Floats<2>& pos)
{
	assert(m_plot);
	gui::Floats<2> delta = m_plot->getPlotArea().fromGlobalSpace(pos) - m_clickPoint;
	gui::Floats<2> factor;
	factor[0] = std::pow(SCALE_BASE, delta[0] * SCALE_SENSITIVITY);
	factor[1] = std::pow(SCALE_BASE, -delta[1] * SCALE_SENSITIVITY);

	m_plot->getPlotArea().getAxes().setTranslation(m_clickPoint - (m_clickPoint - m_startT) * factor);
	m_plot->getPlotArea().getAxes().setScale(m_startS * factor);

	updateAxisUnits();
	m_data->setAxisLimits(m_plot->getPlotArea().getXLimits());
}

void node::FloatKeyEditor::updateAxisUnits()
{
	assert(m_plot);

	//major unit should be 1, 2 or 5 to some power of 10
	gui::Floats<2> major;
	gui::Floats<2> xlims = m_plot->getPlotArea().getXLimits();
	gui::Floats<2> ylims = m_plot->getPlotArea().getYLimits();
	gui::Floats<2> estimate{ 0.5f * std::abs(xlims[1] - xlims[0]), 0.5f * std::abs(ylims[1] - ylims[0]) };
	//gui::Floats<2> estimate = 300.0f / m_area.getAxes().getScale().abs();//problematic before the first frame
	gui::Floats<2> power = estimate.log10().floor();
	gui::Floats<2> oom = { std::pow(10.0f, power[0]), std::pow(10.0f, power[1]) };
	if (estimate[0] >= 5.0f * oom[0])
		major[0] = 5.0f * oom[0];
	else if (estimate[0] >= 2.0f * oom[0])
		major[0] = 2.0f * oom[0];
	else
		major[0] = oom[0];

	if (estimate[1] >= 5.0f * oom[1])
		major[1] = 5.0f * oom[1];
	else if (estimate[1] >= 2.0f * oom[1])
		major[1] = 2.0f * oom[1];
	else
		major[1] = oom[1];

	//minor unit should be major / 4 (2?)
	gui::Floats<2> minor = major / 4.0f;

	m_plot->getPlotArea().getAxes().setMajorUnits(major);
	m_plot->getPlotArea().getAxes().setMinorUnits(minor);
}

void node::FloatKeyEditor::onAddChild(gui::IComponent* c, gui::Component* source)
{
	//Our active widget might be pointing at the wrong key. Refresh it.
	//There are cleaner ways of doing this, but it's no big deal.
	if (m_activeItem != m_selection.end()) {
		assert(m_activePanel);
		m_activePanel->clearChildren();
		m_activePanel->addChild((*m_activeItem)->getActiveWidget());
	}
}

void node::FloatKeyEditor::onRemoveChild(gui::IComponent* c, gui::Component* source)
{
	//If c was selected, it must be removed immediately (it may no longer exist).

	//Regardless, we should refresh our active widget. It may be pointing to 
	//the wrong (and possibly invalid) key.

	bool erased = false;

	if (m_activeItem != m_selection.end()) {
		assert(m_activePanel);
		m_activePanel->clearChildren();

		if (*m_activeItem == c) {
			m_selection.erase(m_activeItem);
			m_activeItem = m_selection.end();
			erased = true;
		}
		else
			m_activePanel->addChild((*m_activeItem)->getActiveWidget());
	}

	if (!erased)
		if (auto it = m_selection.find(static_cast<KeyHandle*>(c)); it != m_selection.end())
			m_selection.erase(it);
}

//Phase and frequency are the inverse transform of the data series
void node::FloatKeyEditor::FrequencyListener::onSet(const float& freq)
{
	assert(m_phase);
	if (m_target) {
		m_target->setScaleX(1.0f / freq);
		m_target->setTranslationX(-m_phase->get() / freq);
	}
}

void node::FloatKeyEditor::PhaseListener::onSet(const float& phase)
{
	assert(m_frequency);
	if (m_target)
		m_target->setTranslationX(-phase / m_frequency->get());
}


node::FloatKeyEditor::KeyHandle::KeyHandle(ni_ptr<Vector<Key<float>>>&& keys, int index) :
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

node::FloatKeyEditor::KeyHandle::~KeyHandle()
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

void node::FloatKeyEditor::KeyHandle::frame(gui::FrameDrawer& fd)
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

void node::FloatKeyEditor::KeyHandle::setTranslation(const gui::Floats<2>& t)
{
	assert(m_keys);
	m_keys->at(m_index).time.set(t[0]);
	m_keys->at(m_index).value.set(t[1]);
}

void node::FloatKeyEditor::KeyHandle::onSet(const float& val)
{
	assert(m_keys);
	m_keys->at(m_index).time.set(val);
}

template<>
struct util::property_traits<node::FloatKeyEditor::KeyHandle::KeyProperty>
{
	using property_type = node::FloatKeyEditor::KeyHandle::KeyProperty;
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

std::unique_ptr<gui::IComponent> node::FloatKeyEditor::KeyHandle::getActiveWidget() const
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

std::unique_ptr<gui::ICommand> node::FloatKeyEditor::KeyHandle::getMoveOp(
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

node::FloatKeyEditor::Interpolant node::FloatKeyEditor::KeyHandle::getInterpolant()
{
	assert(m_keys);

	float v0 = m_keys->at(m_index).value.get();

	return Interpolant(v0,
		m_index < (int)m_keys->size() - 1 ? m_keys->at(m_index + 1).value.get() - v0 : 0.0f);
}


node::FloatKeyEditor::DataSeries::DataSeries(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data) :
	m_ctlr{ ctlr }, m_data{ data }
{
	assert(m_data);
	m_data->keys.addListener(*this);

	int i = 0;
	for (auto&& key : m_data->keys)
		newChild<KeyHandle>(make_ni_ptr(m_data, &NiFloatData::keys), i++);
}

node::FloatKeyEditor::DataSeries::~DataSeries()
{
	m_data->keys.removeListener(*this);
}

void node::FloatKeyEditor::DataSeries::frame(gui::FrameDrawer& fd)
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

void node::FloatKeyEditor::DataSeries::onInsert(int pos)
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

void node::FloatKeyEditor::DataSeries::onErase(int pos)
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

gui::Floats<2> node::FloatKeyEditor::DataSeries::getBounds() const
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

std::unique_ptr<gui::ICommand> node::FloatKeyEditor::DataSeries::getInsertOp(const gui::Floats<2>& pos) const
{
	//Locate the first key with larger time and insert before it.
	//Forbid inserting at either end.
	int index = 0;
	for (; index < (int)m_data->keys.size() && m_data->keys.at(index).time.get() < pos[0]; index++) {}

	return std::make_unique<InsertOp>(make_ni_ptr(m_data, &NiFloatData::keys), index, pos);
}
