//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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

node::UpdateRequirement::UpdateRequirement(
	nif::NiPSysBoundUpdateModifier& bum,
	nif::NiPSysUpdateCtlr& ctlr,
	ReservableSequence<nif::NiPSysModifier>& mods,
	ReservableSequence<nif::NiTimeController>& ctlrs) :
	m_mod{ bum }, m_ctlr{ ctlr }, m_mods{ mods }, m_ctlrs{ ctlrs }, m_modLsnr(m_mod.order()), m_ordLsnr(m_mod.name())
{
}

node::UpdateRequirement::~UpdateRequirement()
{
	if (active())
		deactivate();
}

void node::UpdateRequirement::activate()
{
	m_mod.active().set(true);
	size_t order = m_mods.reserve(-1, m_mod);
	m_mods.addListener(m_modLsnr);
	m_mod.order().addListener(m_ordLsnr);
	m_mod.order().set(order);

	m_ctlrs.reserve(-1, m_ctlr);
	//we don't need to listen to the ctlrs
}

void node::UpdateRequirement::deactivate()
{
	m_mods.removeListener(m_modLsnr);
	m_mod.order().removeListener(m_ordLsnr);
	size_t order = m_mod.order().get();
	assert(m_mods.find(m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod.order().set(-1);

	//name can be left as is

	//if we were listening to ctlrs, remove it here
	m_ctlrs.erase(m_ctlrs.find(m_ctlr));
}

node::MovementRequirement::MovementRequirement(nif::NiPSysPositionModifier& mod, ReservableSequence<nif::NiPSysModifier>& mods) :
	m_mod{ mod }, m_mods{ mods }, m_modLsnr(m_mod.order()), m_ordLsnr(m_mod.name())
{
}

node::MovementRequirement::~MovementRequirement()
{
	if (active())
		deactivate();
}

void node::MovementRequirement::activate()
{
	m_mod.active().set(true);
	size_t order = m_mods.reserve(-1, m_mod);
	m_mods.addListener(m_modLsnr);
	m_mod.order().addListener(m_ordLsnr);
	m_mod.order().set(order);
}

void node::MovementRequirement::deactivate()
{
	m_mods.removeListener(m_modLsnr);
	m_mod.order().removeListener(m_ordLsnr);
	size_t order = m_mod.order().get();
	assert(m_mods.find(m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod.order().set(-1);
}

node::LifetimeRequirement::LifetimeRequirement(nif::NiPSysAgeDeathModifier& mod, ReservableSequence<nif::NiPSysModifier>& mods) :
	m_mod{ mod }, m_mods{ mods }, m_modLsnr(m_mod.order()), m_ordLsnr(m_mod.name())
{
}

node::LifetimeRequirement::~LifetimeRequirement()
{
	if (active())
		deactivate();
}

void node::LifetimeRequirement::activate()
{
	m_mod.active().set(true);
	size_t order = m_mods.reserve(0, m_mod);
	m_mods.addListener(m_modLsnr);
	m_mod.order().addListener(m_ordLsnr);
	m_mod.order().set(order);

	m_mod.name().set("Modifier:" + std::to_string(order));
}

void node::LifetimeRequirement::deactivate()
{
	m_mods.removeListener(m_modLsnr);
	m_mod.order().removeListener(m_ordLsnr);
	size_t order = m_mod.order().get();
	assert(m_mods.find(m_mod) == order);//or we messed something up
	m_mods.erase(order);
	m_mod.order().set(-1);
}