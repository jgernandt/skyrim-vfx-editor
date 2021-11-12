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
#include "KeyEditor.h"

#include "CompositionActions.h"

constexpr gui::ColRGBA TitleCol_Anim = { 0.8f, 0.8f, 0.8f, 1.0f };
constexpr gui::ColRGBA TitleCol_AnimActive = { 0.8f, 0.8f, 0.8f, 1.0f };

constexpr unsigned short DEFAULT_FLAGS = 72;
constexpr float DEFAULT_FREQUENCY = 1.0f;
constexpr float DEFAULT_PHASE = 0.0f;
constexpr float DEFAULT_STARTTIME = 0.0f;
constexpr float DEFAULT_STOPTIME = 1.0f;


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

node::FloatController::FloatController(nif::File& file) : 
	FloatController(file, file.create<nif::NiFloatInterpolator>(), file.create<nif::NiFloatData>())
{
	flags().set(DEFAULT_FLAGS);
	frequency().set(DEFAULT_FREQUENCY);
	phase().set(DEFAULT_PHASE);
	startTime().set(DEFAULT_STARTTIME);
	stopTime().set(DEFAULT_STOPTIME);
	m_data->keyType().set(nif::KeyType::LINEAR);
	m_data->iplnData().keys().set({ { startTime().get(), 0.0f }, { stopTime().get(), 0.0f } });
}

node::FloatController::FloatController(nif::File& file,
	std::shared_ptr<nif::NiFloatInterpolator>&& iplr,
	std::shared_ptr<nif::NiFloatData>&& data) :
	NodeBase(std::move(iplr)), 
	m_startTime{ std::make_shared<LocalProperty<float>>() },
	m_stopTime{ std::make_shared<LocalProperty<float>>() },
	m_data{ std::move(data) }
{
	//Remember to have Constructor set our properties before connecting us!

	setClosable(true);
	setTitle("Float controller");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	if (!m_data) {
		//We don't necessarily need a data block. Should we always have one regardless?
		m_data = file.create<nif::NiFloatData>();
		m_data->keyType().set(nif::KeyType::LINEAR);
		m_data->iplnData().keys().set({ { startTime().get(), 0.0f }, { stopTime().get(), 0.0f } });
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
	auto c = std::make_unique<FloatKeyEditor>(m_data->keyType_ptr(), m_data->iplnData_ptr(), 
		m_startTime, m_stopTime);
	c->open();
	asyncInvoke<gui::AddChild>(std::move(c), this, false);
}
