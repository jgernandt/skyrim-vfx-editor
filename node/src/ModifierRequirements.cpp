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
#include "ModifierRequirements.h"

//A lot of repetition here

node::LifetimeRequirement::LifetimeRequirement(ni_ptr<nif::NiPSysAgeDeathModifier>&& mod, ReservableSequence<nif::NiPSysModifier>& mods) :
	m_mod{ std::move(mod) }, 
	m_mods{ mods }, 
	m_orderUpdater(nif::make_field_ptr(m_mod, &m_mod->order())), 
	m_nameUpdater(nif::make_field_ptr(m_mod, &m_mod->name()))
{
	//a little late for this, but will matter if we improve make_field_ptr to use pointer to member
	assert(m_mod);

	m_mod->order().addListener(m_nameUpdater);
	m_nameUpdater.onSet(m_mod->order().get());

	m_mod->active().set(true);
}

node::LifetimeRequirement::~LifetimeRequirement()
{
	if (active())
		deactivate();
}

void node::LifetimeRequirement::activate()
{
	size_t order = m_mods.reserve(0, *m_mod);
	m_mods.addListener(m_orderUpdater);
	m_mod->order().set(order);

	//m_mod->name().set("Modifier:" + std::to_string(order));
}

void node::LifetimeRequirement::deactivate()
{
	m_mods.removeListener(m_orderUpdater);
	size_t order = m_mod->order().get();
	assert(m_mods.find(*m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod->order().set(-1);
}


node::MovementRequirement::MovementRequirement(
	ni_ptr<nif::NiPSysPositionModifier>&& mod, 
	ReservableSequence<nif::NiPSysModifier>& mods) :
	m_mod{ std::move(mod) }, 
	m_mods{ mods },
	m_orderUpdater(nif::make_field_ptr(m_mod, &m_mod->order())),
	m_nameUpdater(nif::make_field_ptr(m_mod, &m_mod->name()))
{
	//a little late for this, but will matter if we improve make_field_ptr to use pointer to member
	assert(m_mod);

	m_mod->order().addListener(m_nameUpdater);
	m_nameUpdater.onSet(m_mod->order().get());

	m_mod->active().set(true);
}

node::MovementRequirement::~MovementRequirement()
{
	if (active())
		deactivate();
}

void node::MovementRequirement::activate()
{
	size_t order = m_mods.reserve(-1, *m_mod);
	m_mods.addListener(m_orderUpdater);
	m_mod->order().set(order);
}

void node::MovementRequirement::deactivate()
{
	m_mods.removeListener(m_orderUpdater);
	size_t order = m_mod->order().get();
	assert(m_mods.find(*m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod->order().set(-1);
}


node::UpdateRequirement::UpdateRequirement(
	ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum,
	ni_ptr<nif::NiPSysUpdateCtlr>&& ctlr,
	ReservableSequence<nif::NiPSysModifier>& mods,
	ReservableSequence<nif::NiTimeController>& ctlrs) :
	m_mod{ std::move(bum) }, 
	m_ctlr{ std::move(ctlr) },
	m_mods{ mods }, 
	m_ctlrs{ ctlrs },
	m_orderUpdater(nif::make_field_ptr(m_mod, &m_mod->order())),
	m_nameUpdater(nif::make_field_ptr(m_mod, &m_mod->name()))
{
	//a little late for this, but will matter if we improve make_field_ptr to use pointer to member
	assert(m_mod);

	m_mod->order().addListener(m_nameUpdater);
	m_nameUpdater.onSet(m_mod->order().get());

	m_mod->active().set(true);
}

node::UpdateRequirement::~UpdateRequirement()
{
	if (active())
		deactivate();
}

void node::UpdateRequirement::activate()
{
	size_t order = m_mods.reserve(-1, *m_mod);
	m_mods.addListener(m_orderUpdater);
	m_mod->order().set(order);

	m_ctlrs.reserve(-1, *m_ctlr);
	//we don't need to listen to the ctlrs
}

void node::UpdateRequirement::deactivate()
{
	m_mods.removeListener(m_orderUpdater);
	size_t order = m_mod->order().get();
	assert(m_mods.find(*m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod->order().set(-1);

	//name can be left as is

	//if we were listening to ctlrs, remove it here
	m_ctlrs.erase(m_ctlrs.find(*m_ctlr));
}
