#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

#include "Constructor.h"

namespace nif
{
	TEST_CLASS(ObjectNETTests)
	{
	public:
		File file{ File::Version::SKYRIM_SE };

		TEST_METHOD(Name)
		{
			std::shared_ptr<NiObjectNET> obj = file.create<NiNode>();
			Assert::IsNotNull(obj.get());

			StringPropertyTest(obj->name());
		}

		TEST_METHOD(ExtraData)
		{
			std::shared_ptr<NiObjectNET> obj = file.create<NiNode>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiStringExtraData>(); };
			SetTest<nif::NiExtraData>(obj->extraData(), factory);
		}

		TEST_METHOD(Controllers)
		{
			std::shared_ptr<NiObjectNET> obj = file.create<NiNode>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiPSysEmitterCtlr>(); };
			SequenceTest<nif::NiTimeController>(obj->controllers(), factory);
		}
	};

	TEST_CLASS(AVObjectTests)
	{
	public:
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Transform)
		{
			std::shared_ptr<NiAVObject> obj = file.create<NiNode>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::translation_t>(obj->transform().translation(), m_engine);

			//Umm, what are we testing here, exactly? Isn't this stupid?
			struct CustomProperty : ObservableBase<IProperty<math::Rotation::EulerAngles>>
			{
				CustomProperty(IProperty<nif::rotation_t>& p) : prop{ p } {}
				virtual math::Rotation::EulerAngles get() const override
				{
					return prop.get().getEuler(order);
				}

				virtual void set(const math::Rotation::EulerAngles& val) override
				{
					if (val != get()) {
						prop.set(math::Rotation(val));
						for (auto&& l : getListeners())
							l->onSet(val);
					}
				}

				IProperty<nif::rotation_t>& prop;
				math::EulerOrder order{ math::EulerOrder::XYZ };

			} custom(obj->transform().rotation());

			PropertyTest<math::Rotation::euler_type>(custom, m_engine, 1.0e-3f);

			PropertyTest<float>(obj->transform().scale(), m_engine);
		}

	};

	TEST_CLASS(NodeTests)
	{
	public:

		TEST_METHOD(Parent)
		{
			File file{ File::Version::SKYRIM_SE };
			auto node = file.create<NiNode>();

			auto factory = [&file]() { return file.create<NiNode>(); };
			SetTest<nif::NiAVObject>(node->children(), factory);
		}
	};
}

namespace node
{
	TEST_CLASS(ObjectNETTests)
	{
		//Extra data connector should send ISet<NiExtraData> (multi)
		TEST_METHOD(Extra_data)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			std::unique_ptr<ObjectNET> node = std::make_unique<Root>(file);
			ISet<nif::NiExtraData>& set = node->object().extraData();

			auto factory = [&file]() { return file.create<nif::NiStringExtraData>(); };
			SetSenderTest<ObjectNET, nif::NiExtraData>(ObjectNET::EXTRA_DATA, true, std::move(node), set, factory);
		}
	};

	TEST_CLASS(AVObjectTests)
	{
		//Parent connector should receive ISet<NiAVObject> (single)
		TEST_METHOD(Parent)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto node = std::make_unique<Node>(file);
			nif::NiAVObject& obj = node->object();
			SetReceiverTest(AVObject::PARENT, false, std::move(node), obj);
		}
	};

	TEST_CLASS(NodeTests)
	{
	public:

		//Object connector should receive IAssignable<NiNode> (multi)
		TEST_METHOD(Object)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			std::unique_ptr<NodeShared> node = std::make_unique<Node>(file);
			nif::NiNode* obj = &node->object();
			AssignableReceiverTest(NodeShared::OBJECT, true, std::move(node), obj);
		}

		//Children connector should send ISet<NiAVObject> (multi)
		TEST_METHOD(Children)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			std::unique_ptr<NodeShared> node = std::make_unique<Node>(file);
			ISet<nif::NiAVObject>& set = node->object().children();

			auto factory = [&file]() { return file.create<nif::NiNode>(); };
			SetSenderTest<NodeShared, nif::NiAVObject>(NodeShared::CHILDREN, true, std::move(node), set, factory);
		}

		TEST_METHOD(Load_hierarchy) {
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
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
			c.makeRoot(&root->getNative());

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
		}
	};
}
