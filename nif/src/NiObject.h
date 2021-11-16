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
#include <cassert>
#include <map>
#include "nif_types.h"
#include "Assignable.h"
#include "Property.h"
#include "Sequence.h"
#include "Set.h"
#include "Vector.h"

namespace nif
{
	class File;

	class NiObject
	{
	public:
		using native_type = native::NiObject;

		NiObject(native_type* obj);

		NiObject(const NiObject&) = delete;
		NiObject(NiObject&&) = delete;

		virtual ~NiObject();

		NiObject& operator=(const NiObject&) = delete;
		NiObject& operator=(NiObject&&) = delete;

		//Pushes our data to the backend
		virtual void sync(const File&) {}

		//I don't like exposing this. Do we really need it?
		native_type* nativePtr() const { return m_ptr; }

		friend bool operator==(const NiObject& lhs, const NiObject& rhs) { return lhs.m_ptr == rhs.m_ptr; }
		friend bool operator!=(const NiObject& lhs, const NiObject& rhs) { return !(lhs == rhs); }
		friend bool operator<(const NiObject& lhs, const NiObject& rhs) { return lhs.m_ptr < rhs.m_ptr; }
		friend bool operator>(const NiObject& lhs, const NiObject& rhs) { return rhs < lhs; }
		friend bool operator<=(const NiObject& lhs, const NiObject& rhs) { return !(rhs < lhs); }
		friend bool operator>=(const NiObject& lhs, const NiObject& rhs) { return !(lhs < rhs); }

	protected:
		native::NiObject* const m_ptr;
	};

	//Convenience (and safety?) function for redirecting pointers to a field
	template<typename T, typename BlockType>
	inline std::shared_ptr<T> make_field_ptr(const std::shared_ptr<BlockType>& obj, T& field)
	{
		if (obj && field) {
			return std::shared_ptr<T>(obj, field);
		}
		else
			return std::shared_ptr<T>();
	}
}