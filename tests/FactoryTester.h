#pragma once
#include "CppUnitTest.h"
#include "TestConstructor.h"
#include "TestSetup.h"
#include "nif.h"

namespace objects
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	template<typename N>
	void nodeTest(const NiObject& obj, const TestConstructor& ctor)
	{
		Assert::IsTrue(ctor.node.first == &obj);
		Assert::IsNotNull(dynamic_cast<N*>(ctor.node.second.get()));
	}

	template<typename T>
	struct FactoryTester : VerticalTraverser<T, FactoryTester>
	{
		//Should test that Constructor has the expected Node
		bool operator() (const T&, const TestConstructor& ctor) 
		{
			Assert::IsTrue(!ctor.node.first && !ctor.node.second);
			return false;
		}
	};

	template<>
	struct FactoryTester<NiAVObject> : VerticalTraverser<NiAVObject, FactoryTester>
	{
		bool operator() (const NiAVObject& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiNode> : VerticalTraverser<NiNode, FactoryTester>
	{
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, FactoryTester>
	{
		bool operator() (const BSEffectShaderProperty& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, FactoryTester>
	{
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
		void controllerTest(const NiPSysModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, FactoryTester>
	{
		bool operator() (const NiPSysEmitter& obj, const TestConstructor& ctor);
		void controllerTest(const NiPSysEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, FactoryTester>
	{
		bool operator() (const NiPSysVolumeEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, FactoryTester>
	{
		bool operator() (const NiPSysBoxEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, FactoryTester>
	{
		bool operator() (const NiPSysCylinderEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, FactoryTester>
	{
		bool operator() (const NiPSysSphereEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiExtraData> : VerticalTraverser<NiExtraData, FactoryTester>
	{
		bool operator() (const NiExtraData& obj, const TestConstructor& ctor);
	};


	template<typename T>
	struct FactoryTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			c.pushObject(obj);
			TestSetup<T>{}.up(*obj, file);
			node::Factory<T>{}.up(*obj, c);
			FactoryTester<T>{}.up(*obj, c);
		}
	};

	template<>
	struct FactoryTest<NiStringExtraData>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiParticleSystem>
	{
		void run();
	};
}
