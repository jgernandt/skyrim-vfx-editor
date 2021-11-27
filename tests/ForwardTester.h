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
	struct ForwardTester : VerticalTraverser<T, ForwardTester>
	{
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
		bool operator() (const NiObjectNET& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiNode> : VerticalTraverser<NiNode, ForwardTester>
	{
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, ForwardTester>
	{
		bool operator() (const NiParticleSystem& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, ForwardTester>
	{
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
	};


	template<typename T>
	struct ForwardTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			TestSetup<T>{}.up(*obj, file);
			node::Forwarder<T>{}.down(*obj, c);
			ForwardTester<T>{}.up(*obj, c);
		}
	};
}
