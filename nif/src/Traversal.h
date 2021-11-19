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
#include "nif_types.h"

namespace nif
{
	class NiTraverser
	{
	public:
		virtual ~NiTraverser() = default;
		virtual void traverse(NiObject& obj) {}
		virtual void traverse(NiObjectNET& obj) {}
		virtual void traverse(NiAVObject& obj) {}
		virtual void traverse(NiNode& obj) {}
		virtual void traverse(BSFadeNode& obj) {}

		virtual void traverse(NiProperty& obj) {}
		virtual void traverse(NiAlphaProperty& obj) {}
		virtual void traverse(BSEffectShaderProperty& obj) {}
		//etc.
	};

	//Inject into the inheritance chain of NiObjects. 
	//Provides the member type base_type, used for "vertical" traversal 
	// (i.e. traversal of the static inheritance chain),
	//and the virtual member function receive(NiTraverser&), used
	//for "horizontal" traversal (i.e. traversal of the dynamic graph of NiObjects).
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

	//TraverserType<T> should inherit VerticalTraverser<T, TraverserType> and 
	//implement an operator() overload with T& as first parameter
	template<typename T, template<typename> typename TraverserType>
	struct VerticalTraverser : TraverserType<typename T::base_type>
	{
		//Traverse from the base type and down to T
		template<typename... Args>
		void down(T& object, Args&&... args)
		{
			TraverserType<typename T::base_type>::down(object, std::forward<Args>(args)...);
			static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...);
		}
		//Traverse from T and up to the base type
		template<typename... Args>
		void up(T& object, Args&&... args)
		{
			static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...);
			TraverserType<typename T::base_type>::up(object, std::forward<Args>(args)...);
		}
	};

	/*
	//Brainstorming how to template the horizontal traverser. We'd rather not have to
	//stencil out all those traverse overloads more than once...

	//Parameters could be passed as a templated struct:
	template<template<typename> typename TraverserType> struct TraverserParams {};
	//However, this doesn't let us pass e.g. *this as a parameter. 
	//We can get around that, but it's weird.

	//Direction can easily be templated by an enum template arg and constexpr if:
	enum class TraverseDirection
	{
		DOWN,
		UP
	};

	//Forwarding behaviour options can be added the same way. 
	//(we could also template the forwarder class template)
	enum class ForwardingBehaviour
	{
		NONE,
		FIRST,
		LAST
	};

	template<typename T> struct Forwarder;

	template<
		template<typename> typename VerticalType, 
		template<typename> typename ForwarderType = Forwarder,
		ForwardingBehaviour Fwd = ForwardingBehaviour::LAST,
		TraverseDirection Dir = TraverseDirection::DOWN>
	class HorizontalTraverser : public NiTraverser
	{
	public:
		HorizontalTraverser(const TraverserParams<VerticalType>&) {}
		virtual ~HorizontalTraverser() = default;

		virtual void traverse(NiObject& obj) override
		{
			if constexpr (Fwd == ForwardingBehaviour::FIRST) {
				if constexpr (Dir == TraverseDirection::DOWN)
					ForwarderType<NiObject>{}.down(obj, *this);
				else
					ForwarderType<NiObject>{}.up(obj, *this);
			}

			if constexpr (Dir == TraverseDirection::DOWN)
				VerticalType<NiObject>{}.down(obj, params);
			else
				VerticalType<NiObject>{}.up(obj, params);

			if constexpr (Fwd == ForwardingBehaviour::LAST) {
				if constexpr (Dir == TraverseDirection::DOWN)
					ForwarderType<NiObject>{}.down(obj, *this);
				else
					ForwarderType<NiObject>{}.up(obj, *this);
			}
		}
		//etc.

	private:
		TraverserParams<VerticalType> params;
	};

	template<typename T>
	struct ExampleVerticalType : VerticalTraverser<T, ExampleVerticalType>
	{
		void operator() (T& object, TraverserParams<ExampleVerticalType>& params) {}
	};

	class ExampleTraverser : public HorizontalTraverser<ExampleVerticalType>
	{
	public:
		ExampleTraverser(const TraverserParams<ExampleVerticalType>& p) : HorizontalTraverser(p) {}
		virtual ~ExampleTraverser() = default;
	};

	//This isn't quite working, and I'm not sure it's a great idea anyway.
	*/
}
