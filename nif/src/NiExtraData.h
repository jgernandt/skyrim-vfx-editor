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

	template<> class NiSyncer<NiExtraData> : public NiSyncer<NiObject>
	{
	public:
		virtual ~NiSyncer() = default;
		virtual void syncRead(File& file, NiObject* object, Niflib::NiObject* native) const override;
		virtual void syncWrite(File& file, NiObject* object, Niflib::NiObject* native) const override;
	};

	struct NiStringExtraData : NiExtraData
	{
		Property<std::string> value;
	};
	template<> struct type_map<Niflib::NiStringExtraData> { using type = NiStringExtraData; };
	template<> struct type_map<NiStringExtraData> { using type = Niflib::NiStringExtraData; };

	template<> class NiSyncer<NiStringExtraData> : public NiSyncer<NiExtraData>
	{
	public:
		virtual ~NiSyncer() = default;
		virtual void syncRead(File& file, NiObject* object, Niflib::NiObject* native) const override;
		virtual void syncWrite(File& file, NiObject* object, Niflib::NiObject* native) const override;
	};
}