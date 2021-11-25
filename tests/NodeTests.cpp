#include "pch.h"
#include "CppUnitTest.h"
#include "nodes.h"
#include "CommonTests.h"

namespace connectors
{
	using namespace nif;
	using namespace node;

	TEST_CLASS(ObjectNETTests)
	{
	public:

		TEST_METHOD(Extra_data)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<Node>(obj), obj->extraData, ObjectNET::EXTRA_DATA, true);
		}
	};

	TEST_CLASS(AVObjectTests)
	{
	public:

		TEST_METHOD(Parent)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiAVObject>();
			SetReceiverTest(std::make_unique<DummyAVObject>(obj), *obj, AVObject::PARENT, false);
		}
	};

	TEST_CLASS(NodeTests)
	{
	public:

		TEST_METHOD(References)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			AssignableReceiverTest(std::make_unique<Node>(obj), *obj, Node::OBJECT, true);
		}

		TEST_METHOD(Children)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<Node>(obj), obj->children, Node::CHILDREN, true);
		}

		/* We should not need to test this independently anymore
		TEST_METHOD(Load_hierarchy) {
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
			Assert::IsNotNull(root.get());

			auto c1 = file.create<nif::NiNode>();
			Assert::IsNotNull(c1.get());
			root->children().add(*c1);

			auto c2 = file.create<nif::NiNode>();
			Assert::IsNotNull(c2.get());
			root->children().add(*c2);

			auto c1c1 = file.create<nif::NiNode>();
			Assert::IsNotNull(c1c1.get());
			c1->children().add(*c1c1);

			Constructor c(file);
			c.makeRoot();

			Assert::IsTrue(c.size() == 4);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			Node* c1_node = findNode<Node>(c.nodes(), *c1);
			Node* c2_node = findNode<Node>(c.nodes(), *c2);
			Node* c1c1_node = findNode<Node>(c.nodes(), *c1c1);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(c1_node);
			Assert::IsNotNull(c2_node);
			Assert::IsNotNull(c1c1_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, c1_node->getField(Node::PARENT)->connector));
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, c2_node->getField(Node::PARENT)->connector));
			Assert::IsTrue(areConnected(c1_node->getField(Node::CHILDREN)->connector, c1c1_node->getField(Node::PARENT)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(root->getNative().GetChildren().size() == 2);
			Assert::IsTrue(c1->getNative().GetChildren().size() == 1);
			Assert::IsTrue(c2->getNative().GetChildren().size() == 0);
			Assert::IsTrue(c1c1->getNative().GetChildren().size() == 0);
			Assert::IsTrue(c1->getNative().GetParent() == &root->getNative());
			Assert::IsTrue(c2->getNative().GetParent() == &root->getNative());
			Assert::IsTrue(c1c1->getNative().GetParent() == &c1->getNative());
		}*/
	};
}
