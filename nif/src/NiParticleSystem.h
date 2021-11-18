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
#include "BSEffectShaderProperty.h"
#include "NiAVObject.h"

namespace nif
{
	struct NiPSysModifier;

	struct NiPSysData : NiTraversable<NiPSysData, NiObject>
	{
		Property<unsigned short> maxCount;
		Property<std::vector<SubtextureOffset>> subtexOffsets;
		Property<bool> hasColour;
		Property<bool> hasRotationAngles;
		Property<bool> hasRotationSpeeds;
	};
	template<> struct type_map<Niflib::NiPSysData> { using type = NiPSysData; };
	template<> struct type_map<NiPSysData> { using type = Niflib::NiPSysData; };

	template<> class NiSyncer<NiPSysData> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiPSysData* object, Niflib::NiPSysData* native);
		void syncWrite(const File& file, NiPSysData* object, Niflib::NiPSysData* native);
	};

	struct NiParticleSystem : NiTraversable<NiParticleSystem, NiAVObject>
	{
		Assignable	<NiPSysData>				data;
		Sequence	<NiPSysModifier>			modifiers;
		Assignable	<BSEffectShaderProperty>	shaderProperty;
		Assignable	<NiAlphaProperty>			alphaProperty;
		Property	<bool>						worldSpace;
	};
	template<> struct type_map<Niflib::NiParticleSystem> { using type = NiParticleSystem; };
	template<> struct type_map<NiParticleSystem> { using type = Niflib::NiParticleSystem; };

	template<> class NiSyncer<NiParticleSystem> : public NiSyncer<NiAVObject>
	{
	public:
		void syncRead(File& file, NiParticleSystem* object, Niflib::NiParticleSystem* native);
		void syncWrite(const File& file, NiParticleSystem* object, Niflib::NiParticleSystem* native);
	};
}
