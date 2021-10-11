#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(NiPSysModifierCtlrTests)
	{
		nif::NiPSysEmitterCtlr concrete_obj;
		nif::NiPSysModifierCtlr& obj = concrete_obj;

		TEST_METHOD(ModifierName)
		{
			StringPropertyTest(obj.modifierName());
		}
	};

	TEST_CLASS(NiPSysModifierTests)
	{
		nif::NiPSysRotationModifier concrete_obj;
		nif::NiPSysModifier& obj = concrete_obj;
		std::mt19937 m_engine;

		TEST_METHOD(Name)
		{
			StringPropertyTest(obj.name());
		}

		TEST_METHOD(Order)
		{
			PropertyTest<unsigned int>(obj.order(), m_engine);
		}

		TEST_METHOD(Target)
		{
			AssignableTest<nif::NiParticleSystem>(obj.target());
		}

		TEST_METHOD(Active)
		{
			PropertyTest<bool>(obj.active(), m_engine);
		}
	};

	TEST_CLASS(NiPSysRotationModifierTests)
	{
		nif::NiPSysRotationModifier obj;
		std::mt19937 m_engine;

		TEST_METHOD(Speed)
		{
			PropertyTest<float>(obj.speed(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(SpeedVar)
		{
			PropertyTest<float>(obj.speedVar(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(Angle)
		{
			PropertyTest<float>(obj.angle(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(AngleVar)
		{
			PropertyTest<float>(obj.angleVar(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(RandomSign)
		{
			PropertyTest<bool>(obj.randomSign(), m_engine);
		}
	};

	TEST_CLASS(BSPSysScaleModifierTests)
	{
		nif::BSPSysScaleModifier obj;
		std::mt19937 m_engine;

		TEST_METHOD(Scales)
		{
			std::uniform_int_distribution<int> L(1, 20);
			std::uniform_real_distribution<float> D;
			for (int N = 0; N < 3; N++) {//run 3 tests
				//Populate a vector of random length with random values
				std::vector<float> scales(L(m_engine));
				for (size_t i = 0; i < scales.size(); i++)
					scales[i] = D(m_engine);
				//Set, get back and compare
				obj.scales().set(scales);
				std::vector<float> ret = obj.scales().get();
				Assert::IsTrue(ret == scales);
			}
		}
	};

	TEST_CLASS(BSPSysSimpleColorModifierTests)
	{
		nif::BSPSysSimpleColorModifier obj;
		std::mt19937 m_engine;

		TEST_METHOD(FadeInEnd)
		{
			PropertyTest<float>(obj.alpha2Begin(), m_engine);
		}
		TEST_METHOD(FadeOutBegin)
		{
			PropertyTest<float>(obj.alpha2End(), m_engine);
		}
		TEST_METHOD(Col1End)
		{
			PropertyTest<float>(obj.rgb1End(), m_engine);
		}
		TEST_METHOD(Col2Begin)
		{
			PropertyTest<float>(obj.rgb2Begin(), m_engine);
		}
		TEST_METHOD(Col2End)
		{
			PropertyTest<float>(obj.rgb2End(), m_engine);
		}
		TEST_METHOD(Col3Begin)
		{
			PropertyTest<float>(obj.rgb3Begin(), m_engine);
		}
		TEST_METHOD(Col1)
		{
			PropertyTest<nif::ColRGBA>(obj.col1(), m_engine);
		}
		TEST_METHOD(Col2)
		{
			PropertyTest<nif::ColRGBA>(obj.col2(), m_engine);
		}
		TEST_METHOD(Col3)
		{
			PropertyTest<nif::ColRGBA>(obj.col3(), m_engine);
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

			MockModifiable target1;
			MockModifiable target2;
			ConnectorTester<Modifier> tester(std::make_unique<RotationModifier>());

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
			ConnectorTester<Modifier> tester(std::make_unique<RotationModifier>());

			tester.tryConnect<IModifiable, void>(Modifier::NEXT_MODIFIER, false, nullptr);
			IModifiable* ifc = tester.tryConnect<IModifiable, void>(Modifier::NEXT_MODIFIER, false, nullptr);

			Assert::IsNull(ifc);
		}

		TEST_METHOD(Sequence)
		{
			MockModifiable target;
			TestRoot root;

			//Make a sequence of modifiers and connect them to an interface
			Modifier* mod1 = root.newChild<RotationModifier>();
			Modifier* mod2 = root.newChild<RotationModifier>();

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
			Modifier* mod3 = root.newChild<RotationModifier>();
			Modifier* mod4 = root.newChild<RotationModifier>();
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
			nif::NiPSysAgeDeathModifier adm;
			target.modifiers().insert(0, adm);
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

			MockModifiable target;
			TestRoot root;
			auto mod1 = root.newChild<RotationModifier>();

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

			MockModifiable target;
			TestRoot root;
			auto mod1 = root.newChild<SimpleColourModifier>();

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
