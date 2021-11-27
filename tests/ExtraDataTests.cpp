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

		//Target should receive Set<NiExtraData> (multi)
		TEST_METHOD(Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiExtraData>();
			SetReceiverTest(std::make_unique<node::DummyExtraData>(obj), *obj, node::ExtraData::TARGET, true);
		}
	};

	/* Should be a specialisation of the creation test
	* 
	TEST_CLASS(StringExtraDataTests)
	{
	public:
		TEST_METHOD(Loading)
		{
			//partially dependent on Nodes test

			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
			Assert::IsNotNull(root.get());

			auto s1 = file.create<nif::NiStringExtraData>();
			Assert::IsNotNull(s1.get());
			root->extraData().add(*s1);

			auto n1 = file.create<nif::NiNode>();
			Assert::IsNotNull(n1.get());
			root->children().add(*n1);

			auto s2 = file.create<nif::NiStringExtraData>();
			Assert::IsNotNull(s2.get());
			n1->extraData().add(*s2);

			//Multitarget extra data should be allowed (but not necessarily meaningful)
			auto s3 = file.create<nif::NiStringExtraData>();
			Assert::IsNotNull(s3.get());
			root->extraData().add(*s3);
			n1->extraData().add(*s3);

			//Special types
			std::shared_ptr<nif::NiStringExtraData> weaponTypes[7];
			for (auto&& ptr : weaponTypes)
				ptr = file.create<nif::NiStringExtraData>();
			std::string types[7]{
				"WeaponAxe",
				"WeaponDagger",
				"WeaponMace",
				"WeaponSword",
				"WeaponBack",
				"WeaponBow",
				"SHIELD" };
			for (int i = 0; i < 7; i++) {
				weaponTypes[i]->name().set("Prn");
				weaponTypes[i]->value().set(types[i]);
				root->extraData().add(*weaponTypes[i]);
			}

			Constructor c(file);
			c.makeRoot();

			Assert::IsTrue(c.size() == 12);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			Node* n1_node = findNode<Node>(c.nodes(), *n1);
			StringData* s1_node = findNode<StringData>(c.nodes(), *s1);
			StringData* s2_node = findNode<StringData>(c.nodes(), *s2);
			StringData* s3_node = findNode<StringData>(c.nodes(), *s3);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(n1_node);
			Assert::IsNotNull(s1_node);
			Assert::IsNotNull(s2_node);
			Assert::IsNotNull(s3_node);

			WeaponTypeData* wtype_nodes[7];
			for (int i = 0; i < 7; i++) {
				wtype_nodes[i] = findNode<WeaponTypeData>(c.nodes(), *weaponTypes[i]);
				Assert::IsNotNull(wtype_nodes[i]);
			}

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, s1_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(n1_node->getField(Node::EXTRA_DATA)->connector, s2_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, s3_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(n1_node->getField(Node::EXTRA_DATA)->connector, s3_node->getField(ExtraData::TARGET)->connector));

			for (int i = 0; i < 7; i++)
				Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, wtype_nodes[i]->getField(ExtraData::TARGET)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(root->getNative().GetExtraData().size() == 9);
			Assert::IsTrue(n1->getNative().GetExtraData().size() == 2);
		}
	};
	*/
}
