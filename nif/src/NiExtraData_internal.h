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

	//NiStringsExtraData
	template<> struct type_map<Niflib::NiStringsExtraData> { using type = NiStringsExtraData; };
	template<> struct type_map<NiStringsExtraData> { using type = Niflib::NiStringsExtraData; };
	template<> struct ReadSyncer<NiStringsExtraData> : VerticalTraverser<NiStringsExtraData, ReadSyncer>
	{
		bool operator() (NiStringsExtraData& object, const Niflib::NiStringsExtraData* native, File& file);
	};
	template<> struct WriteSyncer<NiStringsExtraData> : VerticalTraverser<NiStringsExtraData, WriteSyncer>
	{
		bool operator() (const NiStringsExtraData& object, Niflib::NiStringsExtraData* native, const File& file);
	};
}
