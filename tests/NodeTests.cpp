#include "pch.h"
#include "CppUnitTest.h"
#include "nodes.h"
#include "CommonTests.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(ObjectNET)
	{
	public:

		TEST_METHOD(Extra_data)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<node::Node>(obj), obj->extraData, node::ObjectNET::EXTRA_DATA, true);
		}
	};

	TEST_CLASS(AVObject)
	{
	public:

		TEST_METHOD(Parent)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiAVObject>();
			SetReceiverTest(std::make_unique<node::DummyAVObject>(obj), *obj, node::AVObject::PARENT, false);
		}
	};

	TEST_CLASS(Node)
	{
	public:

		TEST_METHOD(References)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			AssignableReceiverTest(std::make_unique<node::Node>(obj), *obj, node::Node::OBJECT, true);
		}

		TEST_METHOD(Children)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<node::Node>(obj), obj->children, node::Node::CHILDREN, true);
		}
	};
}
