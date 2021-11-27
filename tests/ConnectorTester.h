#pragma once
#include "CppUnitTest.h"
#include "TestConstructor.h"
#include "TestSetup.h"
#include "nif.h"

namespace objects
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	template<typename T>
	struct ConnectorTester : VerticalTraverser<T, ConnectorTester>
	{
		//Should test that Constructor has all the expected connections
		bool operator() (const T&, const TestConstructor& ctor) 
		{ 
			Assert::IsTrue(ctor.connections.empty() && ctor.modConnections.second.empty());
			return false; 
		}
	};

	template<>
	struct ConnectorTester<NiObjectNET> : VerticalTraverser<NiObjectNET, ConnectorTester>
	{
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiNode> : VerticalTraverser<NiNode, ConnectorTester>
	{
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ConnectorTester>
	{
		bool operator() (const NiParticleSystem& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, ConnectorTester>
	{
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, ConnectorTester>
	{
		bool operator() (const NiPSysVolumeEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, ConnectorTester>
	{
		bool operator() (const NiPSysBoxEmitter& obj, const TestConstructor& ctor) { return true; }
	};

	template<>
	struct ConnectorTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, ConnectorTester>
	{
		bool operator() (const NiPSysCylinderEmitter& obj, const TestConstructor& ctor) { return true; }
	};

	template<>
	struct ConnectorTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, ConnectorTester>
	{
		bool operator() (const NiPSysSphereEmitter& obj, const TestConstructor& ctor) { return true; }
	};


	template<typename T>
	struct ConnectorTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			TestSetup<T>{}.up(*obj, file);
			node::Connector<T>{}.down(*obj, c);
			ConnectorTester<T>{}.up(*obj, c);
		}
	};
}
