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
	struct ConnectorTester<NiNode> : VerticalTraverser<NiNode, ConnectorTester>
	{
		bool operator() (NiNode& obj, File& file);
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

}
