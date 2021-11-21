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
		void operator() (NiBoolData& object, const Niflib::NiBoolData* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolData> : VerticalTraverser<NiBoolData, WriteSyncer>
	{
		void operator() (const NiBoolData& object, Niflib::NiBoolData* native, const File& file);
	};

	//NiFloatData
	template<> struct type_map<Niflib::NiFloatData> { using type = NiFloatData; };
	template<> struct type_map<NiFloatData> { using type = Niflib::NiFloatData; };
	template<> struct ReadSyncer<NiFloatData> : VerticalTraverser<NiFloatData, ReadSyncer>
	{
		void operator() (NiFloatData& object, const Niflib::NiFloatData* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatData> : VerticalTraverser<NiFloatData, WriteSyncer>
	{
		void operator() (const NiFloatData& object, Niflib::NiFloatData* native, const File& file);
	};

	//NiInterpolator
	template<> struct type_map<Niflib::NiInterpolator> { using type = NiInterpolator; };
	template<> struct type_map<NiInterpolator> { using type = Niflib::NiInterpolator; };

	//NiBoolInterpolator
	template<> struct type_map<Niflib::NiBoolInterpolator> { using type = NiBoolInterpolator; };
	template<> struct type_map<NiBoolInterpolator> { using type = Niflib::NiBoolInterpolator; };

	template<> struct ReadSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, ReadSyncer>
	{
		void operator() (NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, WriteSyncer>
	{
		void operator() (const NiBoolInterpolator& object, Niflib::NiBoolInterpolator* native, const File& file);
	};

	//NiFloatInterpolator
	template<> struct type_map<Niflib::NiFloatInterpolator> { using type = NiFloatInterpolator; };
	template<> struct type_map<NiFloatInterpolator> { using type = Niflib::NiFloatInterpolator; };

	template<> struct ReadSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, ReadSyncer>
	{
		void operator() (NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, WriteSyncer>
	{
		void operator() (const NiFloatInterpolator& object, Niflib::NiFloatInterpolator* native, const File& file);
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
		void operator() (NiTimeController& object, const Niflib::NiTimeController* native, File& file);
	};
	template<> struct WriteSyncer<NiTimeController> : VerticalTraverser<NiTimeController, WriteSyncer>
	{
		void operator() (const NiTimeController& object, Niflib::NiTimeController* native, const File& file);
	};

	//NiSingleInterpController
	template<> struct type_map<Niflib::NiSingleInterpController> { using type = NiSingleInterpController; };
	template<> struct type_map<NiSingleInterpController> { using type = Niflib::NiSingleInterpController; };

	template<> struct ReadSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, ReadSyncer>
	{
		void operator() (NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file);
	};
	template<> struct WriteSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, WriteSyncer>
	{
		void operator() (const NiSingleInterpController& object, Niflib::NiSingleInterpController* native, const File& file);
	};

	//NiPSysModifierCtlr
	template<> struct type_map<Niflib::NiPSysModifierCtlr> { using type = NiPSysModifierCtlr; };
	template<> struct type_map<NiPSysModifierCtlr> { using type = Niflib::NiPSysModifierCtlr; };

	template<> struct ReadSyncer<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, ReadSyncer>
	{
		void operator() (NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, WriteSyncer>
	{
		void operator() (const NiPSysModifierCtlr& object, Niflib::NiPSysModifierCtlr* native, const File& file);
	};

	//NiPSysUpdateCtlr
	template<> struct type_map<Niflib::NiPSysUpdateCtlr> { using type = NiPSysUpdateCtlr; };
	template<> struct type_map<NiPSysUpdateCtlr> { using type = Niflib::NiPSysUpdateCtlr; };

	//NiPSysEmitterCtlr
	template<> struct type_map<Niflib::NiPSysEmitterCtlr> { using type = NiPSysEmitterCtlr; };
	template<> struct type_map<NiPSysEmitterCtlr> { using type = Niflib::NiPSysEmitterCtlr; };

	template<> struct ReadSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, ReadSyncer>
	{
		void operator() (NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, WriteSyncer>
	{
		void operator() (const NiPSysEmitterCtlr& object, Niflib::NiPSysEmitterCtlr* native, const File& file);
	};
}
