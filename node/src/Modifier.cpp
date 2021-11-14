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

node::Modifier::Modifier(ni_ptr<nif::NiPSysModifier>&& obj) :
	m_obj{ std::move(obj) }, 
	m_device(m_obj),
	m_nameUpdater(nif::make_field_ptr(m_obj, &m_obj->name()))
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);

	m_obj->order().addListener(m_nameUpdater);
	m_nameUpdater.onSet(m_obj->order().get());

	m_targetField = newField<NextModField>(NEXT_MODIFIER, *this, m_device);
	m_nextField = newField<TargetField>(TARGET, *this, m_device);
}

node::Modifier::~Modifier()
{
}

nif::NiPSysModifier& node::Modifier::object()
{
	assert(m_obj);
	return *m_obj;
}

void node::Modifier::addUnknownController(ni_ptr<nif::NiPSysModifierCtlr>&& ctlr)
{
	if (ctlr) {
		m_device.addController(ctlr);
		m_unknownCtlrs.push_back(std::move(ctlr));
	}
}


node::Modifier::OrderUpdater::OrderUpdater(ni_ptr<IProperty<unsigned int>>&& order) :
	m_order{ std::move(order) }
{
	assert(m_order);
}


void node::Modifier::OrderUpdater::onInsert(size_t pos)
{
	if (pos <= m_order->get()) {
		//we have been pushed back
		size_t order = m_order->get() + 1;
		m_order->set(order);
	}
	//else ignore
}

void node::Modifier::OrderUpdater::onErase(size_t pos)
{
	//if pos == order, we were erased ourselves!
	if (pos < m_order->get()) {
		//we have been pulled forward
		size_t order = m_order->get() - 1;
		m_order->set(order);
	}
	//else ignore
}

node::Modifier::NameUpdater::NameUpdater(ni_ptr<IProperty<std::string>>&& name) :
	m_name{ std::move(name) }
{
	assert(m_name);
}

void node::Modifier::NameUpdater::onSet(const unsigned int& i)
{
	m_name->set("Modifier:" + std::to_string(i));
}

node::Modifier::StringMatcher::StringMatcher(ni_ptr<IProperty<std::string>>&& target) :
	m_target{ std::move(target) }
{
	assert(m_target);
}

node::Modifier::Device::Device(const ni_ptr<nif::NiPSysModifier>& obj) :
	m_mod{ obj },
	m_orderUpdater(nif::make_field_ptr(m_mod, &m_mod->order()))
{
	//a little late for this, but will matter if we improve make_field_ptr to use pointer to member
	assert(m_mod);

	m_mod->active().set(true);//should be controllable somehow
}

void node::Modifier::Device::onConnect(IModifiable& ifc)
{
	assert(!m_ifc);
	m_ifc = &ifc;

	size_t order = ifc.modifiers().insert(-1, *m_mod);

	ifc.modifiers().addListener(m_orderUpdater);//update order if sequence changes
	m_mod->order().set(order);//do we still need this?

	for (auto&& item : m_ctlrs) {
		assert(item.first);
		ifc.controllers().insert(-1, *item.first);
	}

	for (auto&& req : m_reqs) {
		if (req.second > 0)
			ifc.requirements().add(req.first);
	}

	//pass on last
	SequentialDevice::onConnect(ifc);
}

void node::Modifier::Device::onDisconnect(IModifiable& ifc)
{
	assert(m_ifc == &ifc);

	//pass on first
	SequentialDevice::onDisconnect(ifc);

	for (auto&& req : m_reqs) {
		if (req.second > 0)
			ifc.requirements().remove(req.first);
	}

	for (auto&& item : m_ctlrs) {
		assert(item.first);
		if (size_t pos = ifc.controllers().find(*item.first); pos != -1)
			ifc.controllers().erase(pos);
	}

	ifc.modifiers().removeListener(m_orderUpdater);

	//It would be safer to search for our position, 
	//but unless we messed something up we shouldn't need to
	assert(ifc.modifiers().find(*m_mod) == m_mod->order().get());
	ifc.modifiers().erase(m_mod->order().get());

	m_mod->order().set(-1);

	m_ifc = nullptr;
}

void node::Modifier::Device::addController(const ni_ptr<nif::NiPSysModifierCtlr>& ctlr)
{
	//Add the controller to our list and register for it to receive name changes from NiPSysModifier.
	//If we are connected, add the controller to our target.
	if (ctlr) {
		auto up = [&ctlr](const ControllerPair& item) { return item.first == ctlr; };
		if (std::find_if(m_ctlrs.begin(), m_ctlrs.end(), up) == m_ctlrs.end()) {
			m_ctlrs.push_back(
				{ ctlr, std::make_unique<StringMatcher>(nif::make_field_ptr(ctlr, &ctlr->modifierName())) });
			m_mod->name().addListener(*m_ctlrs.back().second);
			m_ctlrs.back().second->onSet(m_mod->name().get());
			if (m_ifc)
				m_ifc->controllers().insert(-1, *ctlr);
		}
	}
}

void node::Modifier::Device::removeController(nif::NiPSysModifierCtlr* ctlr)
{
	//addController in reverse order, essentially
	if (ctlr) {
		auto up = [ctlr](const ControllerPair& item) { return item.first.get() == ctlr; };
		if (auto it = std::find_if(m_ctlrs.begin(), m_ctlrs.end(), up); it != m_ctlrs.end()) {
			if (m_ifc) {
				if (size_t pos = m_ifc->controllers().find(*ctlr))
					m_ifc->controllers().erase(pos);
			}
			m_mod->name().removeListener(*it->second);
			m_ctlrs.erase(it);
		}
	}
}

void node::Modifier::Device::addRequirement(Requirement req)
{
	int count = ++m_reqs[req];//inserts int() at req if the entry does not yet exist
	if (count == 1 && m_ifc)
		m_ifc->requirements().add(req);
}

void node::Modifier::Device::removeRequirement(Requirement req)
{
	int count = --m_reqs[req];
	assert(count >= 0);//or we messed something up!
	if (count == 0 && m_ifc)
		m_ifc->requirements().remove(req);
}


node::Modifier::TargetField::TargetField(const std::string& name, Modifier& node, Device& rcvr) :
	Field(name), m_rcvr{ rcvr }
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
}

node::Modifier::NextModField::NextModField(const std::string& name, Modifier& node, Device& sndr) :
	Field(name), m_sndr{ sndr }
{
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
}


node::DummyModifier::DummyModifier(ni_ptr<nif::NiPSysModifier>&& obj) :
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
