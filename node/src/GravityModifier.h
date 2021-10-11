//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "Modifier.h"

namespace node
{
	class GravityModifier : public Modifier
	{
	public:
		virtual ~GravityModifier() = default;
	protected:
		GravityModifier(std::unique_ptr<nif::NiPSysGravityModifier>&& obj);

	public:
		virtual nif::NiPSysGravityModifier& object() override;

	public:
		constexpr static const char* GRAVITY_OBJECT = "Field object";
		//constexpr static const char* FORCE_TYPE = "Symmetry";
		constexpr static const char* STRENGTH = "Strength";
		constexpr static const char* DECAY = "Decay";
		constexpr static const char* TURBULENCE = "Turbulence";
		constexpr static const char* TURBULENCE_SCALE = "Turbulence scale";

	private:
		class GravityObjectField;
		class DecayField;
		class StrengthField;
		class TurbulenceField;
		class TurbulenceScaleField;

	};

	class PlanarForceField final : public GravityModifier
	{
	public:
		PlanarForceField();
		PlanarForceField(std::unique_ptr<nif::NiPSysGravityModifier>&& obj);

		constexpr static const char* GRAVITY_AXIS = "Direction";
		constexpr static const char* WORLD_ALIGNED = "World aligned";

	private:
		class GravityAxisField;
		//class WorldAlignedField;
	};

	class SphericalForceField final : public GravityModifier
	{
	public:
		SphericalForceField();
		SphericalForceField(std::unique_ptr<nif::NiPSysGravityModifier>&& obj);

	};
}