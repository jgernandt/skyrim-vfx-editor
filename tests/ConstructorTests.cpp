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

#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Constructor.inl"

namespace creation
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	TEST_CLASS(ConstructorTests)
	{
	public:

		//Constructor is expected to resolve the order of modifier connections.
		TEST_METHOD(Modifier_connections)
		{
			//Feed a Constructor some Modifier nodes and the ConnectionInfo we expect them to produce.
			//Test that the correct nodes are connected.
			File file(File::Version::SKYRIM_SE);
			node::AnimationManager am;
			node::Constructor ctor(file, am);

			auto psys = file.create<NiParticleSystem>();
			auto psys_node = node::Default<node::ParticleSystem>{}.create(file, psys);
			auto target = psys_node.get();

			ni_ptr<NiPSysModifier> mods[5];
			bool addNode[5]{ false, true, false, true, false };
			std::unique_ptr<node::DummyModifier> nodes[5];
			node::DummyModifier* node[5]{ nullptr, nullptr, nullptr, nullptr, nullptr };
			std::vector<NiPSysModifier*> reqs(5);

			ctor.addNode(psys.get(), std::move(psys_node));

			for (int i = 0; i < 5; i++) {
				mods[i] = file.create<NiPSysModifier>();
				mods[i]->order.set(i);
				reqs[i] = mods[i].get();
				if (addNode[i]) {
					nodes[i] = node::Default<node::DummyModifier>{}.create(file, mods[i]);
					node[i] = nodes[i].get();
					ctor.addNode(mods[i].get(), std::move(nodes[i]));
				}

			}
			ctor.addModConnections(psys.get(), std::move(reqs));

			gui::ConnectionHandler root;
			ctor.extractNodes(root, false);

			//target::MODIFIERS should be connected to node[1]::TARGET
			//node[1]::NEXT_MODIFIER should be connected to node[3]::TARGET

			Assert::IsTrue(root.getChildren().size() == 3);

			gui::Connector* cPSys = target->getField(node::ParticleSystem::MODIFIERS)->connector;
			gui::Connector* c1_target = node[1]->getField(node::Modifier::TARGET)->connector;
			gui::Connector* c1_next = node[1]->getField(node::Modifier::NEXT_MODIFIER)->connector;
			gui::Connector* c3_target = node[3]->getField(node::Modifier::TARGET)->connector;
			gui::Connector* c3_next = node[3]->getField(node::Modifier::NEXT_MODIFIER)->connector;

			Assert::IsTrue(areConnected(cPSys, c1_target));
			Assert::IsFalse(areConnected(cPSys, c3_target));
			Assert::IsTrue(areConnected(c1_next, c3_target));
			Assert::IsTrue(c3_next->getConnected().empty());
		}

		TEST_METHOD(PostProcessing)
		{
			File file(File::Version::SKYRIM_SE);
			node::AnimationManager am;
			node::Constructor ctor(file, am);

			bool test1 = false;
			bool test2 = false;

			ctor.addPostProcess([&test1]() { test1 = true; });
			ctor.addPostProcess([&test2]() { test2 = true; });

			TestRoot root;
			ctor.extractNodes(root);

			Assert::IsTrue(test1 && test2);
		}

		TEST_METHOD(SetAnimationManager)
		{
			//Make sure we call setAnimationManager on any added node
			struct MockNode : node::NodeBase 
			{
				node::AnimationManager* m_set{ nullptr };
				virtual void setAnimationManager(node::AnimationManager& am) override
				{
					m_set = &am;
				}
			};

			File file(File::Version::SKYRIM_SE);
			node::AnimationManager am;
			node::Constructor ctor(file, am);

			auto obj = file.create<NiObject>();
			auto unode = std::make_unique<MockNode>();
			MockNode* node = unode.get();
			ctor.addNode(obj.get(), std::move(unode));

			Assert::IsTrue(node->m_set == &am);
		}
	};
}
