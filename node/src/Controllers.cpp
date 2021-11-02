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
#include "widgets.h"

class node::FloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, FloatController& node) :
		Field(name), m_ifc(node), m_rcvr(node.object()), m_sndr(m_ifc)
	{
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
	//set defaults
}

node::FloatController::FloatController(
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr, 
	std::unique_ptr<nif::NiFloatData>&& data) :
	NodeBase(std::move(iplr))
{
	setClosable(true);
	setTitle("Float controller");
	setSize({ WIDTH, HEIGHT });

	if (!data) {
		//We don't necessarily need a data block. Should we always have one regardless?
	}

	newField<TargetField>(TARGET, *this);
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
