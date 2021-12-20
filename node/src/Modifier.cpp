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
#include "Modifier.h"
#include "style.h"
#include "widget_types.h"

using namespace nif;

node::Modifier::Modifier(const ni_ptr<NiPSysModifier>& obj) :
	m_modifiableDevice(obj),
	m_targetNodeDevice{ m_targetNode },
	m_nameUpdater(make_ni_ptr(obj, &NiPSysModifier::name))
{
	assert(obj);

	setClosable(true);
	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);

	obj->order.addListener(m_nameUpdater);
	m_nameUpdater.onSet(obj->order.get());

	m_targetField = newField<NextModField>(NEXT_MODIFIER, *this);
	m_nextField = newField<TargetField>(TARGET, *this);
}

node::Modifier::~Modifier()
{
}

void node::Modifier::addController(const ni_ptr<nif::NiPSysModifierCtlr>& ctlr)
{
	m_modifiableDevice.addController(ctlr);
}

void node::Modifier::removeController(NiPSysModifierCtlr* ctlr)
{
	m_modifiableDevice.removeController(ctlr);
}

std::vector<NiPSysModifierCtlr*> node::Modifier::getControllers() const
{
	return m_modifiableDevice.getControllers();
}


node::Modifier::NameUpdater::NameUpdater(ni_ptr<Property<std::string>>&& name) :
	m_name{ std::move(name) }
{
	assert(m_name);
}

void node::Modifier::NameUpdater::onSet(const unsigned int& i)
{
	m_name->set("Modifier:" + std::to_string(i));
}

node::Modifier::Device::Device(const ni_ptr<NiPSysModifier>& obj) :
	m_mod{ obj }
{
	assert(m_mod);
	m_mod->active.set(true);//should be controllable somehow
}

void node::Modifier::Device::onConnect(IModifiable& ifc)
{
	assert(!m_ifc);
	m_ifc = &ifc;

	ifc.addModifier(m_mod);

	for (auto&& item : m_ctlrs)
		ifc.addController(item.first);

	for (auto&& req : m_reqs)
		if (req.second > 0)
			ifc.addRequirement(req.first);

	//pass on last
	SequentialReceiver::onConnect(ifc);
}

void node::Modifier::Device::onDisconnect(IModifiable& ifc)
{
	assert(m_ifc == &ifc);

	//pass on first
	SequentialReceiver::onDisconnect(ifc);

	for (auto&& req : m_reqs)
		if (req.second > 0)
			ifc.removeRequirement(req.first);

	for (auto&& item : m_ctlrs)
		ifc.removeController(item.first.get());

	ifc.removeModifier(m_mod.get());

	m_ifc = nullptr;
}

void node::Modifier::Device::addController(const ni_ptr<NiPSysModifierCtlr>& ctlr)
{
	//Add the controller to our list and register for it to receive name changes from NiPSysModifier.
	//If we are connected, add the controller to our target.
	if (ctlr) {
		auto up = [&ctlr](const ControllerPair& item) { return item.first == ctlr; };
		if (std::find_if(m_ctlrs.begin(), m_ctlrs.end(), up) == m_ctlrs.end()) {
			m_ctlrs.push_back(
				{ ctlr, std::make_unique<PropertySyncer<std::string>>(make_ni_ptr(ctlr, &NiPSysModifierCtlr::modifierName)) });

			m_mod->name.addListener(*m_ctlrs.back().second);
			m_ctlrs.back().second->onSet(m_mod->name.get());
			if (m_ifc)
				m_ifc->addController(ctlr);
		}
	}
}

void node::Modifier::Device::removeController(nif::NiPSysModifierCtlr* ctlr)
{
	//addController in reverse order, essentially
	if (ctlr) {
		auto up = [ctlr](const ControllerPair& item) { return item.first.get() == ctlr; };
		if (auto it = std::find_if(m_ctlrs.begin(), m_ctlrs.end(), up); it != m_ctlrs.end()) {
			if (m_ifc)
				m_ifc->removeController(it->first.get());

			m_mod->name.removeListener(*it->second);
			m_ctlrs.erase(it);
		}
	}
}

std::vector<NiPSysModifierCtlr*> node::Modifier::Device::getControllers() const
{
	std::vector<NiPSysModifierCtlr*> out;
	out.reserve(m_ctlrs.size());
	for (auto&& pair : m_ctlrs)
		out.push_back(pair.first.get());
	return out;
}

void node::Modifier::Device::addRequirement(ModRequirement req)
{
	int count = ++m_reqs[req];//inserts int() at req if the entry does not yet exist
	if (m_ifc && count == 1)
		m_ifc->addRequirement(req);
}

void node::Modifier::Device::removeRequirement(ModRequirement req)
{
	int count = --m_reqs[req];
	assert(count >= 0);
	if (m_ifc)
		m_ifc->removeRequirement(req);
}


node::Modifier::TargetField::TargetField(const std::string& name, Modifier& node) :
	Field(name)
{
	connector = node.addConnector(
		name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(node.m_targetNodeDevice, node.m_modifiableDevice));
}

node::Modifier::NextModField::NextModField(const std::string& name, Modifier& node) :
	Field(name)
{
	connector = node.addConnector(
		name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(node.m_modifiableDevice, node.m_targetNodeDevice));
}


node::DummyModifier::DummyModifier(const ni_ptr<nif::NiPSysModifier>& obj) :
	Modifier(std::move(obj))
{
	setTitle("Unknown modifier");
	setSize({ WIDTH, HEIGHT });

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
}

node::DummyModifier::~DummyModifier()
{
	disconnect();
}
