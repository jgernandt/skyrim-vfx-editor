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
#include "FlagSet.h"
#include "List.h"
#include "Property.h"
#include "Sequence.h"
#include "Set.h"
#include "Vector.h"

namespace nif
{
	class NiTraverser;

	template<typename T, typename Base>
	struct NiTraversable : Base
	{
		using base_type = Base;
		virtual void receive(NiTraverser& t) override;
	};

	struct NiObject
	{
		NiObject();
		NiObject(const NiObject&) = delete;
		NiObject(NiObject&&) = delete;

		~NiObject();

		NiObject& operator=(const NiObject&) = delete;
		NiObject& operator=(NiObject&&) = delete;

		using base_type = void;
		virtual void receive(NiTraverser& t);
	};

	//For the static mapping between Niflib types and our types
	template<typename T>
	struct type_map
	{
		//Specialise with the member type
		//using type = NiObject;
	};

	template<> struct type_map<Niflib::NiObject> { using type = NiObject; };
	template<> struct type_map<NiObject> { using type = Niflib::NiObject; };

	class File;

	//Transfers state between our model and Niflib (specialise and implement).
	//(we don't really need this to be a template, but doesn't hurt us either)
	template<typename T>
	class NiSyncer
	{
	public:
		void syncRead(File& file, T* object, typename type_map<T>::type* native) {}
		void syncWrite(const File& file, T* object, typename type_map<T>::type* native) {}
	};
}
