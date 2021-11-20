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

		virtual void traverse(NiBoolData& obj) {}
		virtual void traverse(NiFloatData& obj) {}

		virtual void traverse(NiInterpolator& obj) {}
		virtual void traverse(NiBoolInterpolator& obj) {}
		virtual void traverse(NiFloatInterpolator& obj) {}

		virtual void traverse(NiTimeController& obj) {}
		virtual void traverse(NiSingleInterpController& obj) {}

		virtual void traverse(NiParticleSystem& obj) {}
		virtual void traverse(NiPSysData& obj) {}

		virtual void traverse(NiExtraData& obj) {}
		virtual void traverse(NiStringExtraData& obj) {}
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
		template<typename... Args>
		void down(const T& object, Args&&... args)
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
		template<typename... Args>
		void up(const T& object, Args&&... args)
		{
			static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...);
			TraverserType<typename T::base_type>::up(object, std::forward<Args>(args)...);
		}
	};

	
	//A pattern for templating NiTraverser. We'd rather not have to
	//stencil out all those traverse overloads more than once...
	
	//An implemented type T inherits HorizontalTraverser<T, ImplementedFcnObj>.
	//This means that the function object has access to members on the implemented traverser type.
	template<typename TraverserType, template<typename> typename FunctionObj>
	class HorizontalTraverser : public NiTraverser
	{
	public:
		virtual ~HorizontalTraverser() = default;

		virtual void traverse(NiObject& obj) override { FunctionObj<NiObject>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiObjectNET& obj) override { FunctionObj<NiObjectNET>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiAVObject& obj) override { FunctionObj<NiAVObject>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiNode& obj) override { FunctionObj<NiNode>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(BSFadeNode& obj) override { FunctionObj<BSFadeNode>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiProperty& obj) override { FunctionObj<NiProperty>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiAlphaProperty& obj) override { FunctionObj<NiAlphaProperty>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(BSEffectShaderProperty& obj) override { FunctionObj<BSEffectShaderProperty>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiBoolData& obj) override { FunctionObj<NiBoolData>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiFloatData& obj) override { FunctionObj<NiFloatData>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiInterpolator& obj) override { FunctionObj<NiInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiBoolInterpolator& obj) override { FunctionObj<NiBoolInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiFloatInterpolator& obj) override { FunctionObj<NiFloatInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiTimeController& obj) override { FunctionObj<NiTimeController>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiSingleInterpController& obj) override { FunctionObj<NiSingleInterpController>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiParticleSystem& obj) override { FunctionObj<NiParticleSystem>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiPSysData& obj) override { FunctionObj<NiPSysData>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiExtraData& obj) override { FunctionObj<NiExtraData>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiStringExtraData& obj) override { FunctionObj<NiStringExtraData>{}(obj, static_cast<TraverserType&>(*this)); }
	};

	class ExampleTraverser;

	template<typename T>
	struct ExampleFcn
	{
		void operator() (T& obj, ExampleTraverser& traverser);
	};

	class ExampleTraverser : public HorizontalTraverser<ExampleTraverser, ExampleFcn>
	{
		template<typename T> friend struct ExampleFcn;
		int member;

	public:
		virtual void traverse(BSFadeNode&) override { /*do something special*/ }
		//else it goes to ExampleFcn by default
	};

	template<typename T> 
	inline void ExampleFcn<T>::operator() (T& obj, ExampleTraverser& traverser)
	{
		int visible = traverser.member;
		//invoke vertical traversers or whatever
	}
}
