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
			node::Constructor ctor(file);

			auto psys = file.create<NiParticleSystem>();
			auto psys_node = node::Default<node::ParticleSystem>{}.create(file, psys);
			auto target = psys_node.get();

			ni_ptr<NiPSysModifier> mods[5];
			bool addNode[5]{ false, true, false, true, false };
			std::unique_ptr<node::DummyModifier> nodes[5];
			node::DummyModifier* node[5]{ nullptr, nullptr, nullptr, nullptr, nullptr };

			ctor.addNode(psys.get(), std::move(psys_node));

			for (int i = 0; i < 5; i++) {
				mods[i] = file.create<NiPSysModifier>();
				mods[i]->order.set(i);
				if (addNode[i]) {
					nodes[i] = node::Default<node::DummyModifier>{}.create(file, mods[i]);
					node[i] = nodes[i].get();
					ctor.addNode(mods[i].get(), std::move(nodes[i]));
				}

				node::ConnectionInfo info;
				info.object1 = mods[i].get();
				info.field1 = node::Modifier::TARGET;
				info.object2 = psys.get();
				info.field2 = node::ParticleSystem::MODIFIERS;
				info.order = i;
				ctor.addConnection(info);
			}

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
	};
}
