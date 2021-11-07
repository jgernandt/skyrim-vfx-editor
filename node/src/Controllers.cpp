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
#include "Controllers.h"
#include "DeviceImpl.h"
#include "widget_types.h"
#include "CompositionActions.h"

constexpr gui::ColRGBA TitleCol_Anim = { 0.8f, 0.8f, 0.8f, 1.0f };
constexpr gui::ColRGBA TitleCol_AnimActive = { 0.8f, 0.8f, 0.8f, 1.0f };

constexpr unsigned short DEFAULT_FLAGS = 72;
constexpr float DEFAULT_FREQUENCY = 1.0f;
constexpr float DEFAULT_PHASE = 0.0f;
constexpr float DEFAULT_STARTTIME = 0.0f;
constexpr float DEFAULT_STOPTIME = 1.0f;

class FloatKeyEditor final : public gui::Popup
{
public:
	FloatKeyEditor(nif::NiFloatData& data, IProperty<float>& tStart, IProperty<float>& tStop) : 
		m_data{ data }, m_tStart{ tStart }, m_tStop{ tStop }
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

		auto plot = newChild<gui::Plot>();
		plot->getPlotArea().setMouseHandler(std::make_unique<PlotAreaInput>(plot->getPlotArea()));
		//plot->setSize({ 600.0f, 400.0f });
		//plot->setXLimits({ m_tStart.get(), m_tStop.get() });
		//ylim should be based on min/max data
	}

	virtual void frame(gui::FrameDrawer& fd) override
	{
		Popup::frame(fd);
	}

	virtual void onClose() override
	{
		asyncInvoke<gui::RemoveChild>(this, getParent(), false);
	}

private:
	constexpr static float SCALE_BASE = 1.1f;
	constexpr static float SCALE_SENSITIVITY = 0.1f;

	class PlotAreaInput final : public gui::MouseHandler
	{
	public:
		PlotAreaInput(gui::PlotArea& area) : m_area{ area } {}

		virtual void onMouseDown(gui::Mouse::Button button) override 
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
		virtual void onMouseUp(gui::Mouse::Button button) override 
		{
			if (button == gui::Mouse::Button::MIDDLE) {
				assert(gui::Mouse::getCapture() == &m_area && (m_panning || m_zooming));
				gui::Mouse::setCapture(nullptr);
				m_panning = false;
				m_zooming = false;
			}
		}
		virtual void onMouseMove(const gui::Floats<2>& delta) override 
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
		virtual void onMouseWheel(float delta) 
		{
			float scaleFactor = std::pow(SCALE_BASE, delta);
			applyScale({ scaleFactor, scaleFactor }, m_area.fromGlobalSpace(gui::Mouse::getPosition()));
		}

	private:
		void applyScale(const gui::Floats<2>& factor, const gui::Floats<2>& pivot)
		{
			gui::Floats<2> T = m_area.getAxes().getTranslation();
			m_area.getAxes().setTranslation(pivot - (pivot - T) * factor);
			m_area.getAxes().scale(factor);

			updateAxisUnits();
		}
		void updateAxisUnits()
		{
			//major unit should be 1, 2 or 5 raised to some power of 10
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

	private:
		gui::PlotArea& m_area;
		gui::Floats<2> m_zoomPivot;
		bool m_panning{ false };
		bool m_zooming{ false };
	};

	nif::NiFloatData& m_data;
	IProperty<float>& m_tStart;
	IProperty<float>& m_tStop;
};

class node::FloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, FloatController& node) :
		Field(name), m_ifc(node), m_rcvr(node.object()), m_sndr(m_ifc)
	{
		//This is technically an upwards connector, but feels somehow better to have it downwards.
		//Go for consistency or intuitivity?
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}
private:
	class Controller final : public IController<float>
	{
	public:
		Controller(FloatController& node) : m_node{ node } {}

		virtual IProperty<unsigned short>& flags() override { return m_node.flags(); }
		virtual IProperty<float>& frequency() override { return m_node.frequency(); }
		virtual IProperty<float>& phase() override { return m_node.phase(); }
		virtual IProperty<float>& startTime() override { return m_node.startTime(); }
		virtual IProperty<float>& stopTime() override { return m_node.stopTime(); }

	private:
		FloatController& m_node;
	};
	Controller m_ifc;
	AssignableReceiver<nif::NiInterpolator> m_rcvr;
	Sender<IController<float>> m_sndr;
};

node::FloatController::FloatController() : 
	FloatController(std::make_unique<nif::NiFloatInterpolator>(), std::unique_ptr<nif::NiFloatData>())
{
	flags().set(DEFAULT_FLAGS);
	frequency().set(DEFAULT_FREQUENCY);
	phase().set(DEFAULT_PHASE);
	startTime().set(DEFAULT_STARTTIME);
	stopTime().set(DEFAULT_STOPTIME);
}

node::FloatController::FloatController(
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr, 
	std::unique_ptr<nif::NiFloatData>&& data) :
	NodeBase(std::move(iplr)), m_data{ std::move(data) }
{
	//Remember to have Constructor set our properties before connecting us!

	setClosable(true);
	setTitle("Float controller");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	if (!m_data) {
		//We don't necessarily need a data block. Should we always have one regardless?
	}

	newField<TargetField>(TARGET, *this);

	using selector_type = gui::Selector<unsigned short, IProperty<unsigned short>>;
	newChild<selector_type>(m_flags.cycleType(), std::string(),
		selector_type::ItemList{ { 0, "Repeat" }, { 1, "Reverse" }, { 2, "Clamp" } });

	auto fr = newChild<DragFloat>(m_frequency, "Frequency");
	fr->setSensitivity(0.01f);
	fr->setLowerLimit(0.0f);
	fr->setAlwaysClamp(true);
	fr->setNumberFormat("%.2f");

	auto ph = newChild<DragFloat>(m_phase, "Phase");
	ph->setSensitivity(0.01f);
	ph->setNumberFormat("%.2f");

	newChild<gui::Button>("Keys", std::bind(&FloatController::openKeyEditor, this));

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ WIDTH, 38.0f });
}

node::FloatController::~FloatController()
{
	disconnect();
	//If we clear, NodeBase will try to disconnect destroyed connectors.
	//If we don't, children that reference our local properties will survive us.
	//They won't touch them during destruction, so it's fine. 
	//Regardless, this is a vulnerability that should be fixed.
	//clearChildren();
}

nif::NiFloatInterpolator& node::FloatController::object()
{
	return static_cast<nif::NiFloatInterpolator&>(NodeBase::object());
}

void node::FloatController::openKeyEditor()
{
	if (!m_data) {
		m_data = std::make_unique<nif::NiFloatData>();
		//defaults?
	}
	auto c = std::make_unique<FloatKeyEditor>(*m_data, startTime(), stopTime());
	c->open();
	asyncInvoke<gui::AddChild>(std::move(c), this, false);
}
