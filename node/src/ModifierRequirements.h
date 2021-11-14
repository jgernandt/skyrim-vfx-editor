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

#pragma once
#include "Modifier.h"

namespace node
{
	class ModifierRequirement
	{
	public:
		virtual ~ModifierRequirement() = default;

		virtual void activate() = 0;
		virtual void deactivate() = 0;

		bool active() const { return m_count > 0; }

		void incr()
		{
			if (m_count++ == 0)
				activate();
		}
		void decr()
		{
			assert(m_count > 0);
			if (--m_count == 0)
				deactivate();
		}

	private:
		int m_count{ 0 };
	};

	class ColourRequirement final : public ModifierRequirement
	{
	public:
		ColourRequirement(ni_ptr<IProperty<bool>>&& hasCols) : m_hasCols{ hasCols } {}

	protected:
		virtual void activate() override { m_hasCols->set(true); }
		virtual void deactivate() override { m_hasCols->set(false); }

	private:
		ni_ptr<IProperty<bool>> m_hasCols;
	};


	class LifetimeRequirement final : public ModifierRequirement
	{
	public:
		LifetimeRequirement(ni_ptr<nif::NiPSysAgeDeathModifier>&& mod, ReservableSequence<nif::NiPSysModifier>& mods);
		~LifetimeRequirement();

	protected:
		virtual void activate() override;
		virtual void deactivate() override;

	public:
		nif::NiPSysAgeDeathModifier& modifier() { return *m_mod; }

	private:
		ni_ptr<nif::NiPSysAgeDeathModifier> m_mod;
		ReservableSequence<nif::NiPSysModifier>& m_mods;
		Modifier::OrderUpdater m_orderUpdater;
		Modifier::NameUpdater m_nameUpdater;
	};


	class MovementRequirement final : public ModifierRequirement
	{
	public:
		MovementRequirement(ni_ptr<nif::NiPSysPositionModifier>&& mod, ReservableSequence<nif::NiPSysModifier>& mods);
		~MovementRequirement();

	protected:
		virtual void activate() override;
		virtual void deactivate() override;

	public:
		nif::NiPSysPositionModifier& modifier() { return *m_mod; }

	private:
		ni_ptr<nif::NiPSysPositionModifier> m_mod;
		ReservableSequence<nif::NiPSysModifier>& m_mods;
		Modifier::OrderUpdater m_orderUpdater;
		Modifier::NameUpdater m_nameUpdater;
	};


	class RotationsRequirement final : public ModifierRequirement
	{
	public:
		RotationsRequirement(ni_ptr<IProperty<bool>>&& angles, ni_ptr<IProperty<bool>>&& speeds) :
			m_angles{ angles }, m_speeds{ speeds } {}

	protected:
		virtual void activate() override
		{
			m_angles->set(true);
			m_speeds->set(true);
		}
		virtual void deactivate() override
		{
			m_angles->set(false);
			m_speeds->set(false);
		}

	private:
		ni_ptr<IProperty<bool>> m_angles;
		ni_ptr<IProperty<bool>> m_speeds;
	};


	class UpdateRequirement final : public ModifierRequirement
	{
	public:
		UpdateRequirement(
			ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum,
			ni_ptr<nif::NiPSysUpdateCtlr>&& ctlr,
			ReservableSequence<nif::NiPSysModifier>& mods,
			ReservableSequence<nif::NiTimeController>& ctlrs);
		~UpdateRequirement();

	protected:
		virtual void activate() override;
		virtual void deactivate() override;

	public:
		nif::NiPSysBoundUpdateModifier& modifier() { return *m_mod; }
		nif::NiPSysUpdateCtlr& controller() { return *m_ctlr; }

	private:
		ni_ptr<nif::NiPSysBoundUpdateModifier> m_mod;
		ni_ptr<nif::NiPSysUpdateCtlr> m_ctlr;
		ReservableSequence<nif::NiPSysModifier>& m_mods;
		ReservableSequence<nif::NiTimeController>& m_ctlrs;
		Modifier::OrderUpdater m_orderUpdater;
		Modifier::NameUpdater m_nameUpdater;
	};
}
