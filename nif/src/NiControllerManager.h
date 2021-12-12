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
#include "NiController.h"
#include "NiExtraData.h"

namespace nif
{
	struct NiControllerManager : NiTraversable<NiControllerManager, NiTimeController>
	{
		Property<bool> cumulative;
		Set<NiControllerSequence> ctlrSequences;//does order matter here?
		Ref<NiDefaultAVObjectPalette> objectPalette;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiControllerManager> : VerticalTraverser<NiControllerManager, Forwarder>
	{
		bool operator() (NiControllerManager& object, NiTraverser& traverser);
	};

	struct ControlledBlock
	{
		Ref<NiInterpolator> interpolator;
		Ref<NiTimeController> controller;
		Property<std::uint_fast8_t> priority;
		Property<std::string> nodeName;
		Property<std::string> propertyType;
		Property<std::string> ctlrType;
		Property<std::string> ctlrID;
		Property<std::string> iplrID;
	};

	struct NiControllerSequence : NiTraversable<NiControllerSequence, NiObject>
	{
		Property<std::string> name;
		Property<unsigned int> arrayGrowBy;
		List<ControlledBlock> blocks;
		Property<float> weight;
		Ref<NiTextKeyExtraData> textKeys;
		Property<CycleType> cycleType;
		Property<float> frequency;
		Property<float> startTime;
		Property<float> stopTime;
		Ptr<NiControllerManager> manager;
		Property<std::string> accumRootName;
		//Set<BSAnimNotes> animNotes;//introduce when we need it

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct Forwarder<NiControllerSequence> : VerticalTraverser<NiControllerSequence, Forwarder>
	{
		bool operator() (NiControllerSequence& object, NiTraverser& traverser);
	};

	struct AVObject
	{
		Property<std::string> name;
		Ptr<NiAVObject> object;
	};

	struct NiDefaultAVObjectPalette : NiTraversable<NiDefaultAVObjectPalette, NiObject>
	{
		Ptr<NiAVObject> scene;
		List<AVObject> objects;

		//We could just store a Set<NiAVObject>. There's really no point in storing the name
		//separately, when it is hard required to be the same as the name of the object.
		//Set<NiAVObject> objects;
		//(this means we keep strong refs, but that shouldn't be a problem)
		//I think I prefer this to be close to the nif format, though. Even if that's more work.

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}
