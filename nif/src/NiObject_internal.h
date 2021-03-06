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
	class File;

	//Transfers state between our model and Niflib (specialise and implement).
	template<typename T>
	struct ReadSyncer : VerticalTraverser<T, ReadSyncer>
	{
		bool operator() (T& object, const typename type_map<T>::type* native, File& file) { return true; }
	};
	template<typename T>
	struct WriteSyncer : VerticalTraverser<T, WriteSyncer>
	{
		bool operator() (const T& object, typename type_map<T>::type* native, const File& file) { return true; }
	};

	//NiObject
	template<> struct type_map<Niflib::NiObject> { using type = NiObject; };
	template<> struct type_map<NiObject> { using type = Niflib::NiObject; };

	//NiObjectNET
	template<> struct type_map<Niflib::NiObjectNET> { using type = NiObjectNET; };
	template<> struct type_map<NiObjectNET> { using type = Niflib::NiObjectNET; };

	template<> struct ReadSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, ReadSyncer>
	{
		bool operator() (NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};
	template<> struct WriteSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, WriteSyncer>
	{
		bool operator() (const NiObjectNET& object, Niflib::NiObjectNET* native, const File& file);
	};

	//NiAVObject
	template<> struct type_map<Niflib::NiAVObject> { using type = NiAVObject; };
	template<> struct type_map<NiAVObject> { using type = Niflib::NiAVObject; };

	template<> struct ReadSyncer<NiAVObject> : VerticalTraverser<NiAVObject, ReadSyncer>
	{
		bool operator() (NiAVObject& object, const Niflib::NiAVObject* native, File& file);
	};
	template<> struct WriteSyncer<NiAVObject> : VerticalTraverser<NiAVObject, WriteSyncer>
	{
		bool operator() (const NiAVObject& object, Niflib::NiAVObject* native, const File& file);
	};
}
