#include "pch.h"
#include "KeyEditor.h"
#include "CompositionActions.h"
#include "widget_types.h"

node::FloatKeyEditor::FloatKeyEditor(nif::NiFloatData& data, IProperty<float>& tStart, IProperty<float>& tStop) :
	m_keys{ data.keys() }, m_keyType{ data.keyType() }
{
	setSize({ 640.0f, 480.0f });

	//We need:
	//plot area
	//input handler for plot area (specific mechanic on generic component inserted above plot area?)
	//handles for each key
	//a curve that interpolates the data
	//widget to select interpolation type
	//controls for start/stop time

	auto item = newChild<gui::Item>();
	item->setSize({ 200.0f, -1.0f });
	item->newChild<gui::Text>("Interpolation");
	using selector_type = gui::Selector<nif::KeyType, IProperty<nif::KeyType>>;
	auto selector = item->newChild<selector_type>(data.keyType(), std::string(),
		selector_type::ItemList{
			{ nif::KeyType::CONSTANT, "Constant" },
			{ nif::KeyType::LINEAR, "Linear" },
			{ nif::KeyType::QUADRATIC, "Quadratic" } });

	m_plot = newChild<gui::Plot>();

	//The initial curve is created by the callback from keyType.
	m_keyType.addListener(*this);

	//determine limits
	gui::Floats<2> xlims = {tStart.get(), tStop.get()};
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

	//Will set the axis units
	m_inputHandler = std::make_unique<PlotAreaInput>(m_plot->getPlotArea());
	m_plot->getPlotArea().setMouseHandler(m_inputHandler.get());
}

node::FloatKeyEditor::~FloatKeyEditor()
{
	//Exiting the program while we are open will make this invalid.
	//The property is destroyed before we are. 
	//We need to refactor NodeBase to solve this.
	m_keyType.removeListener(*this);
}

void node::FloatKeyEditor::onClose()
{
	asyncInvoke<gui::RemoveChild>(this, getParent(), false);
}

void node::FloatKeyEditor::onSet(const nif::KeyType& type)
{
	assert(m_plot);
	//This is where we replace the existing curve widget with a new one.
	//we need to know where the curve is now and remove it.
	if (m_curve) {
		m_plot->getPlotArea().getAxes().removeChild(m_curve);
		m_curve = nullptr;
	}

	//Then we need to make a new of the appropriate type and add that one instead.
	//The new curve will need to know about the keys property.
	std::unique_ptr<Interpolant> curve;
	switch (type) {
	case nif::KeyType::CONSTANT:
		curve = std::make_unique<ConstantInterpolant>();
		break;
	case nif::KeyType::LINEAR:
		curve = std::make_unique<LinearInterpolant>(m_keys);
		break;
	case nif::KeyType::QUADRATIC:
		curve = std::make_unique<QuadraticInterpolant>(m_keys);
		break;
	}
	if (curve) {
		auto tmp = curve.get();//addChild may throw. Don't assign until we know it will survive.
		m_plot->getPlotArea().getAxes().addChild(std::move(curve));
		m_curve = tmp;
	}
}
node::FloatKeyEditor::PlotAreaInput::PlotAreaInput(gui::PlotArea& area) : m_area{ area }
{
	updateAxisUnits();
}

//Locate a component whose center is in the vicinity of the cursor
class ClickSelectionVisitor final : public gui::DescendingVisitor
{
public:
	//expects a point in global space
	ClickSelectionVisitor(const gui::Floats<2>& globalPoint)
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
			(pos[0] >= 0.0f && pos[0] <= size[0] &&	pos[1] >= 0.0f && pos[1] <= size[1])) {
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
class RectangleSelectionVisitor final : public gui::DescendingVisitor
{
public:
	RectangleSelectionVisitor(const gui::Floats<4>& rect) {}

	virtual void visit(gui::Composite& c) override {}
	virtual void visit(gui::Component& c) override {}

	std::vector<gui::IComponent*> found;
};

bool node::FloatKeyEditor::PlotAreaInput::onMouseDown(gui::Mouse::Button button)
{
	if (button == gui::Mouse::Button::LEFT) {
		//locate the clicked object.
		ClickSelectionVisitor v(gui::Mouse::getPosition());
		m_area.accept(v);
		gui::IComponent* object = v.result;

		if (object) {
			if (gui::Keyboard::isDown(gui::Keyboard::Key::SHIFT)) {
				if (m_selection.find(object) != m_selection.end()) {
					//shift+clicked selected object
					//remove from selection
					m_selection.erase(object);
					object->setSelected(false);
				}
				else {
					//shift+clicked non-selected object
					//add to selection
					m_selection.insert(object);
					object->setSelected(true);
				}
			}
			else {
				if (m_selection.find(object) != m_selection.end()) {
					//clicked selected object
					//start dragging selection (if threshold is passed)

					//if we release before reaching the drag threashold, should clear
					//other selected objects
				}
				else {
					//clicked non-selected object
					//set selection and start dragging (if threshold is passed)
					for (auto&& o : m_selection)
						o->setSelected(false);
					m_selection.clear();
					m_selection.insert(object);
					object->setSelected(true);
				}
			}
		}
		//else clear selection? Or not?
		return true;
	}
	else if (button == gui::Mouse::Button::MIDDLE) {
		assert(gui::Mouse::getCapture() == nullptr);
		m_clickPoint = m_area.fromGlobalSpace(gui::Mouse::getPosition());
		gui::Mouse::setCapture(&m_area);
		if (gui::Keyboard::isDown(gui::Keyboard::Key::CTRL)) {
			m_zooming = true;
			m_startS = m_area.getAxes().getScale();
			m_startT = m_area.getAxes().getTranslation();
		}
		else {
			m_panning = true;
			m_startT = m_area.getAxes().getTranslation();
		}

		return true;
	}
	else
		return false;
}

bool node::FloatKeyEditor::PlotAreaInput::onMouseUp(gui::Mouse::Button button)
{
	if (button == gui::Mouse::Button::MIDDLE) {
		assert(gui::Mouse::getCapture() == &m_area && (m_panning || m_zooming));
		gui::Mouse::setCapture(nullptr);
		m_panning = false;
		m_zooming = false;

		return true;
	}
	else
		return false;
}

bool node::FloatKeyEditor::PlotAreaInput::onMouseWheel(float delta)
{
	float scaleFactor = std::pow(SCALE_BASE, delta);

	gui::Floats<2> pivot = m_area.fromGlobalSpace(gui::Mouse::getPosition());
	gui::Floats<2> T = m_area.getAxes().getTranslation();
	m_area.getAxes().setTranslation(pivot - (pivot - T) * scaleFactor);
	m_area.getAxes().scale({ scaleFactor, scaleFactor });

	updateAxisUnits();

	return true;
}

void node::FloatKeyEditor::PlotAreaInput::onMouseMove(const gui::Floats<2>& pos)
{
	if (gui::Mouse::getCapture() == &m_area) {
		gui::Floats<2> delta = m_area.fromGlobalSpace(pos) - m_clickPoint;
		if (m_panning) {
			//gui::Floats<2> tmp = m_area.fromGlobalSpace(delta) - m_area.fromGlobalSpace({ 0.0f ,0.0f });
			m_area.getAxes().setTranslation(m_startT + delta);
		}
		else if (m_zooming) {
			gui::Floats<2> factor;
			factor[0] = std::pow(SCALE_BASE, delta[0] * SCALE_SENSITIVITY);
			factor[1] = std::pow(SCALE_BASE, -delta[1] * SCALE_SENSITIVITY);

			m_area.getAxes().setTranslation(m_clickPoint - (m_clickPoint - m_startT) * factor);
			m_area.getAxes().setScale(m_startS * factor);

			updateAxisUnits();
		}
	}
}

void node::FloatKeyEditor::PlotAreaInput::updateAxisUnits()
{
	//major unit should be 1, 2 or 5 to some power of 10
	gui::Floats<2> major;
	gui::Floats<2> xlims = m_area.getXLimits();
	gui::Floats<2> ylims = m_area.getYLimits();
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

	m_area.getAxes().setMajorUnits(major);
	m_area.getAxes().setMinorUnits(minor);
}

class node::FloatKeyEditor::LinearInterpolant::LinearHandle final : public gui::Component
{
public:
	LinearHandle(LinearKey key) { m_translation = { key.time, key.value }; }

	virtual void frame(gui::FrameDrawer& fd) override 
	{
		//We want to scale with PlotArea, not Axes. Not so easy right now. 
		//We'll just not scale at all.
		fd.circle(fd.toGlobal(m_translation), 3.0f, m_selected ? nif::COL_WHITE : nif::COL_BLACK, true);
	}

	virtual void setFocussed(bool on) override {}
	virtual void setSelected(bool on) override { m_selected = on; }

private:
	bool m_selected{ false };
	//We need somthing to get/set our key. Could be a IProperty, but doesn't really need to.
	//We also want (but don't absolutely need) something to limit our x position. This may
	//be either a key or a start/stop time, so it needs to be abstract.
};

node::FloatKeyEditor::LinearInterpolant::LinearInterpolant(IVectorProperty<nif::Key<float>>& keys) :
	m_keys{ keys }
{
	//We need handles to manipulate our keys. Nothing fancy, just something that can be dragged around.
	//They just need an element property from m_keys.
	//We kind of need some way to set time limits on them, though. Just give them the adjacent
	// property and let them keep track of it through a listener?
	// 
	//The interpolation might be most convenient to draw ourselves.

	m_keys.addListener(*this);//this doesn't trigger an immediate callback. Inconsistent...
	onSet(m_keys.get());
}

node::FloatKeyEditor::LinearInterpolant::~LinearInterpolant()
{
	m_keys.removeListener(*this);
}

void node::FloatKeyEditor::LinearInterpolant::frame(gui::FrameDrawer& fd)
{
	//Draw line
	for (size_t i = 0; i < m_data.size() - 1; i++)
		fd.line({ m_data[i].time, m_data[i].value }, 
			{ m_data[i + 1].time, m_data[i + 1].value }, 
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			3.0f);

	//We may also want to draw the line outside the start-stop time,
	//according to the current cycle rule.

	Composite::frame(fd);

}

gui::Floats<2> node::FloatKeyEditor::LinearInterpolant::getBounds() const
{
	gui::Floats<2> result = { 0.0f, 0.0f };
	for (auto&& key : m_data) {
		if (key.value < result[0])
			result[0] = key.value;
		else if (key.value > result[1])
			result[1] = key.value;
	}
	return result;
}

void node::FloatKeyEditor::LinearInterpolant::onSet(const std::vector<nif::Key<float>>& keys)
{
	//recreate our handles?
	clearChildren();

	m_data.resize(keys.size());
	for (size_t i = 0; i < m_data.size(); i++) {
		m_data[i].time = keys[i].time;
		m_data[i].value = keys[i].value;

		newChild<LinearHandle>(m_data[i]);
	}
}

void node::FloatKeyEditor::LinearInterpolant::onSet(int i, const nif::Key<float>& key)
{
	assert(i >= 0 && static_cast<size_t>(i) < m_data.size());
	m_data[i].time = key.time;
	m_data[i].value = key.value;
}
