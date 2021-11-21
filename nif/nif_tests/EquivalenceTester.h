#pragma once
#include "CppUnitTest.h"
#include "nif.h"

namespace common
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
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
	struct EquivalenceTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, EquivalenceTester>
	{
		void operator() (const NiPSysModifier& object, const Niflib::NiPSysModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, EquivalenceTester>
	{
		void operator() (const NiPSysGravityModifier& object, const Niflib::NiPSysGravityModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, EquivalenceTester>
	{
		void operator() (const NiPSysRotationModifier& object, const Niflib::NiPSysRotationModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, EquivalenceTester>
	{
		void operator() (const BSPSysScaleModifier& object, const Niflib::BSPSysScaleModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, EquivalenceTester>
	{
		void operator() (const BSPSysSimpleColorModifier& object, const Niflib::BSPSysSimpleColorModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, EquivalenceTester>
	{
		void operator() (const NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, EquivalenceTester>
	{
		void operator() (const NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, EquivalenceTester>
	{
		void operator() (const NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, EquivalenceTester>
	{
		void operator() (const NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, EquivalenceTester>
	{
		void operator() (const NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, EquivalenceTester>
	{
		void operator() (const NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, EquivalenceTester>
	{
		void operator() (const NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file);
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