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
		NiExtraData(native::NiExtraData* obj);
		NiExtraData(const NiExtraData& other) = delete;

		virtual ~NiExtraData() = default;

		NiExtraData& operator=(const NiExtraData&) = delete;

		native::NiExtraData& getNative() const;

		IProperty<std::string>& name() { return m_name; }

	private:
		Property<std::string> m_name;
	};

	class NiStringExtraData : public NiExtraData
	{
	public:
		NiStringExtraData();
		NiStringExtraData(native::NiStringExtraData* obj);
		NiStringExtraData(const NiStringExtraData& other) = delete;

		virtual ~NiStringExtraData() = default;

		NiStringExtraData& operator=(const NiStringExtraData&) = delete;

		native::NiStringExtraData& getNative() const;

		IProperty<std::string>& value() { return m_value; }

	private:
		Property<std::string> m_value;
	};
}