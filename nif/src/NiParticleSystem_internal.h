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
