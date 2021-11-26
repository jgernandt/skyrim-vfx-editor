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
#include "NiObject.h"

namespace nif
{
	struct NiParticleSystem : NiTraversable<NiParticleSystem, NiAVObject>
	{
		Ref<NiPSysData> data;
		Sequence<NiPSysModifier> modifiers;
		Ref<BSShaderProperty> shaderProperty;
		Ref<NiAlphaProperty> alphaProperty;
		Property<bool> worldSpace;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiParticleSystem> : VerticalTraverser<NiParticleSystem, Forwarder>
	{
		bool operator() (NiParticleSystem& object, NiTraverser& traverser);
	};

	struct NiPSysData : NiTraversable<NiPSysData, NiObject>
	{
		Property<unsigned short> maxCount;
		Property<std::vector<SubtextureOffset>> subtexOffsets;
		Property<bool> hasColour;
		Property<bool> hasRotationAngles;
		Property<bool> hasRotationSpeeds;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}
