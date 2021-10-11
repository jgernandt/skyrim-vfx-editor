#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(ObjectNETTests)
	{
	public:
		nif::NiNode concrete_obj;//no default implementation for NiObjectNET
		nif::NiObjectNET& obj = concrete_obj;

		TEST_METHOD(Name)
		{
			StringPropertyTest(obj.name());
		}

		TEST_METHOD(ExtraData)
		{
			SetTest<nif::NiExtraData, nif::NiStringExtraData>(obj.extraData());
		}

		TEST_METHOD(Controllers)
		{
			SequenceTest<nif::NiTimeController, nif::NiPSysEmitterCtlr>(obj.controllers());
		}
	};

	TEST_CLASS(AVObjectTests)
	{
	public:
		nif::NiNode concrete_obj;//no default implementation for NiObjectNET
		nif::NiAVObject& obj = concrete_obj;
		std::mt19937 m_engine;

		TEST_METHOD(Transform)
		{
			PropertyTest<nif::translation_t>(obj.transform().translation(), m_engine);

			//Umm, what are we testing here, exactly? Isn't this stupid?
			struct CustomProperty : PropertyBase<math::Rotation::EulerAngles>
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
						notify(val);
					}
				}

				IProperty<nif::rotation_t>& prop;
				math::EulerOrder order{ math::EulerOrder::XYZ };

			} custom(obj.transform().rotation());

			PropertyTest<math::Rotation::euler_type>(static_cast<IProperty<math::Rotation::EulerAngles>&>(custom), m_engine, 1.0e-3f);

			PropertyTest<float>(obj.transform().scale(), m_engine);
		}

	};

	TEST_CLASS(NodeTests)
	{
	public:

		TEST_METHOD(Parent)
		{
			nif::NiNode node;
			nif::NiNode child;

			SetTest<nif::NiAVObject, nif::NiNode>(node.children());
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
			std::unique_ptr<ObjectNET> node = std::make_unique<Root>();
			ISet<nif::NiExtraData>& set = node->object().extraData();
			SetSenderTest<ObjectNET, nif::NiExtraData, nif::NiStringExtraData>(ObjectNET::EXTRA_DATA, true, std::move(node), set);
		}
	};

	TEST_CLASS(AVObjectTests)
	{
		//Parent connector should receive ISet<NiAVObject> (single)
		TEST_METHOD(Parent)
		{
			auto node = std::make_unique<Node>();
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
			std::unique_ptr<NodeShared> node = std::make_unique<Node>();
			nif::NiNode* obj = &node->object();
			AssignableReceiverTest(NodeShared::OBJECT, true, std::move(node), obj);
		}

		//Children connector should send ISet<NiAVObject> (multi)
		TEST_METHOD(Children)
		{
			std::unique_ptr<NodeShared> node = std::make_unique<Node>();
			ISet<nif::NiAVObject>& set = node->object().children();
			SetSenderTest<NodeShared, nif::NiAVObject, nif::NiNode>(NodeShared::CHILDREN, true, std::move(node), set);
		}
	};
}
