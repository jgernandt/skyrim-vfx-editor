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
	struct NiExtraData : NiTraversable<NiExtraData, NiObject>
	{
		Property<std::string> name;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiExtraData> { using type = NiExtraData; };
	template<> struct type_map<NiExtraData> { using type = Niflib::NiExtraData; };
	template<> struct ReadSyncer<NiExtraData> : VerticalTraverser<NiExtraData, ReadSyncer>
	{
		void operator() (NiExtraData& object, Niflib::NiExtraData* native, File& file);
	};
	template<> struct WriteSyncer<NiExtraData> : VerticalTraverser<NiExtraData, WriteSyncer>
	{
		void operator() (NiExtraData& object, Niflib::NiExtraData* native, const File& file);
	};


	struct NiStringExtraData : NiTraversable<NiStringExtraData, NiExtraData>
	{
		Property<std::string> value;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiStringExtraData> { using type = NiStringExtraData; };
	template<> struct type_map<NiStringExtraData> { using type = Niflib::NiStringExtraData; };
	template<> struct ReadSyncer<NiStringExtraData> : VerticalTraverser<NiStringExtraData, ReadSyncer>
	{
		void operator() (NiStringExtraData& object, Niflib::NiStringExtraData* native, File& file);
	};
	template<> struct WriteSyncer<NiStringExtraData> : VerticalTraverser<NiStringExtraData, WriteSyncer>
	{
		void operator() (NiStringExtraData& object, Niflib::NiStringExtraData* native, const File& file);
	};

}
