//Copyright 2021, 2022 Jonas Gernandt
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
		//Transform target rectangle to our local space
		if (m_current.empty()) {
			m_current.push({});
			m_current.top() << c.fromGlobalSpace(m_rect.head(2)), c.fromGlobalSpace(m_rect.tail(2));
		}
		else {
			const gui::Floats<4>& prev = m_current.top();
			m_current.push({});
			m_current.top() << c.fromParentSpace(prev.head(2)), c.fromParentSpace(prev.tail(2));
		}

		//If the cursor was inside our rectangle, pass on to our children
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
	//The target rectangle in the local space of the current component
	std::stack<gui::Floats<4>> m_current;
	//The target rectangle in global space (could be pushed onto m_current at start? Only if we start from global space?)
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
	m_ctlr{ ctlr }, 
	m_cycleTypeAdapter{ make_ni_ptr(ctlr, &NiTimeController::flags) }
{
	assert(ctlr && data);

	setSize({ 800.0f, 480.0f });

	auto plot_panel = newChild<gui::Subwindow>();
	plot_panel->setSize({ 640.0f, 464.0f });
	plot_panel->setTranslation({ 8.0f, 16.0f });
	m_plot = plot_panel->newChild<gui::Plot>();

	//Add a component that carries clip-space transforms
	auto clipTransform = std::make_unique<ClipTransformer>(
		make_ni_ptr(ctlr, &NiTimeController::phase),
		make_ni_ptr(ctlr, &NiTimeController::frequency));

	//Add clip limit indicators
	clipTransform->addChild(
		std::make_unique<ClipLimits>(
			make_ni_ptr(ctlr, &NiTimeController::startTime),
			make_ni_ptr(ctlr, &NiTimeController::stopTime)));

	//Add curve
	auto series = std::make_unique<AnimationCurve>(
		data,
		m_cycleTypeAdapter.getProperty(),
		make_ni_ptr(ctlr, &NiTimeController::startTime),
		make_ni_ptr(ctlr, &NiTimeController::stopTime));
	m_curve = series.get();
	clipTransform->insertChild(0, std::move(series));

	m_plot->getPlotArea().getAxes().addChild(std::move(clipTransform));

	//determine limits
	gui::Floats<2> xlims = 
		(gui::Floats<2>{ ctlr->startTime.get(), ctlr->stopTime.get() } - ctlr->phase.get()) / ctlr->frequency.get();
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

	if (m_curve) {
		gui::Floats<2> ylims = m_curve->getBounds();
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

	m_plot->getPlotArea().addKeyListener(*this);
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

	side_panel->newChild<gui::Text>("Clip");
	using selector_type2 = gui::Selector<ControllerFlags, BitSetWrapper<ControllerFlags, 2, 1>>;
	side_panel->newChild<selector_type2>(
		BitSetWrapper<ControllerFlags, 2, 1>{ make_ni_ptr(ctlr, &NiTimeController::flags) },
		std::string(),
		selector_type2::ItemList{ { 0, "Repeat" }, { 1, "Reverse" }, { 2, "Clamp" } });

	auto fr = side_panel->newChild<DragFloat>(make_ni_ptr(ctlr, &NiTimeController::frequency), "Frequency");
	fr->setSensitivity(0.1f);
	fr->setLowerLimit(0.01f);
	fr->setUpperLimit(100.0f);
	fr->setLogarithmic();
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
}

node::FloatKeyEditor::~FloatKeyEditor()
{
	m_plot->getPlotArea().removeKeyListener(*this);
}

void node::FloatKeyEditor::onClose()
{
	asyncInvoke<gui::RemoveChild>(this, getParent(), false);
}

void node::FloatKeyEditor::onKeyDown(gui::key_t key)
{
	if (m_currentOp == Op::NONE) {
		if (key == 'X' || key == gui::KEY_DEL) {
			//Erase selected

			//for each selected curve:
			//(except it should be packed into one command - we'll figure that out when we get there)
			if (gui::IInvoker* inv = getInvoker())
				inv->queue(m_curve->getEraseOp(m_curve->getSelected()));
		}
	}
}

bool node::FloatKeyEditor::onMouseDown(gui::Mouse::Button button)
{
	assert(m_plot);

	if (m_currentOp != Op::NONE)
		return true;
	else if (button == gui::Mouse::Button::LEFT) {
		if (gui::Keyboard::isDown(gui::KEY_CTRL)) {
			//InsertOp

			//clicked point in key space
			gui::Floats<2> p = m_curve->fromGlobalSpace(gui::Mouse::getPosition());

			if (gui::IInvoker* inv = getInvoker())
				inv->queue(m_curve->getInsertOp(p));

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

				//get curve from object (need to if we have more than one)
				auto curve = object->getCurve();
				//get selection state from object
				auto state = object->getSelectionState();

				if (gui::Keyboard::isDown(gui::KEY_SHIFT)) {

					if (state == SelectionState::NOT_SELECTED) {
						//shift+click unselected = add to selection and make active

						//set curve active
						//set object selected/active (how? directly or via curve?)
						//who removes active state on currently active key?
						if (auto active = curve->getActive())
							active->setSelectionState(SelectionState::SELECTED);

						curve->setSelectionState(SelectionState::ACTIVE);
						object->setSelectionState(SelectionState::ACTIVE);

						//get widget for object
						m_activePanel->clearChildren();
						m_activePanel->addChild(object->getWidget());
					}
					else {
						//shift+click selected = remove from selection

						if (curve->getSelectionState() == SelectionState::ACTIVE)//if we had many curves
							if (object->getSelectionState() == SelectionState::ACTIVE)
								m_activePanel->clearChildren();

						object->setSelectionState(SelectionState::NOT_SELECTED);//should this report to curve?
						//curve->setSelectionState(SelectionState::NOT_SELECTED);//or should we set ourselves?
					}
				}
				else {
					if (state == SelectionState::NOT_SELECTED) {
						//click unselected = select (clear other selected) and start dragging

						//clear all selections
						for (auto selected : curve->getSelected())
							selected->setSelectionState(SelectionState::NOT_SELECTED);
						
						//select object
						curve->setSelectionState(SelectionState::ACTIVE);
						object->setSelectionState(SelectionState::ACTIVE);

						//get widget for object
						m_activePanel->clearChildren();
						m_activePanel->addChild(object->getWidget());
					}
					else {
						//click selected = set active and start dragging
						//if drag threshold is not passed, clear other selected

						if (auto active = curve->getActive())
							active->setSelectionState(SelectionState::SELECTED);

						curve->setSelectionState(SelectionState::ACTIVE);
						object->setSelectionState(SelectionState::ACTIVE);

						//get widget for object
						m_activePanel->clearChildren();
						m_activePanel->addChild(object->getWidget());
					}

					//save temp data
					gui::Mouse::setCapture(&m_plot->getPlotArea());
					m_clickPoint = gui::Mouse::getPosition();
					m_currentOp = Op::DRAG;

					m_clicked = object;
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
		if (gui::Keyboard::isDown(gui::KEY_CTRL)) {
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
			if (m_op) {
				if (gui::IInvoker* inv = getInvoker())
					inv->queue(std::move(m_op));
			}
			else {
				//they released before reaching the drag threshold, should clear
				//other selected objects

				if (m_clicked) {
					//clear all selections
					for (auto selected : m_clicked->getCurve()->getSelected())
						selected->setSelectionState(SelectionState::NOT_SELECTED);

					//reselect object
					m_clicked->setSelectionState(SelectionState::ACTIVE);

					//active widget should have been set already
				}
				//else the move command was issued by an as of yet not existing alternative input
			}

			m_clicked = nullptr;
			m_currentOp = Op::NONE;
			assert(m_plot && gui::Mouse::getCapture() == &m_plot->getPlotArea());
			gui::Mouse::setCapture(nullptr);
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
	if (m_currentOp == Op::NONE) {
		assert(m_plot);

		float scaleFactor = std::pow(SCALE_BASE, delta);

		gui::Floats<2> pivot = m_plot->getPlotArea().fromGlobalSpace(gui::Mouse::getPosition());
		gui::Floats<2> T = m_plot->getPlotArea().getAxes().getTranslation();
		m_plot->getPlotArea().getAxes().setTranslation(pivot - (pivot - T) * scaleFactor);
		m_plot->getPlotArea().getAxes().scale({ scaleFactor, scaleFactor });

		updateAxisUnits();
	}
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
	if (m_op)
		m_op->update(m_curve->fromGlobalSpace(pos));
	else if (m_clicked && (std::abs(pos[0] - m_clickPoint[0]) >= DRAG_THRESHOLD ||
		std::abs(pos[1] - m_clickPoint[1]) >= DRAG_THRESHOLD))
	{
		m_op = m_clicked->getMoveOp(m_clicked->getCurve()->getSelected(), m_curve->fromGlobalSpace(m_clickPoint));
		m_op->update(m_curve->fromGlobalSpace(pos));
	}
}

void node::FloatKeyEditor::pan(const gui::Floats<2>& pos)
{
	assert(m_plot);
	gui::Floats<2> delta = m_plot->getPlotArea().fromGlobalSpace(pos) - m_clickPoint;
	m_plot->getPlotArea().getAxes().setTranslation(m_startT + delta);
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

node::FloatKeyEditor::CycleTypeAdapter::CycleTypeAdapter(const ni_ptr<FlagSet<ControllerFlags>>& flags) :
	m_cycleType{ std::make_shared<Property<CycleType>>() }, m_flags{ flags }
{
	assert(m_flags);
	m_cycleType->addListener(*this);
	m_flags->addListener(*this);
	onRaise(m_flags->raised());
}

node::FloatKeyEditor::CycleTypeAdapter::~CycleTypeAdapter()
{
	m_cycleType->removeListener(*this);
	m_flags->removeListener(*this);
}

void node::FloatKeyEditor::CycleTypeAdapter::onSet(const CycleType& c)
{
	switch (c) {
	case CycleType::REPEAT:
		m_flags->clear(CTLR_LOOP_MASK);
		break;
	case CycleType::REVERSE:
		m_flags->clear(CTLR_LOOP_CLAMP);
		m_flags->raise(CTLR_LOOP_REVERSE);
		break;
	case CycleType::CLAMP:
		m_flags->clear(CTLR_LOOP_REVERSE);
		m_flags->raise(CTLR_LOOP_CLAMP);
		break;
	}
}

void node::FloatKeyEditor::CycleTypeAdapter::onRaise(ControllerFlags flags)
{
	if (flags & CTLR_LOOP_MASK) {
		ControllerFlags raised = m_flags->raised() & CTLR_LOOP_MASK;
		if (raised == CTLR_LOOP_REVERSE)
			m_cycleType->set(CycleType::REVERSE);
		else if (raised == CTLR_LOOP_CLAMP)
			m_cycleType->set(CycleType::CLAMP);
		else {
			assert(raised == CTLR_LOOP_MASK);
			//This shouldn't happen, but it could. Just deal with it.
			//Setting CLAMP will clear the REVERSE flag, which will try to set CLAMP again (which will be ignored).
			m_cycleType->set(CycleType::CLAMP);
		}
	}
}

void node::FloatKeyEditor::CycleTypeAdapter::onClear(ControllerFlags flags)
{
	if (flags & CTLR_LOOP_MASK) {
		ControllerFlags raised = m_flags->raised() & CTLR_LOOP_MASK;
		if (raised == CTLR_LOOP_REVERSE)
			m_cycleType->set(CycleType::REVERSE);
		else if (raised == CTLR_LOOP_CLAMP)
			m_cycleType->set(CycleType::CLAMP);
		else {
			//This is the normal case. The other two are just safeguards against a pathological case.
			assert(raised == 0);
			m_cycleType->set(CycleType::REPEAT);
		}
	}
}
