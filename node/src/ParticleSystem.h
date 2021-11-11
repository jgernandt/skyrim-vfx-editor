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
#include "NiParticleSystem.h"
#include "NiPSysModifier.h"

namespace node
{
	class ParticleSystem final : public AVObject
	{
	public:
		ParticleSystem(nif::File& file);
		ParticleSystem(nif::File& file,
			std::shared_ptr<nif::NiParticleSystem>&& obj,
			std::shared_ptr<nif::NiPSysData>&& data,
			std::shared_ptr<nif::NiAlphaProperty>&& alpha,
			std::shared_ptr<nif::NiPSysUpdateCtlr>&& ctlr,
			std::shared_ptr<nif::NiPSysAgeDeathModifier>&& adm,
			std::shared_ptr<nif::NiPSysPositionModifier>&& pm,
			std::shared_ptr<nif::NiPSysBoundUpdateModifier>&& bum);

		~ParticleSystem();

		virtual nif::NiParticleSystem& object() override;
		nif::NiPSysData& data();
		nif::NiAlphaProperty& alphaProperty();
		nif::NiPSysUpdateCtlr& updateCtlr();
		nif::NiPSysAgeDeathModifier& ageDeathMod();
		nif::NiPSysPositionModifier& positionMod();
		nif::NiPSysBoundUpdateModifier& boundUpdateMod();

		IProperty<nif::SubtextureCount>& subtexCount() { return m_subtexCount; }

	public:
		constexpr static const char* WORLD_SPACE = "World space";
		constexpr static const char* MAX_COUNT = "Particle limit";
		constexpr static const char* SHADER = "Shader";
		constexpr static const char* MODIFIERS = "Modifiers";

		constexpr static float WIDTH = 160.0f;
		constexpr static float HEIGHT = 285.0f;

	private:
		class MaxCountField;
		class WorldSpaceField;
		class ShaderField;
		class ModifiersField;
		class ModifiersManager;

		LocalProperty<nif::SubtextureCount> m_subtexCount;
		std::unique_ptr<SetterListener<nif::SubtextureCount, std::vector<nif::SubtextureOffset>>> m_subtexLsnr;
	};
}