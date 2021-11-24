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
		virtual void traverse(BSShaderProperty& obj) {}
		virtual void traverse(BSEffectShaderProperty& obj) {}

		virtual void traverse(NiBoolData& obj) {}
		virtual void traverse(NiFloatData& obj) {}

		virtual void traverse(NiInterpolator& obj) {}
		virtual void traverse(NiBoolInterpolator& obj) {}
		virtual void traverse(NiFloatInterpolator& obj) {}
		virtual void traverse(NiBlendInterpolator& obj) {}
		virtual void traverse(NiBlendBoolInterpolator& obj) {}
		virtual void traverse(NiBlendFloatInterpolator& obj) {}

		virtual void traverse(NiTimeController& obj) {}
		virtual void traverse(NiSingleInterpController& obj) {}

		virtual void traverse(NiParticleSystem& obj) {}
		virtual void traverse(NiPSysData& obj) {}

		virtual void traverse(NiPSysModifier& obj) {}
		virtual void traverse(NiPSysAgeDeathModifier& obj) {}
		virtual void traverse(NiPSysBoundUpdateModifier& obj) {}
		virtual void traverse(NiPSysGravityModifier& obj) {}
		virtual void traverse(NiPSysPositionModifier& obj) {}
		virtual void traverse(NiPSysRotationModifier& obj) {}
		virtual void traverse(BSPSysScaleModifier& obj) {}
		virtual void traverse(BSPSysSimpleColorModifier& obj) {}

		virtual void traverse(NiPSysEmitter& obj) {}
		virtual void traverse(NiPSysVolumeEmitter& obj) {}
		virtual void traverse(NiPSysBoxEmitter& obj) {}
		virtual void traverse(NiPSysCylinderEmitter& obj) {}
		virtual void traverse(NiPSysSphereEmitter& obj) {}

		virtual void traverse(NiPSysModifierCtlr& obj) {}
		virtual void traverse(NiPSysUpdateCtlr& obj) {}
		virtual void traverse(NiPSysEmitterCtlr& obj) {}

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
	//implement an operator() that returns bool and takes T& as first parameter.
	//True return from this operator indicates continue traversal.
	template<typename T, template<typename> typename TraverserType>
	struct VerticalTraverser : TraverserType<typename T::base_type>
	{
		//Traverse from the base type and down to T
		template<typename... Args>
		bool down(T& object, Args&&... args)
		{
			if (TraverserType<typename T::base_type>::down(object, std::forward<Args>(args)...))
				return static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...);
			else
				return false;
		}
		template<typename... Args>
		bool down(const T& object, Args&&... args)
		{
			if (TraverserType<typename T::base_type>::down(object, std::forward<Args>(args)...))
				return static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...);
			else
				return false;
		}

		//Traverse from T and up to the base type
		template<typename... Args>
		bool up(T& object, Args&&... args)
		{
			if (static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...))
				return TraverserType<typename T::base_type>::up(object, std::forward<Args>(args)...);
			else
				return false;
		}
		template<typename... Args>
		bool up(const T& object, Args&&... args)
		{
			if (static_cast<TraverserType<T>&>(*this)(object, std::forward<Args>(args)...))
				return TraverserType<typename T::base_type>::up(object, std::forward<Args>(args)...);
			else
				return false;
		}
	};

	
	//A pattern for templating NiTraverser. We'd rather not have to
	//stencil out all those traverse overloads more than once...

	//TraverserType should inherit HorizontalTraverser<TraverserType> 
	//and implement a template function void invoke(T&)
	template<typename TraverserType>
	class HorizontalTraverser : public NiTraverser
	{
	public:
		virtual ~HorizontalTraverser() = default;

		//implement in TraverserType
		//template<typename T> void invoke(T&) {}

		virtual void traverse(NiObject& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiObjectNET& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiAVObject& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiNode& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(BSFadeNode& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiProperty& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiAlphaProperty& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(BSShaderProperty& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(BSEffectShaderProperty& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiBoolData& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiFloatData& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiBoolInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiFloatInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiBlendInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiBlendBoolInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiBlendFloatInterpolator& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiTimeController& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiSingleInterpController& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiParticleSystem& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysData& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiPSysModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysAgeDeathModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysBoundUpdateModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysGravityModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysPositionModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysRotationModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(BSPSysScaleModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(BSPSysSimpleColorModifier& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiPSysEmitter& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysVolumeEmitter& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysBoxEmitter& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysCylinderEmitter& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysSphereEmitter& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiPSysModifierCtlr& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysUpdateCtlr& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiPSysEmitterCtlr& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }

		virtual void traverse(NiExtraData& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
		virtual void traverse(NiStringExtraData& obj) override { static_cast<TraverserType&>(*this).invoke(obj); }
	};
	
	//A variant that calls a separate function object instead of a member function.
	//Slightly more wordy, but has the advantage that specialisations of the function template
	//can be added outside the class that uses them.
	//TraverserType should inherit HorizontalTraverser<TraverserType, ImplementedFcnObj> 
	/*
	template<typename TraverserType, template<typename> typename Fcn>
	class HorizontalTraverser2 : public NiTraverser
	{
	public:
		virtual ~HorizontalTraverser2() = default;

		virtual void traverse(NiObject& obj) override { Fcn<NiObject>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiObjectNET& obj) override { Fcn<NiObjectNET>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiAVObject& obj) override { Fcn<NiAVObject>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiNode& obj) override { Fcn<NiNode>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(BSFadeNode& obj) override { Fcn<BSFadeNode>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiProperty& obj) override { Fcn<NiProperty>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiAlphaProperty& obj) override { Fcn<NiAlphaProperty>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(BSEffectShaderProperty& obj) override { Fcn<BSEffectShaderProperty>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiBoolData& obj) override { Fcn<NiBoolData>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiFloatData& obj) override { Fcn<NiFloatData>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiInterpolator& obj) override { Fcn<NiInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiBoolInterpolator& obj) override { Fcn<NiBoolInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiFloatInterpolator& obj) override { Fcn<NiFloatInterpolator>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiTimeController& obj) override { Fcn<NiTimeController>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiSingleInterpController& obj) override { Fcn<NiSingleInterpController>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiParticleSystem& obj) override { Fcn<NiParticleSystem>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiPSysData& obj) override { Fcn<NiPSysData>{}(obj, static_cast<TraverserType&>(*this)); }

		virtual void traverse(NiExtraData& obj) override { Fcn<NiExtraData>{}(obj, static_cast<TraverserType&>(*this)); }
		virtual void traverse(NiStringExtraData& obj) override { Fcn<NiStringExtraData>{}(obj, static_cast<TraverserType&>(*this)); }
	};

	class ExampleTraverser;

	template<typename T>
	struct ExampleFcn
	{
		void operator() (T& obj, ExampleTraverser& traverser);
	};

	class ExampleTraverser : public HorizontalTraverser2<ExampleTraverser, ExampleFcn>
	{
		template<typename T> friend struct ExampleFcn;
		int member;
	};

	template<typename T> 
	inline void ExampleFcn<T>::operator() (T& obj, ExampleTraverser& traverser)
	{
		int visible = traverser.member;
		//invoke vertical traversers or whatever
	}
	*/
}
