#pragma once
#include "TestConstructor.h"
#include "nif.h"

namespace objects
{
	using namespace nif;

	template<typename T>
	struct ForwardTester : VerticalTraverser<T, ForwardTester>
	{
		//Should prepare object to have a certain structure. 
		//Unlike ConnectorTester, may need to also set up the Constructor.
		bool operator() (T&, TestConstructor&, File&) { return false; }

		//Should test that all the expected objects receive ctor and that ctor has the expected 
		//current object ptr/object stack at that time.
		bool operator() (const T&, const TestConstructor& ctor) 
		{
			Assert::IsTrue(ctor.forwards.empty());
			return false; 
		}
	};

	template<>
	struct ForwardTester<NiObjectNET> : VerticalTraverser<NiObjectNET, ForwardTester>
	{
		bool operator() (NiObjectNET& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiNode> : VerticalTraverser<NiNode, ForwardTester>
	{
		bool operator() (NiNode& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ForwardTester>
	{
		bool operator() (NiParticleSystem& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiParticleSystem& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, ForwardTester>
	{
		bool operator() (NiPSysModifier& obj, TestConstructor& ctor, File& file);
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
	};
}
