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
#include "NiController.h"
#include "node_concepts.h"

namespace node
{
	class FloatController final : public NodeBase
	{
	public:
		FloatController();
		FloatController(std::unique_ptr<nif::NiFloatInterpolator>&& iplr, std::unique_ptr<nif::NiFloatData>&& data);
		~FloatController();

		virtual nif::NiFloatInterpolator& object() override;

		IProperty<unsigned short>& flags() { return m_flags; }
		IProperty<float>& frequency() { return m_frequency; }
		IProperty<float>& phase() { return m_phase; }
		IProperty<float>& startTime() { return m_startTime; }
		IProperty<float>& stopTime() { return m_stopTime; }

	public:
		constexpr static const char* TARGET = "Target";

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 200.0f;

	private:
		LocalProperty<unsigned short> m_flags;
		LocalProperty<float> m_frequency;
		LocalProperty<float> m_phase;
		LocalProperty<float> m_startTime;
		LocalProperty<float> m_stopTime;

		class TargetField;
	};
}