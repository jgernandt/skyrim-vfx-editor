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

	auto series = std::make_unique<AnimationCurve>(ctlr, data);
	m_data = series.get();
	m_plot->getPlotArea().getAxes().addChild(std::move(series));
	m_data->addCompositeListener(*this);

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
	m_ctlr->frequency.removeListener(m_freqLsnr);
	m_ctlr->phase.removeListener(m_phaseLsnr);

	if (m_data)
		m_data->removeCompositeListener(*this);

	m_plot->getPlotArea().removeKeyListener(*this);
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

void node::FloatKeyEditor::onKeyDown(gui::key_t key)
{
	if (key == 'X' || key == gui::KEY_DEL) {
		//Erase selected
		if (!m_selection.empty()) {
			if (gui::IInvoker* inv = getInvoker())
				inv->queue(m_data->getEraseOp(m_selection));
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
				if (gui::Keyboard::isDown(gui::KEY_SHIFT)) {
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
					inv->queue(std::move(m_op));
			}

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
		assert(!m_selection.empty());

		m_dragThresholdPassed = true;
		if (m_activeItem != m_selection.end())
			m_op = (*m_activeItem)->getMoveOp(m_selection);
		else
			m_op = (*m_selection.begin())->getMoveOp(m_selection);
	}

	if (m_dragThresholdPassed) {
		if (m_op)
			m_op->update(m_data->fromGlobalSpace(pos) - m_data->fromGlobalSpace(m_clickPoint));
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

void node::FloatKeyEditor::onRemoveChild(gui::IComponent* c, gui::Composite* source)
{
	//If c was selected, it must be removed from selection immediately (it may no longer exist).
	if (m_activeItem != m_selection.end() && *m_activeItem == c) {
		m_selection.erase(m_activeItem);
		m_activeItem = m_selection.end();
		m_activePanel->clearChildren();
	}
	else if (auto it = m_selection.find(static_cast<KeyHandle*>(c)); it != m_selection.end())
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
