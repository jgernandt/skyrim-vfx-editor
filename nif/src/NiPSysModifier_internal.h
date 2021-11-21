#pragma once
#include "NiPSysModifier.h"
#include "NiParticleSystem_internal.h"

namespace nif
{
	//NiPSysModifier
	template<> struct type_map<Niflib::NiPSysModifier> { using type = NiPSysModifier; };
	template<> struct type_map<NiPSysModifier> { using type = Niflib::NiPSysModifier; };

	template<> struct ReadSyncer<NiPSysModifier> : VerticalTraverser<NiPSysModifier, ReadSyncer>
	{
		void operator() (NiPSysModifier& object, const Niflib::NiPSysModifier* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysModifier> : VerticalTraverser<NiPSysModifier, WriteSyncer>
	{
		void operator() (const NiPSysModifier& object, Niflib::NiPSysModifier* native, const File& file);
	};

	//NiPSysAgeDeathModifier
	template<> struct type_map<Niflib::NiPSysAgeDeathModifier> { using type = NiPSysAgeDeathModifier; };
	template<> struct type_map<NiPSysAgeDeathModifier> { using type = Niflib::NiPSysAgeDeathModifier; };

	//NiPSysBoundUpdateModifier
	template<> struct type_map<Niflib::NiPSysBoundUpdateModifier> { using type = NiPSysBoundUpdateModifier; };
	template<> struct type_map<NiPSysBoundUpdateModifier> { using type = Niflib::NiPSysBoundUpdateModifier; };

	//NiPSysGravityModifier
	template<> struct type_map<Niflib::NiPSysGravityModifier> { using type = NiPSysGravityModifier; };
	template<> struct type_map<NiPSysGravityModifier> { using type = Niflib::NiPSysGravityModifier; };

	template<> struct ReadSyncer<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, ReadSyncer>
	{
		void operator() (NiPSysGravityModifier& object, const Niflib::NiPSysGravityModifier* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, WriteSyncer>
	{
		void operator() (const NiPSysGravityModifier& object, Niflib::NiPSysGravityModifier* native, const File& file);
	};

	//NiPSysPositionModifier
	template<> struct type_map<Niflib::NiPSysPositionModifier> { using type = NiPSysPositionModifier; };
	template<> struct type_map<NiPSysPositionModifier> { using type = Niflib::NiPSysPositionModifier; };

	//NiPSysRotationModifier
	template<> struct type_map<Niflib::NiPSysRotationModifier> { using type = NiPSysRotationModifier; };
	template<> struct type_map<NiPSysRotationModifier> { using type = Niflib::NiPSysRotationModifier; };

	template<> struct ReadSyncer<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, ReadSyncer>
	{
		void operator() (NiPSysRotationModifier& object, const Niflib::NiPSysRotationModifier* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, WriteSyncer>
	{
		void operator() (const NiPSysRotationModifier& object, Niflib::NiPSysRotationModifier* native, const File& file);
	};

	//BSPSysScaleModifier
	template<> struct type_map<Niflib::BSPSysScaleModifier> { using type = BSPSysScaleModifier; };
	template<> struct type_map<BSPSysScaleModifier> { using type = Niflib::BSPSysScaleModifier; };

	template<> struct ReadSyncer<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, ReadSyncer>
	{
		void operator() (BSPSysScaleModifier& object, const Niflib::BSPSysScaleModifier* native, File& file);
	};
	template<> struct WriteSyncer<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, WriteSyncer>
	{
		void operator() (const BSPSysScaleModifier& object, Niflib::BSPSysScaleModifier* native, const File& file);
	};

	//BSPSysSimpleColorModifier
	template<> struct type_map<Niflib::BSPSysSimpleColorModifier> { using type = BSPSysSimpleColorModifier; };
	template<> struct type_map<BSPSysSimpleColorModifier> { using type = Niflib::BSPSysSimpleColorModifier; };

	template<> struct ReadSyncer<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, ReadSyncer>
	{
		void operator() (BSPSysSimpleColorModifier& object, const Niflib::BSPSysSimpleColorModifier* native, File& file);
	};
	template<> struct WriteSyncer<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, WriteSyncer>
	{
		void operator() (const BSPSysSimpleColorModifier& object, Niflib::BSPSysSimpleColorModifier* native, const File& file);
	};
}
