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
#include "nodes_internal.h"
#include "CommonTests.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(ObjectNET)
	{
	public:

		TEST_METHOD(Connector_Extra_data)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<node::Node>(obj), obj->extraData, node::ObjectNET::EXTRA_DATA, true);
		}
	};

	TEST_CLASS(AVObject)
	{
	public:

		TEST_METHOD(Connector_Parent)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiAVObject>();
			SetReceiverTest(std::make_unique<node::DummyAVObject>(obj), *obj, node::AVObject::PARENT, false);
		}
	};

	TEST_CLASS(Node)
	{
	public:

		TEST_METHOD(Connector_References)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			AssignableReceiverTest<Ptr>(std::make_unique<node::Node>(obj), *obj, node::Node::OBJECT, true);
		}

		TEST_METHOD(Connector_Children)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiNode>();
			SetSenderTest(std::make_unique<node::Node>(obj), obj->children, node::Node::CHILDREN, true);
		}
	};
}
