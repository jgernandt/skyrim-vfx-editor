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
#include "NiParticleSystem.h"
#include "NiObject_internal.h"

namespace nif
{
	//NiParticleSystem
	template<> struct type_map<Niflib::NiParticleSystem> { using type = NiParticleSystem; };
	template<> struct type_map<NiParticleSystem> { using type = Niflib::NiParticleSystem; };

	template<> struct ReadSyncer<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ReadSyncer>
	{
		bool operator() (NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file);
	};
	template<> struct WriteSyncer<NiParticleSystem> : VerticalTraverser<NiParticleSystem, WriteSyncer>
	{
		bool operator() (const NiParticleSystem& object, Niflib::NiParticleSystem* native, const File& file);
	};

	//NiPSysData
	template<> struct type_map<Niflib::NiPSysData> { using type = NiPSysData; };
	template<> struct type_map<NiPSysData> { using type = Niflib::NiPSysData; };
	template<> struct ReadSyncer<NiPSysData> : VerticalTraverser<NiPSysData, ReadSyncer>
	{
		bool operator() (NiPSysData& object, const Niflib::NiPSysData* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysData> : VerticalTraverser<NiPSysData, WriteSyncer>
	{
		bool operator() (const NiPSysData& object, Niflib::NiPSysData* native, const File& file);
	};
}
