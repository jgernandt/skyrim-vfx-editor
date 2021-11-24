#pragma once
#include "NiExtraData.h"
#include "NiObject_internal.h"

namespace nif
{
	//NiExtraData
	template<> struct type_map<Niflib::NiExtraData> { using type = NiExtraData; };
	template<> struct type_map<NiExtraData> { using type = Niflib::NiExtraData; };
	template<> struct ReadSyncer<NiExtraData> : VerticalTraverser<NiExtraData, ReadSyncer>
	{
		bool operator() (NiExtraData& object, const Niflib::NiExtraData* native, File& file);
	};
	template<> struct WriteSyncer<NiExtraData> : VerticalTraverser<NiExtraData, WriteSyncer>
	{
		bool operator() (const NiExtraData& object, Niflib::NiExtraData* native, const File& file);
	};

	//NiStringExtraData
	template<> struct type_map<Niflib::NiStringExtraData> { using type = NiStringExtraData; };
	template<> struct type_map<NiStringExtraData> { using type = Niflib::NiStringExtraData; };
	template<> struct ReadSyncer<NiStringExtraData> : VerticalTraverser<NiStringExtraData, ReadSyncer>
	{
		bool operator() (NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file);
	};
	template<> struct WriteSyncer<NiStringExtraData> : VerticalTraverser<NiStringExtraData, WriteSyncer>
	{
		bool operator() (const NiStringExtraData& object, Niflib::NiStringExtraData* native, const File& file);
	};
}
