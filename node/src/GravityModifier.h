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
	using namespace nif;

	class GravityModifier : public Modifier
	{
	protected:
		GravityModifier(const ni_ptr<NiPSysGravityModifier>& obj);

	public:
		virtual ~GravityModifier() = default;

		constexpr static const char* GRAVITY_OBJECT = "Field object";
		constexpr static const char* STRENGTH = "Strength";
		constexpr static const char* DECAY = "Decay";
		constexpr static const char* TURBULENCE = "Turbulence";
		constexpr static const char* TURBULENCE_SCALE = "Turbulence scale";

	private:
		std::unique_ptr<Field> m_objectField;
		std::unique_ptr<Field> m_strengthField;
		std::unique_ptr<Field> m_decayField;
		std::unique_ptr<Field> m_turbField;
		std::unique_ptr<Field> m_turbScaleField;
	};

	class PlanarForceField final : public GravityModifier
	{
	public:
		PlanarForceField(const ni_ptr<NiPSysGravityModifier>& obj);
		~PlanarForceField();

		constexpr static const char* GRAVITY_AXIS = "Direction";
		constexpr static const char* WORLD_ALIGNED = "World aligned";

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 280.0f;

	private:
		std::unique_ptr<Field> m_axisField;
	};

	class SphericalForceField final : public GravityModifier
	{
	public:
		SphericalForceField(const ni_ptr<NiPSysGravityModifier>& obj);
		~SphericalForceField();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 210.0f;

	};
}