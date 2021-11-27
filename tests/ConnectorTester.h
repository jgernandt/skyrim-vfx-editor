#pragma once
#include "TestConstructor.h"
#include "nif.h"

namespace objects
{
	using namespace nif;

	template<typename T>
	struct ConnectorTester : VerticalTraverser<T, ConnectorTester>
	{
		//Should prepare object to have a certain structure
		bool operator() (T&, File&) { return false; }

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
		bool operator() (NiObjectNET& obj, File& file);
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiNode> : VerticalTraverser<NiNode, ConnectorTester>
	{
		bool operator() (NiNode& obj, File& file);
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ConnectorTester>
	{
		bool operator() (NiParticleSystem& obj, File& file);
		bool operator() (const NiParticleSystem& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, ConnectorTester>
	{
		bool operator() (NiPSysModifier& obj, File& file);
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, ConnectorTester>
	{
		bool operator() (NiPSysBoxEmitter& obj, File& file);
		bool operator() (const NiPSysBoxEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, ConnectorTester>
	{
		bool operator() (NiPSysCylinderEmitter& obj, File& file);
		bool operator() (const NiPSysCylinderEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, ConnectorTester>
	{
		bool operator() (NiPSysSphereEmitter& obj, File& file);
		bool operator() (const NiPSysSphereEmitter& obj, const TestConstructor& ctor);
	};

}
