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
	class File;

	struct NiObject;

	class NiTraverser
	{
	public:
		virtual ~NiTraverser() = default;
		virtual void traverse(NiObject&) {}
	};

	template<typename T, typename Base>
	struct NiTraversable : Base
	{
		using base_type = Base;

		virtual void receive(NiTraverser& t) override
		{
			t.traverse(static_cast<T&>(*this));
		}
	};

	template<typename T>
	struct NiTraversable<T, void>
	{
		using base_type = void;

		virtual void receive(NiTraverser& t)
		{
			t.traverse(static_cast<T&>(*this));
		}
	};

	class NiReadSyncer : public NiTraverser
	{
	public:
		NiReadSyncer(File& file) {}
		virtual ~NiReadSyncer() = default;
		virtual void traverse(NiObject&) {}
	};

	class NiWriteSyncer : public NiTraverser
	{
	public:
		NiWriteSyncer(const File& file) {}
		virtual ~NiWriteSyncer() = default;
		virtual void traverse(NiObject&) {}
	};

	struct NiObject : NiTraversable<NiObject, void>
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
		virtual void syncRead(File&, NiObject*, Niflib::NiObject*) const {}
		virtual void syncWrite(const File&, NiObject*, Niflib::NiObject*) const {}
	};

	//Specialise and inherit
	template<typename T>
	class NiSyncer : public Syncer
	{
	public:
		virtual ~NiSyncer() = default;
		virtual void syncRead(File&, NiObject*, Niflib::NiObject*) const override {}
		virtual void syncWrite(const File&, NiObject*, Niflib::NiObject*) const override {}
	};

	template<> class NiSyncer<NiObject> : public Syncer { public: virtual ~NiSyncer() = default; };

	//Use as base for derived syncer classes to automate passing to the base, and to reuse some wordy casts
	template<typename Derived, typename Base>
	class SyncerInherit : public NiSyncer<Base>
	{
	public:
		virtual ~SyncerInherit() = default;
		virtual void syncRead(File& file, NiObject* object, Niflib::NiObject* native) const override
		{
			NiSyncer<Base>::syncRead(file, object, native);
			static_cast<const NiSyncer<Derived>&>(*this).syncReadImpl(
				file, 
				static_cast<Derived*>(object), 
				static_cast<typename type_map<Derived>::type*>(native));
		}
		virtual void syncWrite(const File& file, NiObject* object, Niflib::NiObject* native) const override
		{
			NiSyncer<Base>::syncWrite(file, object, native);
			static_cast<const NiSyncer<Derived>&>(*this).syncWriteImpl(
				file,
				static_cast<Derived*>(object),
				static_cast<typename type_map<Derived>::type*>(native));
		}
	};

	//Indexed by File, to keep track of created objects.
	//This object owns the resources that it points to and will clean them up on destruction.
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
		NiObjectBlock(const Niflib::Ref<Niflib::NiObject>& ref) :
			nativeRef{ ref }
		{
			if (!nativeRef)
				nativeRef = new typename type_map<T>::type();
			this->native = nativeRef;
			this->object = &objectImpl;
			this->syncer = &syncerImpl;
		}
		NiObjectBlock(const NiObjectBlock&) = delete;
		NiObjectBlock& operator=(const NiObjectBlock&) = delete;

		Niflib::Ref<Niflib::NiObject> nativeRef;
		T objectImpl;
		NiSyncer<T> syncerImpl;
	};

	template<typename T>
	std::shared_ptr<ObjectBlock> make_NiObject(const Niflib::Ref<Niflib::NiObject>& native)
	{
		return std::make_shared<NiObjectBlock<T>>(native);
	}
}