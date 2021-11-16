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

	struct NiObject
	{
		NiObject();
		NiObject(const NiObject&) = delete;
		NiObject(NiObject&&) = delete;

		~NiObject();

		NiObject& operator=(const NiObject&) = delete;
		NiObject& operator=(NiObject&&) = delete;
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

	//Transfers state between our model and Niflib after read and before write
	class Syncer
	{
	public:
		virtual ~Syncer() = default;
		virtual void syncRead(const File&, NiObject*, Niflib::NiObject*) const = 0;
		virtual void syncWrite(const File&, NiObject*, Niflib::NiObject*) const = 0;
	};

	//Specialise and inherit
	template<typename T>
	class NiSyncer : public Syncer
	{
	public:
		virtual ~NiSyncer() = default;
		virtual void syncRead(const File&, NiObject*, Niflib::NiObject*) const override {}
		virtual void syncWrite(const File&, NiObject*, Niflib::NiObject*) const override {}
	};

	//like this
	template<>
	class NiSyncer<NiObject> : public Syncer
	{
	public:
		virtual ~NiSyncer() = default;
		virtual void syncRead(const File&, NiObject*, Niflib::NiObject*) const override {}
		virtual void syncWrite(const File&, NiObject*, Niflib::NiObject*) const override {}
	};

	//Indexed by File, to keep track of created objects
	struct ObjectBlock
	{
		Niflib::NiObject* native;
		NiObject* object;
		Syncer* syncer;
	};

	//Our implementation of ObjectBlock keeps everything in the same object.
	//The pointers in ObjectBlock are aliases for the members of the derived type.
	template<typename T>
	struct NiObjectBlock : ObjectBlock
	{
		NiObjectBlock(const Niflib::Ref<typename type_map<T>::type>& ref) :
			nativeRef{ ref }
		{
			this->native = nativeRef;
			this->object = &objectImpl;
			this->syncer = &syncerImpl;
		}
		NiObjectBlock(const NiObjectBlock&) = delete;
		NiObjectBlock& operator=(const NiObjectBlock&) = delete;

		Niflib::Ref<typename type_map<T>::type> nativeRef;
		T objectImpl;
		NiSyncer<T> syncerImpl;
	};
}