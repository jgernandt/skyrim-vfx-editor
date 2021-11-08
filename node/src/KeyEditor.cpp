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
	m_plot->getPlotArea().setMouseHandler(std::make_unique<PlotAreaInput>(m_plot->getPlotArea()));
	//plot->setSize({ 600.0f, 400.0f });
	//plot->setXLimits({ m_tStart.get(), m_tStop.get() });
	//ylim should be based on min/max data

	//The initial curve is created by the callback from keyType.
	m_keyType.addListener(*this);
}

node::FloatKeyEditor::~FloatKeyEditor()
{
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
	gui::ComponentPtr curve;
	switch (type) {
	case nif::KeyType::CONSTANT:
		curve = std::make_unique<ConstantInterpolation>();
		break;
	case nif::KeyType::LINEAR:
		curve = std::make_unique<LinearInterpolation>(m_keys);
		break;
	case nif::KeyType::QUADRATIC:
		curve = std::make_unique<QuadraticInterpolation>(m_keys);
		break;
	}
	if (curve) {
		auto tmp = curve.get();//addChild may throw. Don't assign until we know it will survive.
		m_plot->getPlotArea().getAxes().addChild(std::move(curve));
		m_curve = tmp;
	}
}

void node::FloatKeyEditor::PlotAreaInput::onMouseDown(gui::Mouse::Button button)
{
	if (button == gui::Mouse::Button::MIDDLE) {
		assert(gui::Mouse::getCapture() == nullptr);
		gui::Mouse::setCapture(&m_area);
		if (gui::Keyboard::isDown(gui::Keyboard::Key::CTRL)) {
			m_zooming = true;
			//maintain the pivot point
			m_zoomPivot = m_area.fromGlobalSpace(gui::Mouse::getPosition());
		}
		else
			m_panning = true;
	}
}

void node::FloatKeyEditor::PlotAreaInput::onMouseUp(gui::Mouse::Button button)
{
	if (button == gui::Mouse::Button::MIDDLE) {
		assert(gui::Mouse::getCapture() == &m_area && (m_panning || m_zooming));
		gui::Mouse::setCapture(nullptr);
		m_panning = false;
		m_zooming = false;
	}
}

void node::FloatKeyEditor::PlotAreaInput::onMouseMove(const gui::Floats<2>& delta)
{
	if (gui::Mouse::getCapture() == &m_area) {
		if (m_panning) {
			gui::Floats<2> tmp = m_area.fromGlobalSpace(delta) - m_area.fromGlobalSpace({ 0.0f ,0.0f });
			m_area.getAxes().translate(tmp);
		}
		else if (m_zooming) {
			gui::Floats<2> scale = m_area.getScale();
			scale[0] *= std::pow(SCALE_BASE, delta[0] * SCALE_SENSITIVITY);
			scale[1] *= std::pow(SCALE_BASE, -delta[1] * SCALE_SENSITIVITY);
			applyScale(scale, m_zoomPivot);
		}
	}
}

void node::FloatKeyEditor::PlotAreaInput::onMouseWheel(float delta)
{
	float scaleFactor = std::pow(SCALE_BASE, delta);
	applyScale({ scaleFactor, scaleFactor }, m_area.fromGlobalSpace(gui::Mouse::getPosition()));
}

void node::FloatKeyEditor::PlotAreaInput::applyScale(const gui::Floats<2>& factor, const gui::Floats<2>& pivot)
{
	gui::Floats<2> T = m_area.getAxes().getTranslation();
	m_area.getAxes().setTranslation(pivot - (pivot - T) * factor);
	m_area.getAxes().scale(factor);

	updateAxisUnits();
}

void node::FloatKeyEditor::PlotAreaInput::updateAxisUnits()
{
	//major unit should be 1, 2 or 5 to some power of 10
	gui::Floats<2> major;
	gui::Floats<2> estimate = 300.0f / m_area.getAxes().getScale().abs();
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

node::FloatKeyEditor::LinearInterpolation::LinearInterpolation(IListProperty<nif::Key<float>>& keys) :
	m_keys{ keys }
{
	//We need handles to manipulate our keys. Nothing fancy, just something that can be dragged around.
	//They just need an element property from m_keys.
	//We kind of need some way to set time limits on them, though. Just give them the adjacent
	// property and let them keep track of it through a listener?
	// 
	//The interpolation might be most convenient to draw ourselves.

	m_keys.addListener(*this);//this doesn't trigger an immediate callback. Inconsistent...
}

node::FloatKeyEditor::LinearInterpolation::~LinearInterpolation()
{
	m_keys.removeListener(*this);
}

void node::FloatKeyEditor::LinearInterpolation::frame(gui::FrameDrawer& fd)
{
	//Iterate through our data (or directly through the property, 
	// if we add some way to do that - nicer!) and draw a line between each key.
	//Nothing fancy at all.

	//Let the handles do their thing:
	Composite::frame(fd);
}