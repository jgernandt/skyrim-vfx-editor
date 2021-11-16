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
	class NiExtraData : public NiObject
	{
	public:
		using native_type = native::NiExtraData;

		NiExtraData(native_type* obj);
		virtual ~NiExtraData() = default;

		native_type* nativePtr() const;

		Property<std::string> name;
	};

	class NiStringExtraData : public NiExtraData
	{
	public:
		using native_type = native::NiStringExtraData;

		NiStringExtraData();
		NiStringExtraData(native_type* obj);
		virtual ~NiStringExtraData() = default;

		native_type* nativePtr() const;

		Property<std::string> value;
	};
}