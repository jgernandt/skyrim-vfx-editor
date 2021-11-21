#pragma once
#include "NiObject.h"

namespace nif
{
	class File;

	//Transfers state between our model and Niflib (specialise and implement).
	template<typename T>
	struct ReadSyncer : VerticalTraverser<T, ReadSyncer>
	{
		void operator() (T& object, const typename type_map<T>::type* native, File& file) {}
	};
	template<typename T>
	struct WriteSyncer : VerticalTraverser<T, WriteSyncer>
	{
		void operator() (const T& object, typename type_map<T>::type* native, const File& file) {}
	};

	//NiObject
	template<> struct type_map<Niflib::NiObject> { using type = NiObject; };
	template<> struct type_map<NiObject> { using type = Niflib::NiObject; };

	//NiObjectNET
	template<> struct type_map<Niflib::NiObjectNET> { using type = NiObjectNET; };
	template<> struct type_map<NiObjectNET> { using type = Niflib::NiObjectNET; };

	template<> struct ReadSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, ReadSyncer>
	{
		void operator() (NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};
	template<> struct WriteSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, WriteSyncer>
	{
		void operator() (const NiObjectNET& object, Niflib::NiObjectNET* native, const File& file);
	};

	//NiAVObject
	template<> struct type_map<Niflib::NiAVObject> { using type = NiAVObject; };
	template<> struct type_map<NiAVObject> { using type = Niflib::NiAVObject; };

	template<> struct ReadSyncer<NiAVObject> : VerticalTraverser<NiAVObject, ReadSyncer>
	{
		void operator() (NiAVObject& object, const Niflib::NiAVObject* native, File& file);
	};
	template<> struct WriteSyncer<NiAVObject> : VerticalTraverser<NiAVObject, WriteSyncer>
	{
		void operator() (const NiAVObject& object, Niflib::NiAVObject* native, const File& file);
	};
}
