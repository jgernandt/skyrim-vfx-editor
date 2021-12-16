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
#include "TestSetup.h"
#include "nif.h"
#include "nodes.h"
#include "Editor.h"

namespace objects
{
	//Test that a controller manager is setup correctly when loading a file

	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	class MockAnimationManager : public HorizontalTraverser<MockAnimationManager>
	{
	public:
		std::vector<NiObject*> visited;
		std::vector<node::AnimationManager::BlockInfo> blocks;
		ControlledBlock* current{ nullptr };

		template<typename T>
		void invoke(T& obj)
		{
			//log obj so we can track who was visited
			visited.push_back(&obj);
		}

		void registerBlock(const node::AnimationManager::BlockInfo& b)
		{
			blocks.push_back(b);
		}
		void unregisterBlock(const node::AnimationManager::BlockInfo&)
		{
			Assert::Fail();
		}

		ControlledBlock* getCurrentBlock() const { return current; }
		void setCurrentBlock(ControlledBlock* block) { current = block; }
	};

	template<typename T>
	class AnimationTester
	{
	public:
		void operator() (const T&, MockAnimationManager&) { Assert::Fail(); }
	};

	template<>
	class AnimationTester<NiNode>
	{
	public:
		void operator() (const NiNode& obj, MockAnimationManager& visitor);
	};

	template<>
	class AnimationTester<NiTimeController>
	{
	public:
		void operator() (const NiTimeController& obj, MockAnimationManager& visitor);
	};

	template<>
	class AnimationTester<NiControllerManager>
	{
	public:
		void operator() (const NiControllerManager& obj, MockAnimationManager& visitor);
	};

	template<>
	class AnimationTester<NiPSysModifierCtlr>
	{
	public:
		void operator() (const NiPSysModifierCtlr& obj, MockAnimationManager& visitor);
	};


	template<typename T>
	struct AnimationTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestSetup<T>{}.up(*obj, file);

			ControlledBlock block;
			block.controller.assign(obj);
			block.nodeName.set("TestNodeName");
			block.propertyType.set("TestPropertyType");
			block.ctlrType.set("TestControllerType");
			block.ctlrID.set("TestControllerID");
			block.iplrID.set("TestInterpolatorID");

			MockAnimationManager v;
			v.setCurrentBlock(&block);

			node::AnimationInit<T>{}.up(*obj, v);
			AnimationTester<T>{}(*obj, v);
		}
	};

	template<>
	struct AnimationTest<NiNode>
	{
		//Like primary template without providing a block
		void run();
	};
}
