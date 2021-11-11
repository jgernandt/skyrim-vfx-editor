#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes.h"

#include "Constructor.h"
#include "nif_backend.h"

namespace nif
{
	TEST_CLASS(NiPSysModifierCtlrTests)
	{
		TEST_METHOD(ModifierName)
		{
			File file{ File::Version::SKYRIM_SE };
			std::shared_ptr<NiPSysModifierCtlr> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			StringPropertyTest(obj->modifierName());
		}
	};

	TEST_CLASS(NiPSysModifierTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Name)
		{
			std::shared_ptr<NiPSysModifier> obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			StringPropertyTest(obj->name());
		}

		TEST_METHOD(Order)
		{
			std::shared_ptr<NiPSysModifier> obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<unsigned int>(obj->order(), m_engine);
		}

		TEST_METHOD(Target)
		{
			std::shared_ptr<NiPSysModifier> obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiParticleSystem>(); };
			AssignableTest<nif::NiParticleSystem>(obj->target(), factory);
		}

		TEST_METHOD(Active)
		{
			std::shared_ptr<NiPSysModifier> obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->active(), m_engine);
		}
	};

	TEST_CLASS(NiPSysRotationModifierTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Speed)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->speed(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(SpeedVar)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->speedVar(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(Angle)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->angle(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(AngleVar)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->angleVar(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(RandomSign)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->randomSign(), m_engine);
		}
	};

	TEST_CLASS(BSPSysScaleModifierTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Scales)
		{
			auto obj = file.create<BSPSysScaleModifier>();
			Assert::IsNotNull(obj.get());

			std::uniform_int_distribution<int> L(1, 20);
			std::uniform_real_distribution<float> D;
			for (int N = 0; N < 3; N++) {//run 3 tests
				//Populate a vector of random length with random values
				std::vector<float> scales(L(m_engine));
				for (size_t i = 0; i < scales.size(); i++)
					scales[i] = D(m_engine);
				//Set, get back and compare
				obj->scales().set(scales);
				std::vector<float> ret = obj->scales().get();
				Assert::IsTrue(ret == scales);
			}
		}
	};

	TEST_CLASS(BSPSysSimpleColorModifierTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(FadeInEnd)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->alpha2Begin(), m_engine);
		}
		TEST_METHOD(FadeOutBegin)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->alpha2End(), m_engine);
		}
		TEST_METHOD(Col1End)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->rgb1End(), m_engine);
		}
		TEST_METHOD(Col2Begin)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->rgb2Begin(), m_engine);
		}
		TEST_METHOD(Col2End)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->rgb2End(), m_engine);
		}
		TEST_METHOD(Col3Begin)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->rgb3Begin(), m_engine);
		}
		TEST_METHOD(Col1)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::ColRGBA>(obj->col1(), m_engine);
		}
		TEST_METHOD(Col2)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::ColRGBA>(obj->col2(), m_engine);
		}
		TEST_METHOD(Col3)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::ColRGBA>(obj->col3(), m_engine);
		}
	};
}

namespace node
{
	TEST_CLASS(ModifierTests)
	{
		TEST_METHOD(Target)
		{
			//Test that
			//*we can connect to a sender of IModifiable
			//*we take the expected action on an IModifiable interface

			nif::File file{ nif::File::Version::SKYRIM_SE };

			MockModifiable target1;
			MockModifiable target2;
			ConnectorTester<Modifier> tester(std::make_unique<RotationModifier>(file));

			tester.tryConnect<void, IModifiable>(Modifier::TARGET, false, &target1);
			tester.tryConnect<void, IModifiable>(Modifier::TARGET, false, &target2);

			//Regardless of the type of modifier, it should be inserted
			Assert::IsTrue(target2.modifiers().size() == 1);
			Assert::IsTrue(target2.modifiers().find(tester.getNode()->object()) != -1);

			//If it has a controller, that should also be inserted (test separately)
			/*if (tester.getNode()->controller()) {
				Assert::IsTrue(target2.controllers().size() == 1);
				Assert::IsTrue(target2.controllers().find(*tester.getNode()->controller()) != -1);
			}*/

			tester.disconnect(&target2);

			Assert::IsTrue(target2.modifiers().size() == 0);
		}

		TEST_METHOD(NextModifier)
		{
			//Test that
			//*we can connect to a receiver of IModifiable
			//*we expose no interface to them

			nif::File file{ nif::File::Version::SKYRIM_SE };

			ConnectorTester<Modifier> tester(std::make_unique<RotationModifier>(file));

			tester.tryConnect<IModifiable, void>(Modifier::NEXT_MODIFIER, false, nullptr);
			IModifiable* ifc = tester.tryConnect<IModifiable, void>(Modifier::NEXT_MODIFIER, false, nullptr);

			Assert::IsNull(ifc);
		}

		TEST_METHOD(Sequence)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			MockModifiable target;
			TestRoot root;

			//Make a sequence of modifiers and connect them to an interface
			Modifier* mod1 = root.newChild<RotationModifier>(file);
			Modifier* mod2 = root.newChild<RotationModifier>(file);

			if (Field* f1 = mod1->getField(Modifier::NEXT_MODIFIER))
				if (Field* f2 = mod2->getField(Modifier::TARGET))
					if (f1->connector && f2->connector) {
						f1->connector->onClick();
						f2->connector->onRelease();
					}

			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					auto c2 = root.newChild<TestConnector>(target);
					f1->connector->onClick();
					c2->onRelease();
				}

			//Assert mods were added in the corret order
			Assert::IsTrue(target.modifiers().size() == 2);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 0);
			Assert::IsTrue(target.modifiers().find(mod2->object()) == 1);
			Assert::IsTrue(mod1->object().order().get() == 0);
			Assert::IsTrue(mod2->object().order().get() == 1);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:0");
			Assert::IsTrue(mod2->object().name().get() == "Modifier:1");

			//Make another sequence
			Modifier* mod3 = root.newChild<RotationModifier>(file);
			Modifier* mod4 = root.newChild<RotationModifier>(file);
			if (Field* f3 = mod3->getField(Modifier::NEXT_MODIFIER))
				if (Field* f4 = mod4->getField(Modifier::TARGET))
					if (f3->connector && f4->connector) {
						f3->connector->onClick();
						f4->connector->onRelease();
					}
			//Append it to the first
			if (Field* f2 = mod2->getField(Modifier::NEXT_MODIFIER))
				if (Field* f3 = mod3->getField(Modifier::TARGET))
					if (f2->connector && f3->connector) {
						f2->connector->onClick();
						f3->connector->onRelease();
					}
			//Assert their mods were added and the existing sequence was unchanged (and not duplicated)
			Assert::IsTrue(target.modifiers().size() == 4);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 0);
			Assert::IsTrue(target.modifiers().find(mod2->object()) == 1);
			Assert::IsTrue(target.modifiers().find(mod3->object()) == 2);
			Assert::IsTrue(target.modifiers().find(mod4->object()) == 3);
			Assert::IsTrue(mod1->object().order().get() == 0);
			Assert::IsTrue(mod2->object().order().get() == 1);
			Assert::IsTrue(mod3->object().order().get() == 2);
			Assert::IsTrue(mod4->object().order().get() == 3);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:0");
			Assert::IsTrue(mod2->object().name().get() == "Modifier:1");
			Assert::IsTrue(mod3->object().name().get() == "Modifier:2");
			Assert::IsTrue(mod4->object().name().get() == "Modifier:3");

			//If a mod is added to the beginning (for whatever reason), the others should update name and order
			auto adm = file.create<nif::NiPSysAgeDeathModifier>();
			Assert::IsNotNull(adm.get());
			target.modifiers().insert(0, *adm);
			Assert::IsTrue(target.modifiers().size() == 5);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 1);
			Assert::IsTrue(target.modifiers().find(mod2->object()) == 2);
			Assert::IsTrue(target.modifiers().find(mod3->object()) == 3);
			Assert::IsTrue(target.modifiers().find(mod4->object()) == 4);
			Assert::IsTrue(mod1->object().order().get() == 1);
			Assert::IsTrue(mod2->object().order().get() == 2);
			Assert::IsTrue(mod3->object().order().get() == 3);
			Assert::IsTrue(mod4->object().order().get() == 4);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:1");
			Assert::IsTrue(mod2->object().name().get() == "Modifier:2");
			Assert::IsTrue(mod3->object().name().get() == "Modifier:3");
			Assert::IsTrue(mod4->object().name().get() == "Modifier:4");

			//Now reverse everything
			target.modifiers().erase(0);
			Assert::IsTrue(target.modifiers().size() == 4);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 0);
			Assert::IsTrue(target.modifiers().find(mod2->object()) == 1);
			Assert::IsTrue(target.modifiers().find(mod3->object()) == 2);
			Assert::IsTrue(target.modifiers().find(mod4->object()) == 3);
			Assert::IsTrue(mod1->object().order().get() == 0);
			Assert::IsTrue(mod2->object().order().get() == 1);
			Assert::IsTrue(mod3->object().order().get() == 2);
			Assert::IsTrue(mod4->object().order().get() == 3);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:0");
			Assert::IsTrue(mod2->object().name().get() == "Modifier:1");
			Assert::IsTrue(mod3->object().name().get() == "Modifier:2");
			Assert::IsTrue(mod4->object().name().get() == "Modifier:3");

			//Now remove a part of the sequence
			if (Field* f3 = mod3->getField(Modifier::TARGET))
				if (f3->connector)
					f3->connector->disconnect();
			//And make sure they were removed (but the remaining sequence was not)
			Assert::IsTrue(target.modifiers().size() == 2);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 0);
			Assert::IsTrue(target.modifiers().find(mod2->object()) == 1);
			Assert::IsTrue(mod1->object().order().get() == 0);
			Assert::IsTrue(mod2->object().order().get() == 1);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:0");
			Assert::IsTrue(mod2->object().name().get() == "Modifier:1");

			//Remove the last two, for good measure
			if (Field* f2 = mod2->getField(Modifier::TARGET))
				if (f2->connector)
					f2->connector->disconnect();
			Assert::IsTrue(target.modifiers().size() == 1);
			Assert::IsTrue(target.modifiers().find(mod1->object()) == 0);
			Assert::IsTrue(mod1->object().order().get() == 0);
			Assert::IsTrue(mod1->object().name().get() == "Modifier:0");

			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector)
					f1->connector->disconnect();
			Assert::IsTrue(target.modifiers().size() == 0);
		}

		TEST_METHOD(Load)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
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
			c.makeRoot(&root->getNative());

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

	};

	TEST_CLASS(RotationModifierTests)
	{
		//We should send a constant rotations requirement
		TEST_METHOD(RequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			nif::File file{ nif::File::Version::SKYRIM_SE };

			MockModifiable target;
			TestRoot root;
			auto mod1 = root.newChild<RotationModifier>(file);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::ROTATION) == 1);

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::ROTATION) == 0);
		}
	};

	TEST_CLASS(SimpleColourModifierTests)
	{
		//We should send a constant colour requirement
		TEST_METHOD(RequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			nif::File file{ nif::File::Version::SKYRIM_SE };

			MockModifiable target;
			TestRoot root;
			auto mod1 = root.newChild<SimpleColourModifier>(file);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 0);
		}
	};
}
