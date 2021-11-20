#pragma once
#include "nif.h"

namespace common
{
	using namespace nif;

	//Test an object and a native for equivalence in all fields
	template<typename T>
	struct EquivalenceTester : VerticalTraverser<T, EquivalenceTester>
	{
		void operator() (const T& object, const typename type_map<T>::type* native, File& file) {}
	};

	template<>
	struct EquivalenceTester<NiObjectNET> : VerticalTraverser<NiObjectNET, EquivalenceTester>
	{
		void operator() (const NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiAVObject> : VerticalTraverser<NiAVObject, EquivalenceTester>
	{
		void operator() (const NiAVObject& object, const Niflib::NiAVObject* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiNode> : VerticalTraverser<NiNode, EquivalenceTester>
	{
		void operator() (const NiNode& object, const Niflib::NiNode* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, EquivalenceTester>
	{
		void operator() (const NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, EquivalenceTester>
	{
		void operator() (const BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiBoolData> : VerticalTraverser<NiBoolData, EquivalenceTester>
	{
		void operator() (const NiBoolData& object, const Niflib::NiBoolData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiFloatData> : VerticalTraverser<NiFloatData, EquivalenceTester>
	{
		void operator() (const NiFloatData& object, const Niflib::NiFloatData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, EquivalenceTester>
	{
		void operator() (const NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, EquivalenceTester>
	{
		void operator() (const NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiTimeController> : VerticalTraverser<NiTimeController, EquivalenceTester>
	{
		void operator() (const NiTimeController& object, const Niflib::NiTimeController* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, EquivalenceTester>
	{
		void operator() (const NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, EquivalenceTester>
	{
		void operator() (const NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysData> : VerticalTraverser<NiPSysData, EquivalenceTester>
	{
		void operator() (const NiPSysData& object, const Niflib::NiPSysData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiExtraData> : VerticalTraverser<NiExtraData, EquivalenceTester>
	{
		void operator() (const NiExtraData& object, const Niflib::NiExtraData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiStringExtraData> : VerticalTraverser<NiStringExtraData, EquivalenceTester>
	{
		void operator() (const NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file);
	};
}