#pragma once
#include "NiPSysEmitter.h"
#include "NiPSysModifier_internal.h"

namespace nif
{
	//NiPSysEmitter
	template<> struct type_map<Niflib::NiPSysEmitter> { using type = NiPSysEmitter; };
	template<> struct type_map<NiPSysEmitter> { using type = Niflib::NiPSysEmitter; };

	template<> struct ReadSyncer<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, ReadSyncer>
	{
		bool operator() (NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, WriteSyncer>
	{
		bool operator() (const NiPSysEmitter& object, Niflib::NiPSysEmitter* native, const File& file);
	};

	//NiPSysVolumeEmitter
	template<> struct type_map<Niflib::NiPSysVolumeEmitter> { using type = NiPSysVolumeEmitter; };
	template<> struct type_map<NiPSysVolumeEmitter> { using type = Niflib::NiPSysVolumeEmitter; };

	template<> struct ReadSyncer<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, ReadSyncer>
	{
		bool operator() (NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, WriteSyncer>
	{
		bool operator() (const NiPSysVolumeEmitter& object, Niflib::NiPSysVolumeEmitter* native, const File& file);
	};

	//NiPSysBoxEmitter
	template<> struct type_map<Niflib::NiPSysBoxEmitter> { using type = NiPSysBoxEmitter; };
	template<> struct type_map<NiPSysBoxEmitter> { using type = Niflib::NiPSysBoxEmitter; };

	template<> struct ReadSyncer<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, ReadSyncer>
	{
		bool operator() (NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, WriteSyncer>
	{
		bool operator() (const NiPSysBoxEmitter& object, Niflib::NiPSysBoxEmitter* native, const File& file);
	};

	//NiPSysCylinderEmitter
	template<> struct type_map<Niflib::NiPSysCylinderEmitter> { using type = NiPSysCylinderEmitter; };
	template<> struct type_map<NiPSysCylinderEmitter> { using type = Niflib::NiPSysCylinderEmitter; };

	template<> struct ReadSyncer<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, ReadSyncer>
	{
		bool operator() (NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, WriteSyncer>
	{
		bool operator() (const NiPSysCylinderEmitter& object, Niflib::NiPSysCylinderEmitter* native, const File& file);
	};

	//NiPSysSphereEmitter
	template<> struct type_map<Niflib::NiPSysSphereEmitter> { using type = NiPSysSphereEmitter; };
	template<> struct type_map<NiPSysSphereEmitter> { using type = Niflib::NiPSysSphereEmitter; };

	template<> struct ReadSyncer<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, ReadSyncer>
	{
		bool operator() (NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, WriteSyncer>
	{
		bool operator() (const NiPSysSphereEmitter& object, Niflib::NiPSysSphereEmitter* native, const File& file);
	};
}
