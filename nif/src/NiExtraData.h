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
	struct NiExtraData : NiObject
	{
		Property<std::string> name;
	};
	template<> struct type_map<Niflib::NiExtraData> { using type = NiExtraData; };
	template<> struct type_map<NiExtraData> { using type = Niflib::NiExtraData; };

	template<> class NiSyncer<NiExtraData> : public SyncerInherit<NiExtraData, NiObject>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiExtraData* object, Niflib::NiExtraData* native) const;
		void syncWriteImpl(const File& file, NiExtraData* object, Niflib::NiExtraData* native) const;
	};

	struct NiStringExtraData : NiExtraData
	{
		Property<std::string> value;
	};
	template<> struct type_map<Niflib::NiStringExtraData> { using type = NiStringExtraData; };
	template<> struct type_map<NiStringExtraData> { using type = Niflib::NiStringExtraData; };

	template<> class NiSyncer<NiStringExtraData> : public SyncerInherit<NiStringExtraData, NiExtraData>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiStringExtraData* object, Niflib::NiStringExtraData* native) const;
		void syncWriteImpl(const File& file, NiStringExtraData* object, Niflib::NiStringExtraData* native) const;
	};
}