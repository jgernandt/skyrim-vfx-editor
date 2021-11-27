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
#include "ModifiersTests.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(Modifier)
	{
	public:

		//Test that modifiers change their name to match their order
		TEST_METHOD(ModifierName)
		{
			File file{ File::Version::SKYRIM_SE };

			auto obj0 = file.create<NiPSysBoxEmitter>();
			auto node = node::Default<node::BoxEmitter>{}.create(file, obj0);

			obj0->order.set(3);
			Assert::IsTrue(obj0->name.get() == "Modifier:3");
		}

		//Test that modifier ctlrs receive name changes from their modifier
		TEST_METHOD(ControllerName)
		{
			File file{ File::Version::SKYRIM_SE };

			auto obj0 = file.create<NiPSysBoxEmitter>();
			auto ctlr0 = file.create<NiPSysEmitterCtlr>();
			auto node0 = node::Default<node::BoxEmitter>{}.create(file, obj0, ctlr0);

			obj0->name.set("wabnoearnb");
			Assert::IsTrue(ctlr0->modifierName.get() == obj0->name.get());

			auto obj1 = file.create<NiPSysModifier>();
			auto ctlr1 = file.create<NiPSysModifierCtlr>();
			auto node1 = node::Default<node::DummyModifier>{}.create(file, obj1);
			node1->addController(ctlr1);

			obj1->name.set("awbvaierubn");
			Assert::IsTrue(ctlr1->modifierName.get() == obj1->name.get());
		}

		TEST_METHOD(Connector_Target)
		{
			//Test that
			//*we can connect to a sender of IModifiable
			//*we take the expected action on an IModifiable interface

			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysModifier>();
			auto ctlr = file.create<NiPSysModifierCtlr>();

			MockModifiable target1;
			MockModifiable target2;
			ConnectorTester<node::Modifier> tester(node::Default<node::DummyModifier>{}.create(file, obj));
			tester.getNode()->addController(ctlr);

			tester.tryConnect<void, node::IModifiable>(node::Modifier::TARGET, false, &target1);
			tester.tryConnect<void, node::IModifiable>(node::Modifier::TARGET, false, &target2);

			//Test mod addition
			Assert::IsTrue(target1.modsAdded.size() == target1.modsRemoved.size());
			Assert::IsTrue(target2.modsAdded.size() == 1);
			Assert::IsTrue(target2.modsAdded.front() == obj.get());
			target2.modsAdded.clear();
			Assert::IsTrue(target2.modsRemoved.empty());

			//Test ctlr addition
			Assert::IsTrue(target1.ctlrsAdded.size() == target1.ctlrsRemoved.size());
			Assert::IsTrue(target2.ctlrsAdded.size() == 1);
			Assert::IsTrue(target2.ctlrsAdded.front() == ctlr.get());
			target2.ctlrsAdded.clear();
			Assert::IsTrue(target2.ctlrsRemoved.empty());

			tester.disconnect(&target2);

			//Test mod removal
			Assert::IsTrue(target2.modsAdded.empty());
			Assert::IsTrue(target2.modsRemoved.size() == 1);
			Assert::IsTrue(target2.modsRemoved.front() == obj.get());

			//Test ctlr removal
			Assert::IsTrue(target2.ctlrsAdded.empty());
			Assert::IsTrue(target2.ctlrsRemoved.size() == 1);
			Assert::IsTrue(target2.ctlrsRemoved.front() == ctlr.get());
		}

		TEST_METHOD(Connector_NextModifier)
		{
			//Test that
			//*we can connect to a receiver of IModifiable
			//*we expose no interface to them

			File file{ File::Version::SKYRIM_SE };
			ConnectorTester<node::Modifier> tester(node::Default<node::DummyModifier>{}.create(file));

			tester.tryConnect<node::IModifiable, void>(node::Modifier::NEXT_MODIFIER, false, nullptr);
			auto ifc = tester.tryConnect<node::IModifiable, void>(node::Modifier::NEXT_MODIFIER, false, nullptr);

			Assert::IsNull(ifc);
		}

		TEST_METHOD(Sequence)
		{
			File file{ File::Version::SKYRIM_SE };
			MockModifiable target;
			TestRoot root;

			//Make a sequence of modifiers and connect them to an interface

			auto obj0 = file.create<NiPSysModifier>();
			auto ctlr0 = file.create<NiPSysModifierCtlr>();
			node::Modifier* node0 = root.newChild<node::DummyModifier>(obj0);
			node0->addController(ctlr0);

			auto obj1 = file.create<NiPSysModifier>();
			auto ctlr1 = file.create<NiPSysModifierCtlr>();
			node::Modifier* node1 = root.newChild<node::DummyModifier>(obj1);
			node1->addController(ctlr1);


			if (node::Field* f0 = node0->getField(node::Modifier::NEXT_MODIFIER)) {
				if (node::Field* f1 = node1->getField(node::Modifier::TARGET))
					if (f0->connector && f1->connector) {
						f0->connector->onClick();
						f1->connector->onRelease();
					}
			}
			if (node::Field* f0 = node0->getField(node::Modifier::TARGET)) {
				if (f0->connector) {
					auto c2 = root.newChild<TestConnector>(target);
					f0->connector->onClick();
					c2->onRelease();
				}
			}

			//Assert mods and ctlrs were added in the correct order
			Assert::IsTrue(target.modsAdded.size() == 2);
			Assert::IsTrue(target.modsAdded[0] == obj0.get());
			Assert::IsTrue(target.modsAdded[1] == obj1.get());
			target.modsAdded.clear();
			Assert::IsTrue(target.modsRemoved.empty());

			Assert::IsTrue(target.ctlrsAdded.size() == 2);
			Assert::IsTrue(target.ctlrsAdded[0] == ctlr0.get());
			Assert::IsTrue(target.ctlrsAdded[1] == ctlr1.get());
			target.ctlrsAdded.clear();
			Assert::IsTrue(target.ctlrsRemoved.empty());


			//Make another sequence

			auto obj2 = file.create<NiPSysModifier>();
			auto ctlr2 = file.create<NiPSysModifierCtlr>();
			node::Modifier* node2 = root.newChild<node::DummyModifier>(obj2);
			node2->addController(ctlr2);

			auto obj3 = file.create<NiPSysModifier>();
			auto ctlr3 = file.create<NiPSysModifierCtlr>();
			node::Modifier* node3 = root.newChild<node::DummyModifier>(obj3);
			node3->addController(ctlr3);

			if (node::Field* f2 = node2->getField(node::Modifier::NEXT_MODIFIER))
				if (node::Field* f3 = node3->getField(node::Modifier::TARGET))
					if (f2->connector && f3->connector) {
						f2->connector->onClick();
						f3->connector->onRelease();
					}

			//Append it to the first
			if (node::Field* f1 = node1->getField(node::Modifier::NEXT_MODIFIER))
				if (node::Field* f2 = node2->getField(node::Modifier::TARGET))
					if (f1->connector && f2->connector) {
						f1->connector->onClick();
						f2->connector->onRelease();
					}

			//Assert that only the new mods were added, in order
			Assert::IsTrue(target.modsAdded.size() == 2);
			Assert::IsTrue(target.modsAdded[0] == obj2.get());
			Assert::IsTrue(target.modsAdded[1] == obj3.get());
			target.modsAdded.clear();
			Assert::IsTrue(target.modsRemoved.empty());

			Assert::IsTrue(target.ctlrsAdded.size() == 2);
			Assert::IsTrue(target.ctlrsAdded[0] == ctlr2.get());
			Assert::IsTrue(target.ctlrsAdded[1] == ctlr3.get());
			target.ctlrsAdded.clear();
			Assert::IsTrue(target.ctlrsRemoved.empty());

			//Disconnect the last two 
			if (node::Field* f2 = node2->getField(node::Modifier::TARGET))
				if (f2->connector)
					f2->connector->disconnect();

			//They should have been removed, last to first
			Assert::IsTrue(target.modsAdded.empty());
			Assert::IsTrue(target.modsRemoved.size() == 2);
			Assert::IsTrue(target.modsRemoved[0] == obj3.get());
			Assert::IsTrue(target.modsRemoved[1] == obj2.get());
			target.modsRemoved.clear();

			Assert::IsTrue(target.ctlrsAdded.empty());
			Assert::IsTrue(target.ctlrsRemoved.size() == 2);
			Assert::IsTrue(target.ctlrsRemoved[0] == ctlr3.get());
			Assert::IsTrue(target.ctlrsRemoved[1] == ctlr2.get());
			target.ctlrsRemoved.clear();

			//Remove the remaining two, for good measure
			if (node::Field* f0 = node0->getField(node::Modifier::TARGET))
				if (f0->connector)
					f0->connector->disconnect();

			Assert::IsTrue(target.modsAdded.empty());
			Assert::IsTrue(target.modsRemoved.size() == 2);
			Assert::IsTrue(target.modsRemoved[0] == obj1.get());
			Assert::IsTrue(target.modsRemoved[1] == obj0.get());
			target.modsRemoved.clear();

			Assert::IsTrue(target.ctlrsAdded.empty());
			Assert::IsTrue(target.ctlrsRemoved.size() == 2);
			Assert::IsTrue(target.ctlrsRemoved[0] == ctlr1.get());
			Assert::IsTrue(target.ctlrsRemoved[1] == ctlr0.get());
			target.ctlrsRemoved.clear();
		}
		/*
		* This should be divided into one test of modifier creation registering the correct ConnectionInfo,
		* and one test of Constructor resolving connections correctly
		TEST_METHOD(Load)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			auto mod0 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod0.get());
			psys->modifiers().insert(-1, *mod0);

			auto mod1 = file.create<nif::NiPSysGravityModifier>();
			Assert::IsNotNull(mod1.get());
			mod1->forceType().set(nif::FORCE_PLANAR);
			mod1->gravityObject().assign(root.get());
			psys->modifiers().insert(-1, *mod1);

			auto mod2 = file.create<nif::NiPSysGravityModifier>();
			Assert::IsNotNull(mod2.get());
			mod2->forceType().set(nif::FORCE_SPHERICAL);
			psys->modifiers().insert(-1, *mod2);

			auto mod3 = file.create<nif::BSPSysSimpleColorModifier>();
			Assert::IsNotNull(mod3.get());
			psys->modifiers().insert(-1, *mod3);

			auto mod4 = file.create<nif::BSPSysScaleModifier>();
			Assert::IsNotNull(mod4.get());
			psys->modifiers().insert(-1, *mod4);

			Constructor c(file);
			c.makeRoot();

			Assert::IsTrue(c.size() == 7);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), *psys);
			RotationModifier* mod0_node = findNode<RotationModifier>(c.nodes(), *mod0);
			PlanarForceField* mod1_node = findNode<PlanarForceField>(c.nodes(), *mod1);
			SphericalForceField* mod2_node = findNode<SphericalForceField>(c.nodes(), *mod2);
			SimpleColourModifier* mod3_node = findNode<SimpleColourModifier>(c.nodes(), *mod3);
			ScaleModifier* mod4_node = findNode<ScaleModifier>(c.nodes(), *mod4);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);
			Assert::IsNotNull(mod0_node);
			Assert::IsNotNull(mod1_node);
			Assert::IsNotNull(mod2_node);
			Assert::IsNotNull(mod3_node);
			Assert::IsNotNull(mod4_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(psys_node->getField(ParticleSystem::MODIFIERS)->connector, mod0_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod0_node->getField(Modifier::NEXT_MODIFIER)->connector, mod1_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod1_node->getField(Modifier::NEXT_MODIFIER)->connector, mod2_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod2_node->getField(Modifier::NEXT_MODIFIER)->connector, mod3_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod3_node->getField(Modifier::NEXT_MODIFIER)->connector, mod4_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod1_node->getField(GravityModifier::GRAVITY_OBJECT)->connector, root_node->getField(Node::OBJECT)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 8);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod0->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod1->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[3] == &mod2->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[4] == &mod3->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[5] == &mod4->getNative());

			Assert::IsTrue(mod1->getNative().GetGravityObject() == &root->getNative());
		}
		*/
	};

	TEST_CLASS(GravityModifierTests)
	{
	public:

		//Gravity object should send Assignable<NiNode>, single
		TEST_METHOD(GravityObject)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysGravityModifier>();
			AssignableSenderTest(node::Default<node::PlanarForceField>{}.create(file, obj), 
				obj->gravityObject, node::GravityModifier::GRAVITY_OBJECT, false);
		}
	};

	TEST_CLASS(RotationModifier)
	{
	public:

		//We should send a constant rotations requirement
		TEST_METHOD(RequirementTest)
		{
			RequirementsTest<node::RotationModifier, node::ModRequirement::ROTATION>{}.run();
		}
	};

	TEST_CLASS(SimpleColourModifierTests)
	{
	public:

		//We should send a constant colour requirement
		TEST_METHOD(RequirementTest)
		{
			RequirementsTest<node::SimpleColourModifier, node::ModRequirement::COLOUR>{}.run();
		}
	};
}
