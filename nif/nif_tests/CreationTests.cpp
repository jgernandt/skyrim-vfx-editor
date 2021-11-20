#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace creation
{
	using namespace nif;

	//Test that new objects are synced with their native
	TEST_CLASS(Sync)
	{
	public:
		template<typename T>
		struct Test
		{
			void run()
			{
				static nif::File file{ nif::File::Version::SKYRIM_SE };
				static std::mt19937 rng;//or do we want a new one for every test?

				Niflib::Ref<typename type_map<T>::type> native = new typename type_map<T>::type;
				T dummy{};

				common::Randomiser<T>{}.down(dummy, native, rng);
				auto object = file.get<T>(native);
				common::EquivalenceTester<T>{}.down(*object, native, file);
			}
		};

		TEST_METHOD(NiObjectNET) { Test<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(NiAVObject) { Test<nif::NiAVObject>{}.run(); }
		TEST_METHOD(NiNode) { Test<nif::NiNode>{}.run(); }
		TEST_METHOD(BSFadeNode) { Test<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(NiAlphaProperty) { Test<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(BSEffectShaderProperty) { Test<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(NiBoolData) { Test<nif::NiBoolData>{}.run(); }
		TEST_METHOD(NiFloatData) { Test<nif::NiFloatData>{}.run(); }
		TEST_METHOD(NiBoolInterpolator) { Test<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(NiFloatInterpolator) { Test<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(NiTimeController) { Test<nif::NiTimeController>{}.run(); }
		TEST_METHOD(NiSingleInterpController) { Test<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(NiParticleSystem) { Test<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(NiPSysData) { Test<nif::NiPSysData>{}.run(); }
		TEST_METHOD(NiExtraData) { Test<nif::NiExtraData>{}.run(); }
		TEST_METHOD(NiStringExtraData) { Test<nif::NiStringExtraData>{}.run(); }
	};

	//Test that File creates the correct objects
	TEST_CLASS(ObjectType)
	{
	public:

		template<typename T>
		struct Test
		{
			void run()
			{
				static nif::File file{ nif::File::Version::SKYRIM_SE };

				Niflib::Ref<Niflib::NiObject> native = new typename type_map<T>::type;
				auto object = file.get<NiObject>(native);
				Assert::IsTrue(object->type() == T::TYPE);
			}
		};
		TEST_METHOD(NiObjectNET) { Test<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(NiAVObject) { Test<nif::NiAVObject>{}.run(); }
		TEST_METHOD(NiNode) { Test<nif::NiNode>{}.run(); }
		TEST_METHOD(BSFadeNode) { Test<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(NiProperty) { Test<nif::NiProperty>{}.run(); }
		TEST_METHOD(NiAlphaProperty) { Test<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(BSShaderProperty) { Test<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(BSEffectShaderProperty) { Test<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(NiBoolData) { Test<nif::NiBoolData>{}.run(); }
		TEST_METHOD(NiFloatData) { Test<nif::NiFloatData>{}.run(); }
		TEST_METHOD(NiInterpolator) { Test<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(NiBoolInterpolator) { Test<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(NiFloatInterpolator) { Test<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(NiBlendInterpolator) { Test<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(NiBlendBoolInterpolator) { Test<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(NiBlendFloatInterpolator) { Test<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(NiTimeController) { Test<nif::NiTimeController>{}.run(); }
		TEST_METHOD(NiSingleInterpController) { Test<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(NiParticleSystem) { Test<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(NiPSysData) { Test<nif::NiPSysData>{}.run(); }
		TEST_METHOD(NiPSysEmitterCtlr) { Test<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(NiPSysEmitter) { Test<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(NiPSysVolumeEmitter) { Test<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(NiPSysBoxEmitter) { Test<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(NiPSysCylinderEmitter) { Test<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(NiPSysSphereEmitter) { Test<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(NiPSysModifierCtlr) { Test<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(NiPSysUpdateCtlr) { Test<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(NiPSysModifier) { Test<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(NiPSysBoundUpdateModifier) { Test<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(NiPSysAgeDeathModifier) { Test<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(NiPSysPositionModifier) { Test<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(NiPSysGravityModifier) { Test<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(NiPSysRotationModifier) { Test<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(BSPSysScaleModifier) { Test<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(BSPSysSimpleColorModifier) { Test<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(NiExtraData) { Test<nif::NiExtraData>{}.run(); }
		TEST_METHOD(NiStringExtraData) { Test<nif::NiStringExtraData>{}.run(); }
	};
}
