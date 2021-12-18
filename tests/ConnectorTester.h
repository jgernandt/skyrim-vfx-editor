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
	struct ConnectorTester<NiControllerManager> : VerticalTraverser<NiControllerManager, ConnectorTester>
	{
		bool operator() (const NiControllerManager& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiControllerSequence> : VerticalTraverser<NiControllerSequence, ConnectorTester>
	{
		bool operator() (const NiControllerSequence& obj, const TestConstructor& ctor);
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
	struct ConnectorTester<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, ConnectorTester>
	{
		bool operator() (const NiPSysGravityModifier& obj, const TestConstructor& ctor);
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

	template<>
	struct ConnectorTester<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, ConnectorTester>
	{
		bool operator() (const NiPSysEmitterCtlr& obj, const TestConstructor& ctor);
	};

	template<>
	struct ConnectorTester<NiPSysGravityStrengthCtlr> : VerticalTraverser<NiPSysGravityStrengthCtlr, ConnectorTester>
	{
		bool operator() (const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor);
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
