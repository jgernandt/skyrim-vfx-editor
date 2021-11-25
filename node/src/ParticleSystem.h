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
#include "AVObject.h"

namespace node
{
	using namespace nif;

	class ParticleSystem final : public AVObject
	{
	public:
		ParticleSystem(
			const ni_ptr<NiParticleSystem>& psys,
			const ni_ptr<NiPSysData>& data,
			const ni_ptr<NiAlphaProperty>& alpha,
			const ni_ptr<NiPSysAgeDeathModifier>& adm,
			const ni_ptr<NiPSysBoundUpdateModifier>& bum,
			const ni_ptr<NiPSysPositionModifier>& pm,
			const ni_ptr<NiPSysUpdateCtlr>& ctlr);

		~ParticleSystem();

		Property<SubtextureCount>& subtexCount() { return *m_subtexCount; }

	public:
		constexpr static const char* WORLD_SPACE = "World space";
		constexpr static const char* MAX_COUNT = "Particle limit";
		constexpr static const char* SHADER = "Shader";
		constexpr static const char* MODIFIERS = "Modifiers";

		constexpr static float WIDTH = 160.0f;
		constexpr static float HEIGHT = 285.0f;

	private:
		class MaxCountField;
		class ShaderField;
		class ModifiersField;

		PropertySyncer<SubtextureCount, std::vector<SubtextureOffset>> m_subtexLsnr;
		ni_ptr<Property<SubtextureCount>> m_subtexCount;

		std::unique_ptr<Field> m_shaderField;
		std::unique_ptr<Field> m_maxCountField;
		std::unique_ptr<Field> m_modifiersField;
	};
}