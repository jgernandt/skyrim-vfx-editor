#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nif_tests
{
	using namespace nif;

	TEST_CLASS(CreationTests)
	{
	public:
		//Test that File creates the correct objects

		TEST_METHOD(NiObjectNET)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiObjectNET;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiObjectNET::TYPE);
		}

		TEST_METHOD(NiAVObject)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiAVObject;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiAVObject::TYPE);
		}

		TEST_METHOD(NiNode)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiNode;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiNode::TYPE);
		}

		TEST_METHOD(BSFadeNode)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::BSFadeNode;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::BSFadeNode::TYPE);
		}

		TEST_METHOD(NiProperty)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiProperty;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiProperty::TYPE);
		}

		TEST_METHOD(NiAlphaProperty)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiAlphaProperty;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiAlphaProperty::TYPE);
		}

		TEST_METHOD(BSEffectShaderProperty)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::BSEffectShaderProperty;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::BSEffectShaderProperty::TYPE);
		}

		TEST_METHOD(NiBoolData)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiBoolData;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiBoolData::TYPE);
		}

		TEST_METHOD(NiFloatData)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiFloatData;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiFloatData::TYPE);
		}

		TEST_METHOD(NiInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiInterpolator::TYPE);
		}

		TEST_METHOD(NiBoolInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiBoolInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiBoolInterpolator::TYPE);
		}

		TEST_METHOD(NiFloatInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiFloatInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiFloatInterpolator::TYPE);
		}

		TEST_METHOD(NiBlendInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiBlendInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiBlendInterpolator::TYPE);
		}

		TEST_METHOD(NiBlendBoolInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiBlendBoolInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiBlendBoolInterpolator::TYPE);
		}

		TEST_METHOD(NiBlendFloatInterpolator)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiBlendFloatInterpolator;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiBlendFloatInterpolator::TYPE);
		}

		TEST_METHOD(NiTimeController)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiTimeController;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiTimeController::TYPE);
		}

		TEST_METHOD(NiSingleInterpController)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiSingleInterpController;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiSingleInterpController::TYPE);
		}

		TEST_METHOD(NiExtraData)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiExtraData;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiExtraData::TYPE);
		}

		TEST_METHOD(NiStringExtraData)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiStringExtraData;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiStringExtraData::TYPE);
		}

		TEST_METHOD(NiPSysData)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysData;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysData::TYPE);
		}

		TEST_METHOD(NiParticleSystem)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiParticleSystem;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiParticleSystem::TYPE);
		}

		TEST_METHOD(NiPSysEmitterCtlr)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysEmitterCtlr;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysEmitterCtlr::TYPE);
		}

		TEST_METHOD(NiPSysEmitter)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysEmitter;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysEmitter::TYPE);
		}

		TEST_METHOD(NiPSysVolumeEmitter)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysVolumeEmitter;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysVolumeEmitter::TYPE);
		}

		TEST_METHOD(NiPSysBoxEmitter)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysBoxEmitter;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysBoxEmitter::TYPE);
		}

		TEST_METHOD(NiPSysCylinderEmitter)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysCylinderEmitter;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysCylinderEmitter::TYPE);
		}

		TEST_METHOD(NiPSysSphereEmitter)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysSphereEmitter;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysSphereEmitter::TYPE);
		}

		TEST_METHOD(NiPSysModifierCtlr)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysModifierCtlr;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysModifierCtlr::TYPE);
		}

		TEST_METHOD(NiPSysUpdateCtlr)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysUpdateCtlr;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysUpdateCtlr::TYPE);
		}

		TEST_METHOD(NiPSysModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysModifier::TYPE);
		}

		TEST_METHOD(NiPSysBoundUpdateModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysBoundUpdateModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysBoundUpdateModifier::TYPE);
		}

		TEST_METHOD(NiPSysAgeDeathModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysAgeDeathModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysAgeDeathModifier::TYPE);
		}

		TEST_METHOD(NiPSysPositionModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysPositionModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysPositionModifier::TYPE);
		}

		TEST_METHOD(NiPSysGravityModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysGravityModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysGravityModifier::TYPE);
		}

		TEST_METHOD(NiPSysRotationModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::NiPSysRotationModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::NiPSysRotationModifier::TYPE);
		}

		TEST_METHOD(BSPSysScaleModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::BSPSysScaleModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::BSPSysScaleModifier::TYPE);
		}

		TEST_METHOD(BSPSysSimpleColorModifier)
		{
			File file(File::Version::SKYRIM_SE);
			Niflib::Ref<Niflib::NiObject> native = new Niflib::BSPSysSimpleColorModifier;
			auto object = file.get<NiObject>(native);
			Assert::IsTrue(object->type() == nif::BSPSysSimpleColorModifier::TYPE);
		}
	};
}
