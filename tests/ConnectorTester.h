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
		bool operator() (T&, File&) { return true; }

		//Should test that Constructor has all the expected connections
		bool operator() (const T&, const TestConstructor&) { Assert::Fail(); return true; }
	};

	template<>
	struct ConnectorTester<NiObjectNET> : VerticalTraverser<NiObjectNET, ConnectorTester>
	{
		bool operator() (NiObjectNET& obj, File& file);
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiAVObject> : VerticalTraverser<NiAVObject, ConnectorTester>
	{
		bool operator() (NiAVObject&, File&) { return false; }
		bool operator() (const NiAVObject& obj, const TestConstructor& ctor);
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
	struct ConnectorTester<NiExtraData> : VerticalTraverser<NiExtraData, ConnectorTester>
	{
		bool operator() (NiExtraData&, File&) { return false; }
		bool operator() (const NiExtraData& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiStringExtraData> : VerticalTraverser<NiStringExtraData, ConnectorTester>
	{
		bool operator() (NiStringExtraData&, File&) { return false; }
		bool operator() (const NiStringExtraData& obj, const TestConstructor& ctor);
	};

}
