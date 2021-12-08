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
#include "nodes_internal.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(ExtraData)
	{
	public:

		//Target should receive Set<NiExtraData> (single)
		TEST_METHOD(Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiExtraData>();
			SetReceiverTest(std::make_unique<node::DummyExtraData>(obj), *obj, node::ExtraData::TARGET, false);
		}
	};

	TEST_CLASS(AttachPointData)
	{
	public:

		TEST_METHOD(PreWriteProcessor)
		{
			{
				//If there is AttachPointData on any non-root node, 
				//a "MultiTechnique" data should be attached to the root.
				File file{ File::Version::SKYRIM_SE };
				auto root = file.getRoot();
				auto node1 = file.create<NiNode>();
				auto node2 = file.create<NiNode>();
				root->children.add(node1);
				root->children.add(node2);

				auto data1 = file.create<NiStringsExtraData>();
				data1->name.set("AttachT");
				data1->strings.resize(1);
				data1->strings.at(0).set("NamedNode&Bone1");
				node1->extraData.add(data1);

				auto data2 = file.create<NiStringsExtraData>();
				data2->name.set("AttachT");
				data2->strings.resize(1);
				data2->strings.at(0).set("NamedNode&Bone2");
				node2->extraData.add(data2);

				node::AttachPointData::PreWriteProcessor t(file);
				root->receive(t);

				Assert::IsTrue(root->extraData.size() == 1);
				for (auto&& data : root->extraData) {
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.size() == 1);
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.at(0).get() == "MultiTechnique");
				}
			}
			{
				//If the root has AttachPointData, no additional data should be added
				File file{ File::Version::SKYRIM_SE };
				auto root = file.getRoot();

				auto data = file.create<NiStringsExtraData>();
				data->name.set("AttachT");
				data->strings.resize(1);
				data->strings.at(0).set("NamedNode&Bone1");
				root->extraData.add(data);

				node::AttachPointData::PreWriteProcessor t(file);
				root->receive(t);

				Assert::IsTrue(root->extraData.size() == 1);
				for (auto&& data : root->extraData) {
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.size() == 1);
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.at(0).get() == "NamedNode&Bone1");
				}
			}
			{
				//If the root has a "MultiTechnique" data but no non-root attach points,
				//this should be removed.
				File file{ File::Version::SKYRIM_SE };
				auto root = file.getRoot();

				auto data1 = file.create<NiStringsExtraData>();
				data1->name.set("AttachT");
				data1->strings.resize(1);
				data1->strings.at(0).set("MultiTechnique");
				root->extraData.add(data1);

				auto data2 = file.create<NiStringsExtraData>();
				data2->name.set("AttachT");
				data2->strings.resize(1);
				data2->strings.at(0).set("NamedNode&Bone1");
				root->extraData.add(data2);

				node::AttachPointData::PreWriteProcessor t(file);
				root->receive(t);

				Assert::IsTrue(root->extraData.size() == 1);
				for (auto&& data : root->extraData) {
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.size() == 1);
					Assert::IsTrue(std::static_pointer_cast<NiStringsExtraData>(data)->strings.at(0).get() == "NamedNode&Bone1");
				}
			}
		}
	};
}
