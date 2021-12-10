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
#include "NodeBase.h"

/*
//This might be universally useful.
//Update: Might *have* been useful. I don't know if it translates to the new nif system.

template<typename T, typename FieldType, int Offset, int Width>
class BitsetProperty final : public IProperty<T>
{
	static_assert(std::is_integral<T>::value && std::is_integral<FieldType>::value);
	static_assert(Offset >= 0 && Width > 0);
	static_assert(Offset < 8 * sizeof(FieldType) && Width <= 8 * sizeof(FieldType) - Offset);
public:
	BitsetProperty(IProperty<FieldType>& bitfield) : m_bitfield{ bitfield } {}

	virtual T get() const override
	{
		return static_cast<T>((m_bitfield.get() & MASK) >> Offset);
	}
	virtual void set(const T& t) override
	{
		m_bitfield.set(((static_cast<FieldType>(t) << Offset) & MASK) | (m_bitfield.get() & ~MASK));
	}

private:
	constexpr static FieldType MASK = ~(~FieldType(0) << Width) << Offset;

	IProperty<FieldType>& m_bitfield;
};
*/

namespace node
{
	using namespace nif;

	class ControllerBase : public NodeBase
	{
	public:
		ControllerBase(const ni_ptr<NiInterpolator>& iplr);
		virtual ~ControllerBase() = default;

		FlagSet<ControllerFlags>& flags() { return m_ctlr->flags; }
		const FlagSet<ControllerFlags>& flags() const { return m_ctlr->flags; }
		Property<float>& frequency() { return m_ctlr->frequency; }
		Property<float>& phase() { return m_ctlr->phase; }
		Property<float>& startTime() { return m_ctlr->startTime; }
		Property<float>& stopTime() { return m_ctlr->stopTime; }

		constexpr static const char* TARGET = "Target";

	protected:
		const ni_ptr<NiTimeController> m_ctlr;//dummy controller

		//bleh
		class TargetField;
		std::unique_ptr<TargetField> m_target;
	};

	class FloatController final : public ControllerBase
	{
	public:
		FloatController(const ni_ptr<NiFloatInterpolator>& iplr);
		~FloatController();

	private:
		void openKeyEditor();

	public:
		constexpr static float WIDTH = 120.0f;
		constexpr static float HEIGHT = 96.0f;

	private:
		const ni_ptr<NiFloatInterpolator> m_iplr;
	};

	class NLFloatController final : public ControllerBase
	{
	public:
		using default_object = NiBlendFloatInterpolator;

		NLFloatController(const ni_ptr<NiBlendFloatInterpolator>& obj);
		~NLFloatController();

		constexpr static float WIDTH = 120.0f;
		constexpr static float HEIGHT = 96.0f;

	private:
		const ni_ptr<NiBlendFloatInterpolator> m_iplr;
	};
}
