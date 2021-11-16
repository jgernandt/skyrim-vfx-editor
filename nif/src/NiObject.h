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

	//Use for the static mapping between Niflib types and our types
	template<typename T>
	struct type_map
	{
		//Specialise with the member type
		//using type = NiObject;
	};

	struct NiObject
	{
		NiObject();
		NiObject(const NiObject&) = delete;
		NiObject(NiObject&&) = delete;

		~NiObject();

		NiObject& operator=(const NiObject&) = delete;
		NiObject& operator=(NiObject&&) = delete;
	};

	//Convenience (?) function for redirecting pointers to a field
	template<typename T, typename BlockType>
	inline std::shared_ptr<T> make_field_ptr(const std::shared_ptr<BlockType>& obj, T& field)
	{
		if (obj && field) {
			return std::shared_ptr<T>(obj, field);
		}
		else
			return std::shared_ptr<T>();
	}

	//And sends something like this when it creates objects or writes output.
	//Specialisations are supplied by each object type.
	template<typename From, typename To>
	class NiSyncer
	{
	public:
		void operator()(From& from, To& to)
		{
			//match data
		}
	};
}