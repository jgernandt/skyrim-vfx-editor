//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include "nif_concepts.h"

namespace nif
{
	//These objects are effectively references. This is likely to cause confusion down the road.
	class NiObject
	{
	public:
		NiObject(native::NiObject* obj) : m_ptr(obj) { assert(obj); }//disallow null references
		NiObject(const NiObject&) = delete;

		virtual ~NiObject() = default;

		NiObject& operator=(const NiObject&) = delete;

		native::NiObject& getNative() const { return *m_ptr; }

	protected:
		ni_ptr<native::NiObject> m_ptr;
	};

	inline bool operator==(const NiObject& l, const NiObject& r) { return &l.getNative() == &r.getNative(); }
	inline bool operator!=(const NiObject& l, const NiObject& r) { return !(l == r); }
}