#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace traversal
{
	//Tests forwarding behaviour from each part independently
	TEST_CLASS(Forwarder)
	{
	public:

		struct ForwarderTester : nif::NiTraverser
		{
			ForwarderTester(std::vector<std::pair<nif::ni_type, nif::NiObject*>>& tr) :
				m_traversed{ tr } {}

			virtual void traverse(nif::NiObject& obj) { m_traversed.push_back({ nif::NiObject::TYPE, &obj }); }
			virtual void traverse(nif::NiObjectNET& obj) { m_traversed.push_back({ nif::NiObjectNET::TYPE, &obj }); }
			//etc (we really need a macro or template for this...)

			std::vector<std::pair<nif::ni_type, nif::NiObject*>>& m_traversed;
		};

		TEST_METHOD(NiObjectNET)
		{
			nif::NiObjectNET obj;

			auto data0 = std::make_shared<nif::NiStringExtraData>();
			auto data1 = std::make_shared<nif::NiExtraData>();
			obj.extraData.add(data0);
			obj.extraData.add(data1);

			auto ctlr0 = std::make_shared<nif::NiPSysEmitterCtlr>();
			auto ctlr1 = std::make_shared<nif::NiTimeController>();
			obj.controllers.insert(0, ctlr0);
			obj.controllers.insert(1, ctlr1);

			std::vector<std::pair<nif::ni_type, nif::NiObject*>> traversed;
			ForwarderTester t(traversed);
			nif::Forwarder<nif::NiObjectNET>{}(obj, t);

			//Iteration order of Set is not specified, so there are two valid outcomoes
			std::vector<std::pair<nif::ni_type, nif::NiObject*>> expected1 =
				{ { nif::NiStringExtraData::TYPE, data0.get() }, { nif::NiExtraData::TYPE, data1.get() },
				{ nif::NiPSysEmitterCtlr::TYPE, ctlr0.get() }, { nif::NiTimeController::TYPE, ctlr1.get() } };

			std::vector<std::pair<nif::ni_type, nif::NiObject*>> expected2 =
				{ { nif::NiExtraData::TYPE, data1.get() }, { nif::NiStringExtraData::TYPE, data0.get() },
				{ nif::NiPSysEmitterCtlr::TYPE, ctlr0.get() }, { nif::NiTimeController::TYPE, ctlr1.get() } };

			Assert::IsTrue(traversed == expected1 || traversed == expected2);
		}

	};

	TEST_CLASS(HorizontalTraverser)
	{
	public:
		class TestTraverser : public nif::HorizontalTraverser<TestTraverser>
		{
			nif::ni_type& m_type;

		public:
			TestTraverser(nif::ni_type& type) : m_type { type } {}
			template<typename T>
			void invoke(T&)
			{
				m_type = T::TYPE;
			}
		};

		template<typename T>
		struct Test
		{
			void run()
			{
				T object{};
				nif::ni_type type = nif::NiObject::TYPE;
				TestTraverser t{ type };
				static_cast<nif::NiObject&>(object).receive(t);
				Assert::IsTrue(type == T::TYPE);
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

	TEST_CLASS(VerticalTraverser)
	{
	public:
		template<typename T>
		struct TestTraverser : nif::VerticalTraverser<T, TestTraverser>
		{
			void operator() (T&, std::vector<nif::ni_type>& types)
			{
				types.push_back(T::TYPE);
			}
		};

		template<typename T>
		struct Test
		{
			void run()
			{
				T object{};
				std::vector<nif::ni_type> types;

				nif::ni_type type = T::TYPE;
				nif::ni_type baseType = T::base_type::TYPE;

				//This would be interesting to test as well, but it's not a test of traversal.
				//Also, we may not always *want* the same inheritance as Niflib. We may want
				//to skip some old unused steps.
				//nif::ni_type nativeBase = std::hash<std::string>{}(nif::type_map<T>::type::TYPE.base_type->GetTypeName());
				//Assert::IsTrue(baseType == nativeBase);

				TestTraverser<T>{}.down(object, types);
				Assert::IsTrue(types.size() > 1);
				Assert::IsTrue(types.back() == T::TYPE);
				Assert::IsTrue(types[types.size() - 2] == baseType);

				types.clear();

				TestTraverser<T>{}.up(object, types);
				Assert::IsTrue(types.size() > 1);
				Assert::IsTrue(types[0] == T::TYPE);
				Assert::IsTrue(types[1] == baseType);
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
