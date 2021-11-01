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

node::Modifier::Modifier(std::unique_ptr<nif::NiPSysModifier>&& obj) :
	NodeBase(std::move(obj))
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);
}

node::Modifier::~Modifier()
{
	for (auto&& l : m_lsnrs) {
		assert(l);
		object().name().removeListener(*l);
	}
}

nif::NiPSysModifier& node::Modifier::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysModifier*>(getObjects()[0].get());
}

void node::Modifier::addUnknownController(std::unique_ptr<nif::NiPSysModifierCtlr>&& ctlr)
{
	if (ctlr) {
		m_lsnrs.push_back(std::make_unique<ModifierNameListener>(ctlr->modifierName()));
		object().name().addListener(*m_lsnrs.back());
		NodeBase::addController(std::move(ctlr));
	}
}

void node::Modifier::addTargetField(std::shared_ptr<Device>&& device)
{
	newField<NextModField>(NEXT_MODIFIER, *this, device);
	newField<TargetField>(TARGET, *this, device);
}


void node::Modifier::OrderListener::onInsert(const ISequence<nif::NiPSysModifier>&, size_t pos)
{
	if (pos <= m_order.get()) {
		//we have been pushed back
		size_t order = m_order.get() + 1;
		m_order.set(order);
	}
	//else ignore
}

void node::Modifier::OrderListener::onErase(const ISequence<nif::NiPSysModifier>&, size_t pos)
{
	//if pos == order, we were erased ourselves!
	if (pos < m_order.get()) {
		//we have been pulled forward
		size_t order = m_order.get() - 1;
		m_order.set(order);
	}
	//else ignore
}

void node::Modifier::NameListener::onSet(const unsigned int& i)
{
	m_name.set("Modifier:" + std::to_string(i));
}

void node::Modifier::Device::onConnect(IModifiable& ifc)
{
	m_mod.active().set(true);
	size_t order = ifc.modifiers().insert(-1, m_mod);

	ifc.modifiers().addListener(m_modLsnr);//update order if sequence changes
	m_mod.order().addListener(m_ordLsnr);//update name if order changes

	m_mod.order().set(order);

	for (auto&& ctlr : m_node.getControllers()) {
		assert(ctlr);
		ifc.controllers().insert(-1, *ctlr);
	}

	//Controllers may be attached/detached when we are already connected. This means that
	//*we need to store our target
	//*we need to be informed of/listen to controller additions
	//If we listen to NodeBase add/remove controller:
	//onAdd: ifc.controllers.find(node.getControllers().back()), insert after it
	//onRemove: find controller, erase

	//pass on last
	SequentialDevice::onConnect(ifc);
}

void node::Modifier::Device::onDisconnect(IModifiable& ifc)
{
	//pass on first
	SequentialDevice::onDisconnect(ifc);

	for (auto&& ctlr : m_node.getControllers()) {
		assert(ctlr);
		if (size_t pos = ifc.controllers().find(*ctlr); pos != -1)
			ifc.controllers().erase(pos);
	}

	ifc.modifiers().removeListener(m_modLsnr);
	m_mod.order().removeListener(m_ordLsnr);

	assert(ifc.modifiers().find(m_mod) == m_mod.order().get());
	ifc.modifiers().erase(m_mod.order().get());

	m_mod.order().set(-1);
	m_mod.name().set(std::string());
}


node::Modifier::TargetField::TargetField(const std::string& name, Modifier& node, const std::shared_ptr<Device>& device) :
	Field(name), m_device{ device }
{
	assert(m_device);
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, *m_device));
}

node::Modifier::NextModField::NextModField(const std::string& name, Modifier& node, const std::shared_ptr<Device>& device) :
	Field(name), m_device{ device }
{
	assert(m_device);
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(*m_device, m_rcvr));
}


node::DummyModifier::DummyModifier(std::unique_ptr<nif::NiPSysModifier>&& obj) :
	Modifier(std::move(obj))
{
	setTitle("Unknown modifier");
	setSize({ WIDTH, HEIGHT });
	addTargetField(std::make_shared<Device>(*this));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
}
