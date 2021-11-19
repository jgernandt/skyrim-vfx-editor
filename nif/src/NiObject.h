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

#include "Traversal.h"

namespace nif
{
	class File;

	//For the static mapping between Niflib types and our types
	template<typename T>
	struct type_map
	{
		//Specialise with the member type
		//using type = NiObject;
	};

	//Forwards a horizontal traverser to subnodes
	template<typename T>
	struct Forwarder : VerticalTraverser<T, Forwarder>
	{
		void operator() (T& object, NiTraverser& traverser) {}
	};

	//Transfers state between our model and Niflib (specialise and implement).
	template<typename T>
	struct ReadSyncer : VerticalTraverser<T, ReadSyncer>
	{
		void operator() (T& object, const typename type_map<T>::type* native, File& file) {}
	};
	template<typename T>
	struct WriteSyncer : VerticalTraverser<T, WriteSyncer>
	{
		void operator() (const T& object, typename type_map<T>::type* native, const File& file) {}
	};

	using ni_type = size_t;

	struct NiObject : NiTraversable<NiObject, void>
	{
		NiObject();
		NiObject(const NiObject&) = delete;
		NiObject(NiObject&&) = delete;

		~NiObject();

		NiObject& operator=(const NiObject&) = delete;
		NiObject& operator=(NiObject&&) = delete;

		static const ni_type TYPE;
		virtual ni_type type() const { return TYPE; }
	};

	template<> struct type_map<Niflib::NiObject> { using type = NiObject; };
	template<> struct type_map<NiObject> { using type = Niflib::NiObject; };

	//We specialise this instead of requiring specialisations for each TraverserType<void>
	template<template<typename> typename TraverserType>
	struct VerticalTraverser<NiObject, TraverserType>
	{
		template<typename... Args>
		void down(NiObject& object, Args&&... args)
		{
			static_cast<TraverserType<NiObject>&>(*this)(object, std::forward<Args>(args)...);
		}
		template<typename... Args>
		void up(NiObject& object, Args&&... args)
		{
			static_cast<TraverserType<NiObject>&>(*this)(object, std::forward<Args>(args)...);
		}
	};


	struct NiObjectNET : NiTraversable<NiObjectNET, NiObject>
	{
		Property<std::string> name;
		Set<NiExtraData> extraData;
		Sequence<NiTimeController> controllers;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	template<> struct type_map<Niflib::NiObjectNET> { using type = NiObjectNET; };
	template<> struct type_map<NiObjectNET> { using type = Niflib::NiObjectNET; };

	template<> struct ReadSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, ReadSyncer>
	{
		void operator() (NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};
	template<> struct WriteSyncer<NiObjectNET> : VerticalTraverser<NiObjectNET, WriteSyncer>
	{
		void operator() (const NiObjectNET& object, Niflib::NiObjectNET* native, const File& file);
	};


	struct Transform
	{
		Property<translation_t> translation;
		Property<rotation_t> rotation;
		Property<scale_t> scale;
	};

	struct NiAVObject : NiTraversable<NiAVObject, NiObjectNET>
	{
		//flags is 32 bit, but it was 16 in Niflibs days. 
		//I can't be bothered to fix that right now. Will later.
		FlagSet<std::uint_fast32_t> flags;
		Transform transform;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiAVObject> { using type = NiAVObject; };
	template<> struct type_map<NiAVObject> { using type = Niflib::NiAVObject; };

	template<> struct ReadSyncer<NiAVObject> : VerticalTraverser<NiAVObject, ReadSyncer>
	{
		void operator() (NiAVObject& object, const Niflib::NiAVObject* native, File& file);
	};
	template<> struct WriteSyncer<NiAVObject> : VerticalTraverser<NiAVObject, WriteSyncer>
	{
		void operator() (const NiAVObject& object, Niflib::NiAVObject* native, const File& file);
	};
}
