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
#include "NiObject.h"

namespace nif
{
	struct NiBoolData : NiTraversable<NiBoolData, NiObject>
	{
		Property<KeyType> keyType;
		Vector<Key<bool>> keys;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiFloatData : NiTraversable<NiFloatData, NiObject>
	{
		Property<KeyType> keyType;
		Vector<Key<float>> keys;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiInterpolator : NiTraversable<NiInterpolator, NiObject> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiBoolInterpolator : NiTraversable<NiBoolInterpolator, NiInterpolator>
	{
		Property<bool> value;
		Ref<NiBoolData> data;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, Forwarder>
	{
		bool operator() (NiBoolInterpolator& object, NiTraverser& traverser);
	};

	struct NiFloatInterpolator : NiTraversable<NiFloatInterpolator, NiInterpolator>
	{
		Property<float> value;
		Ref<NiFloatData> data;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, Forwarder>
	{
		bool operator() (NiFloatInterpolator& object, NiTraverser& traverser);
	};

	struct NiBlendInterpolator : NiTraversable<NiBlendInterpolator, NiInterpolator> 
	{
		Property<bool> managerControlled;
		//Never mind the actual blend stuff right now. We'll only be doing manager controlled.

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiBlendBoolInterpolator : NiTraversable<NiBlendBoolInterpolator, NiBlendInterpolator> 
	{
		Property<bool> value;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiBlendFloatInterpolator : NiTraversable<NiBlendFloatInterpolator, NiBlendInterpolator> 
	{
		Property<float> value;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	
	struct NiTimeController : NiTraversable<NiTimeController, NiObject>
	{
		//Leave this out? It makes less sense to us.
		//Assignable<NiTimeController> nextCtlr;

		FlagSet<ControllerFlags> flags;
		Property<float> frequency;
		Property<float> phase;
		Property<float> startTime;
		Property<float> stopTime;
		Ptr<NiObjectNET> target;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiSingleInterpController : NiTraversable<NiSingleInterpController, NiTimeController>
	{
		Ref<NiInterpolator> interpolator;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, Forwarder>
	{
		bool operator() (NiSingleInterpController& object, NiTraverser& traverser);
	};

	struct NiPSysModifierCtlr : NiTraversable<NiPSysModifierCtlr, NiSingleInterpController>
	{
		Property<std::string> modifierName;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysUpdateCtlr : NiTraversable<NiPSysUpdateCtlr, NiTimeController>
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysEmitterCtlr : NiTraversable<NiPSysEmitterCtlr, NiPSysModifierCtlr>
	{
		Ref<NiInterpolator> visIplr;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, Forwarder>
	{
		bool operator() (NiPSysEmitterCtlr& object, NiTraverser& traverser);
	};

	struct NiPSysGravityStrengthCtlr : NiTraversable<NiPSysGravityStrengthCtlr, NiPSysModifierCtlr>
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}