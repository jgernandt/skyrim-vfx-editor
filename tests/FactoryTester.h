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

	template<typename N>
	void nodeTest(const NiObject& obj, const TestConstructor& ctor)
	{
		Assert::IsTrue(ctor.node.first == &obj);
		Assert::IsNotNull(dynamic_cast<N*>(ctor.node.second.get()));
	}

	template<typename T>
	struct FactoryTester : VerticalTraverser<T, FactoryTester>
	{
		//Should test that Constructor has the expected Node
		bool operator() (const T&, const TestConstructor& ctor) 
		{
			Assert::IsTrue(!ctor.node.first && !ctor.node.second);
			return false;
		}
	};

	template<>
	struct FactoryTester<NiAVObject> : VerticalTraverser<NiAVObject, FactoryTester>
	{
		bool operator() (const NiAVObject& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiNode> : VerticalTraverser<NiNode, FactoryTester>
	{
		bool operator() (const NiNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiBillboardNode> : VerticalTraverser<NiBillboardNode, FactoryTester>
	{
		bool operator() (const NiBillboardNode& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, FactoryTester>
	{
		bool operator() (const BSEffectShaderProperty& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiControllerManager> : VerticalTraverser<NiControllerManager, FactoryTester>
	{
		bool operator() (const NiControllerManager& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiControllerSequence> : VerticalTraverser<NiControllerSequence, FactoryTester>
	{
		bool operator() (const NiControllerSequence& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, FactoryTester>
	{
		bool operator() (const NiPSysModifier& obj, const TestConstructor& ctor);
		void controllerTest(const std::vector<NiPSysModifierCtlr*>& ctlrs, const ni_ptr<NiParticleSystem>& target);
	};

	template<>
	struct FactoryTester<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, FactoryTester>
	{
		bool operator() (const NiPSysGravityModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, FactoryTester>
	{
		bool operator() (const NiPSysRotationModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, FactoryTester>
	{
		bool operator() (const BSPSysScaleModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, FactoryTester>
	{
		bool operator() (const BSPSysSimpleColorModifier& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, FactoryTester>
	{
		bool operator() (const NiPSysEmitter& obj, const TestConstructor& ctor);
		void controllerTest(const std::vector<NiPSysModifierCtlr*>& ctlrs, const ni_ptr<NiParticleSystem>& target);
	};

	template<>
	struct FactoryTester<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, FactoryTester>
	{
		bool operator() (const NiPSysVolumeEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, FactoryTester>
	{
		bool operator() (const NiPSysBoxEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, FactoryTester>
	{
		bool operator() (const NiPSysCylinderEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, FactoryTester>
	{
		bool operator() (const NiPSysSphereEmitter& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, FactoryTester>
	{
		bool operator() (const NiPSysEmitterCtlr& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiPSysGravityStrengthCtlr> : VerticalTraverser<NiPSysGravityStrengthCtlr, FactoryTester>
	{
		bool operator() (const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor);
	};

	template<>
	struct FactoryTester<NiExtraData> : VerticalTraverser<NiExtraData, FactoryTester>
	{
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
			TestSetup<T>{}.up(*obj, file);
			node::Factory<T>{}.up(*obj, c);
			FactoryTester<T>{}.up(*obj, c);
		}
	};

	template<>
	struct FactoryTest<NiParticleSystem>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiPSysGravityModifier>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiPSysEmitterCtlr>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiPSysGravityStrengthCtlr>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiStringExtraData>
	{
		void run();
	};

	template<>
	struct FactoryTest<NiStringsExtraData>
	{
		void run();
	};

	template<>
	struct FactoryTest<BSBehaviorGraphExtraData>
	{
		void run();
	};
}
