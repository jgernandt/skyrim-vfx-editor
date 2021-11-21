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
		Assignable	<NiPSysData>		data;
		Sequence	<NiPSysModifier>	modifiers;
		Assignable	<BSShaderProperty>	shaderProperty;
		Assignable	<NiAlphaProperty>	alphaProperty;
		Property	<bool>				worldSpace;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiParticleSystem> { using type = NiParticleSystem; };
	template<> struct type_map<NiParticleSystem> { using type = Niflib::NiParticleSystem; };

	template<> struct Forwarder<NiParticleSystem> : VerticalTraverser<NiParticleSystem, Forwarder>
	{
		void operator() (NiParticleSystem& object, NiTraverser& traverser);
	};
	template<> struct ReadSyncer<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ReadSyncer>
	{
		void operator() (NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file);
	};
	template<> struct WriteSyncer<NiParticleSystem> : VerticalTraverser<NiParticleSystem, WriteSyncer>
	{
		void operator() (const NiParticleSystem& object, Niflib::NiParticleSystem* native, const File& file);
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
	template<> struct type_map<Niflib::NiPSysData> { using type = NiPSysData; };
	template<> struct type_map<NiPSysData> { using type = Niflib::NiPSysData; };
	template<> struct ReadSyncer<NiPSysData> : VerticalTraverser<NiPSysData, ReadSyncer>
	{
		void operator() (NiPSysData& object, const Niflib::NiPSysData* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysData> : VerticalTraverser<NiPSysData, WriteSyncer>
	{
		void operator() (const NiPSysData& object, Niflib::NiPSysData* native, const File& file);
	};

}
