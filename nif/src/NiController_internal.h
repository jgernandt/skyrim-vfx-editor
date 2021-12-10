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
#include "NiObject_internal.h"

namespace nif
{
	//NiBoolData
	template<> struct type_map<Niflib::NiBoolData> { using type = NiBoolData; };
	template<> struct type_map<NiBoolData> { using type = Niflib::NiBoolData; };
	template<> struct ReadSyncer<NiBoolData> : VerticalTraverser<NiBoolData, ReadSyncer>
	{
		bool operator() (NiBoolData& object, const Niflib::NiBoolData* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolData> : VerticalTraverser<NiBoolData, WriteSyncer>
	{
		bool operator() (const NiBoolData& object, Niflib::NiBoolData* native, const File& file);
	};

	//NiFloatData
	template<> struct type_map<Niflib::NiFloatData> { using type = NiFloatData; };
	template<> struct type_map<NiFloatData> { using type = Niflib::NiFloatData; };
	template<> struct ReadSyncer<NiFloatData> : VerticalTraverser<NiFloatData, ReadSyncer>
	{
		bool operator() (NiFloatData& object, const Niflib::NiFloatData* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatData> : VerticalTraverser<NiFloatData, WriteSyncer>
	{
		bool operator() (const NiFloatData& object, Niflib::NiFloatData* native, const File& file);
	};

	//NiInterpolator
	template<> struct type_map<Niflib::NiInterpolator> { using type = NiInterpolator; };
	template<> struct type_map<NiInterpolator> { using type = Niflib::NiInterpolator; };

	//NiBoolInterpolator
	template<> struct type_map<Niflib::NiBoolInterpolator> { using type = NiBoolInterpolator; };
	template<> struct type_map<NiBoolInterpolator> { using type = Niflib::NiBoolInterpolator; };

	template<> struct ReadSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, ReadSyncer>
	{
		bool operator() (NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, WriteSyncer>
	{
		bool operator() (const NiBoolInterpolator& object, Niflib::NiBoolInterpolator* native, const File& file);
	};

	//NiFloatInterpolator
	template<> struct type_map<Niflib::NiFloatInterpolator> { using type = NiFloatInterpolator; };
	template<> struct type_map<NiFloatInterpolator> { using type = Niflib::NiFloatInterpolator; };

	template<> struct ReadSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, ReadSyncer>
	{
		bool operator() (NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, WriteSyncer>
	{
		bool operator() (const NiFloatInterpolator& object, Niflib::NiFloatInterpolator* native, const File& file);
	};

	//NiBlendInterpolator
	template<> struct type_map<Niflib::NiBlendInterpolator> { using type = NiBlendInterpolator; };
	template<> struct type_map<NiBlendInterpolator> { using type = Niflib::NiBlendInterpolator; };

	//NiBlendBoolInterpolator
	template<> struct type_map<Niflib::NiBlendBoolInterpolator> { using type = NiBlendBoolInterpolator; };
	template<> struct type_map<NiBlendBoolInterpolator> { using type = Niflib::NiBlendBoolInterpolator; };

	//NiBlendFloatInterpolator
	template<> struct type_map<Niflib::NiBlendFloatInterpolator> { using type = NiBlendFloatInterpolator; };
	template<> struct type_map<NiBlendFloatInterpolator> { using type = Niflib::NiBlendFloatInterpolator; };

	//NiTimeController
	template<> struct type_map<Niflib::NiTimeController> { using type = NiTimeController; };
	template<> struct type_map<NiTimeController> { using type = Niflib::NiTimeController; };

	template<> struct ReadSyncer<NiTimeController> : VerticalTraverser<NiTimeController, ReadSyncer>
	{
		bool operator() (NiTimeController& object, const Niflib::NiTimeController* native, File& file);
	};
	template<> struct WriteSyncer<NiTimeController> : VerticalTraverser<NiTimeController, WriteSyncer>
	{
		bool operator() (const NiTimeController& object, Niflib::NiTimeController* native, const File& file);
	};

	//NiSingleInterpController
	template<> struct type_map<Niflib::NiSingleInterpController> { using type = NiSingleInterpController; };
	template<> struct type_map<NiSingleInterpController> { using type = Niflib::NiSingleInterpController; };

	template<> struct ReadSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, ReadSyncer>
	{
		bool operator() (NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file);
	};
	template<> struct WriteSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, WriteSyncer>
	{
		bool operator() (const NiSingleInterpController& object, Niflib::NiSingleInterpController* native, const File& file);
	};

	//NiPSysUpdateCtlr
	template<> struct type_map<Niflib::NiPSysUpdateCtlr> { using type = NiPSysUpdateCtlr; };
	template<> struct type_map<NiPSysUpdateCtlr> { using type = Niflib::NiPSysUpdateCtlr; };

	//NiPSysModifierCtlr
	template<> struct type_map<Niflib::NiPSysModifierCtlr> { using type = NiPSysModifierCtlr; };
	template<> struct type_map<NiPSysModifierCtlr> { using type = Niflib::NiPSysModifierCtlr; };

	template<> struct ReadSyncer<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, ReadSyncer>
	{
		bool operator() (NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, WriteSyncer>
	{
		bool operator() (const NiPSysModifierCtlr& object, Niflib::NiPSysModifierCtlr* native, const File& file);
	};

	//NiPSysEmitterCtlr
	template<> struct type_map<Niflib::NiPSysEmitterCtlr> { using type = NiPSysEmitterCtlr; };
	template<> struct type_map<NiPSysEmitterCtlr> { using type = Niflib::NiPSysEmitterCtlr; };

	template<> struct ReadSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, ReadSyncer>
	{
		bool operator() (NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, WriteSyncer>
	{
		bool operator() (const NiPSysEmitterCtlr& object, Niflib::NiPSysEmitterCtlr* native, const File& file);
	};

	//NiPSysGravityStrengthCtlr
	template<> struct type_map<Niflib::NiPSysGravityStrengthCtlr> { using type = NiPSysGravityStrengthCtlr; };
	template<> struct type_map<NiPSysGravityStrengthCtlr> { using type = Niflib::NiPSysGravityStrengthCtlr; };
}
