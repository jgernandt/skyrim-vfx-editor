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
	struct FactoryTester<NiNode> : VerticalTraverser<NiNode, FactoryTester>
	{
		bool operator() (NiNode& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};
}
