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
#include "nif_data.h"
#include "nif_types.h"
#include "Traversal.h"

namespace nif
{
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

	template<> struct Forwarder<NiObjectNET> : VerticalTraverser<NiObjectNET, Forwarder>
	{
		void operator() (NiObjectNET& object, NiTraverser& traverser);
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
}
