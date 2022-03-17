//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor, a program for creating visual effects
//in the NetImmerse format.
//
//SVFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SVFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SVFX Editor. If not, see <https://www.gnu.org/licenses/>.

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
	struct ForwardTester<NiControllerManager> : VerticalTraverser<NiControllerManager, ForwardTester>
	{
		bool operator() (const NiControllerManager& obj, const TestConstructor& ctor);
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

	template<>
	struct ForwardTester<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, ForwardTester>
	{
		bool operator() (const NiPSysEmitterCtlr& obj, const TestConstructor& ctor);
	};

	template<>
	struct ForwardTester<NiPSysGravityStrengthCtlr> : VerticalTraverser<NiPSysGravityStrengthCtlr, ForwardTester>
	{
		bool operator() (const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor);
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

	template<>
	struct ForwardTest<NiPSysEmitterCtlr>
	{
		void run();
	};

	template<>
	struct ForwardTest<NiPSysGravityStrengthCtlr>
	{
		void run();
	};
}
