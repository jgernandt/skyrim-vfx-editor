#pragma once
#include "TestConstructor.h"
#include "nif.h"

namespace objects
{
	using namespace nif;

	template<typename T>
	struct FactoryTester : VerticalTraverser<T, FactoryTester>
	{
		//Should prepare object to have a certain structure. 
		//Unlike ConnectorTester, may need to also set up the Constructor.
		bool operator() (T&, TestConstructor&, File&) { return true; }

		//Should test that Constructor has the expected Node
		bool operator() (const T&, const TestConstructor&) { Assert::Fail(); return true; }
	};

	template<>
	struct FactoryTester<NiObjectNET> : VerticalTraverser<NiObjectNET, FactoryTester>
	{
		bool operator() (NiObjectNET&, TestConstructor&, File&) { return false; }
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiAVObject> : VerticalTraverser<NiAVObject, FactoryTester>
	{
		bool operator() (NiAVObject&, TestConstructor&, File&) { return false; }
		bool operator() (const NiAVObject& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiNode> : VerticalTraverser<NiNode, FactoryTester>
	{
		bool operator() (NiNode& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, FactoryTester>
	{
		bool operator() (BSEffectShaderProperty&, TestConstructor&, File&) { return false; }
		bool operator() (const BSEffectShaderProperty& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, FactoryTester>
	{
		bool operator() (NiPSysModifier& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiExtraData> : VerticalTraverser<NiExtraData, FactoryTester>
	{
		bool operator() (NiExtraData&, TestConstructor&, File&) { return false; }
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
			FactoryTester<T>{}.up(*obj, c, file);//this overload sets up obj and c
			node::Factory<T>{}.up(*obj, c);
			FactoryTester<T>{}.up(*obj, c);//this overload makes the assertions
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
