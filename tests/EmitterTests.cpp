#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"


namespace nif
{
	TEST_CLASS(NiPSysEmitterTests)
	{
	public:
		nif::NiPSysBoxEmitter concrete_obj;
		nif::NiPSysEmitter& obj = concrete_obj;
		std::mt19937 m_engine;

		TEST_METHOD(Colour)
		{
			PropertyTest<nif::ColRGBA>(obj.colour(), m_engine);
		}

		TEST_METHOD(LifeSpan)
		{
			PropertyTest<float>(obj.lifeSpan(), m_engine);
		}

		TEST_METHOD(LifeSpanVar)
		{
			PropertyTest<float>(obj.lifeSpanVar(), m_engine);
		}

		TEST_METHOD(Size)
		{
			PropertyTest<float>(obj.size(), m_engine);
		}

		TEST_METHOD(SizeVar)
		{
			PropertyTest<float>(obj.sizeVar(), m_engine);
		}

		TEST_METHOD(Speed)
		{
			PropertyTest<float>(obj.speed(), m_engine);
		}

		TEST_METHOD(SpeedVar)
		{
			PropertyTest<float>(obj.speedVar(), m_engine);
		}

		TEST_METHOD(Azimuth)
		{
			PropertyTest<float>(obj.azimuth(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(AzimuthVar)
		{
			PropertyTest<float>(obj.azimuthVar(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(Elevation)
		{
			PropertyTest<float>(obj.elevation(), m_engine, 1.0e-4f);
		}

		TEST_METHOD(ElevationVar)
		{
			PropertyTest<float>(obj.elevationVar(), m_engine, 1.0e-4f);
		}
	};

	TEST_CLASS(NiPSysVolumeEmitterTests)
	{
		nif::NiPSysBoxEmitter concrete_obj;
		nif::NiPSysVolumeEmitter& obj = concrete_obj; 

		TEST_METHOD(EmitterObject)
		{
			AssignableTest<nif::NiNode>(obj.emitterObject());
		}
	};

	TEST_CLASS(NiPSysBoxEmitterTests)
	{
		nif::NiPSysBoxEmitter obj;
		std::mt19937 m_engine;

		TEST_METHOD(Width)
		{
			PropertyTest<float>(obj.width(), m_engine);
		}
		TEST_METHOD(Height)
		{
			PropertyTest<float>(obj.height(), m_engine);
		}
		TEST_METHOD(Depth)
		{
			PropertyTest<float>(obj.depth(), m_engine);
		}
	};

	TEST_CLASS(NiPSysCylinderEmitterTests)
	{
		nif::NiPSysCylinderEmitter obj;
		std::mt19937 m_engine;

		TEST_METHOD(Radius)
		{
			PropertyTest<float>(obj.radius(), m_engine);
		}
		TEST_METHOD(Height)
		{
			PropertyTest<float>(obj.height(), m_engine);
		}
	};

	TEST_CLASS(NiPSysSphereEmitterTests)
	{
		nif::NiPSysSphereEmitter obj;
		std::mt19937 m_engine;

		TEST_METHOD(Radius)
		{
			PropertyTest<float>(obj.radius(), m_engine);
		}
	};

	TEST_CLASS(NiPSysEmitterCtlrTests)
	{
		nif::NiPSysEmitterCtlr obj;

		TEST_METHOD(VisibilityInterpolator)
		{
			AssignableTest<nif::NiInterpolator, nif::NiBoolInterpolator>(obj.visIplr());
		}
	};
}

namespace node
{
	TEST_CLASS(EmitterTests)
	{
		std::mt19937 m_engine;

	public:

		//Test insertion of controller
		TEST_METHOD(Controller_single)
		{
			MockModifiable target1;
			MockModifiable target2;
			ConnectorTester<Emitter> tester(std::make_unique<BoxEmitter>());

			tester.tryConnect<void, IModifiable>(Modifier::TARGET, false, &target1);
			tester.tryConnect<void, IModifiable>(Modifier::TARGET, false, &target2);

			Assert::IsTrue(target2.controllers().size() == 1);
			Assert::IsTrue(target2.controllers().find(tester.getNode()->controller()) == 0);

			tester.disconnect(&target2);

			Assert::IsTrue(target2.controllers().size() == 0);
		}

		TEST_METHOD(Controller_sequence)
		{
			//Test that
			//*if a sequence of modifiers receives an interface, they all take action on it in the expected order
			//*if another sequence is appended, they all take action on the interface, but the existing sequence does not
			//*if part of the sequence is disconnected, they receive a disconnect call, but the remaining sequence does not

			MockModifiable target;
			TestRoot root;

			//Make a sequence of modifiers and connect them to an interface
			Emitter* mod1 = root.newChild<BoxEmitter>();
			Emitter* mod2 = root.newChild<BoxEmitter>();

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
			Assert::IsTrue(target.controllers().size() == 2);
			Assert::IsTrue(target.controllers().find(mod1->controller()) == 0);
			Assert::IsTrue(target.controllers().find(mod2->controller()) == 1);
			Assert::IsTrue(mod1->controller().modifierName().get() == "Modifier:0");
			Assert::IsTrue(mod2->controller().modifierName().get() == "Modifier:1");

			//Make sure the controllers hear name changes
			nif::NiPSysAgeDeathModifier adm;
			target.modifiers().insert(0, adm);
			Assert::IsTrue(mod1->controller().modifierName().get() == "Modifier:1");
			Assert::IsTrue(mod2->controller().modifierName().get() == "Modifier:2");

			//Now reverse everything
			target.modifiers().erase(0);
			Assert::IsTrue(mod1->controller().modifierName().get() == "Modifier:0");
			Assert::IsTrue(mod2->controller().modifierName().get() == "Modifier:1");

			//Remove the last two, for good measure
			if (Field* f2 = mod2->getField(Modifier::TARGET))
				if (f2->connector)
					f2->connector->disconnect();
			Assert::IsTrue(target.controllers().size() == 1);
			Assert::IsTrue(target.controllers().find(mod1->controller()) == 0);
			Assert::IsTrue(mod1->controller().modifierName().get() == "Modifier:0");

			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector)
					f1->connector->disconnect();
			Assert::IsTrue(target.controllers().size() == 0);
		}

		//Emitters should send a colour requirement if their colour is non-white
		TEST_METHOD(ColourRequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			MockModifiable target;
			TestRoot root;
			Emitter* mod1 = root.newChild<BoxEmitter>();

			//Set to white before connecting
			mod1->object().colour().set({ 1.0f, 1.0f, 1.0f, 1.0f });

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			//No colour requirement should have been sent
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 0);

			//Set to non-white. Requirement should be sent.
			mod1->object().colour().set(nif::COL_RED);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);

			//Reset same. No additional requirement should be sent.
			mod1->object().colour().set(nif::COL_RED);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);

			//Set other non-white. No additional requirement should be sent.
			mod1->object().colour().set(nif::COL_BLUE);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);

			//Set to white. Requirement should be removed.
			mod1->object().colour().set(nif::COL_WHITE);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 0);

			//Reset to white. Requirement should not be removed again.
			target.requirements().add(Modifier::Requirement::COLOUR);
			mod1->object().colour().set(nif::COL_WHITE);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);
			target.requirements().remove(Modifier::Requirement::COLOUR);

			//Other properties should not require colour
			mod1->object().lifeSpan().set(1.0f);
			mod1->object().lifeSpanVar().set(1.0f);
			mod1->object().size().set(1.0f);
			mod1->object().sizeVar().set(1.0f);
			mod1->object().speed().set(1.0f);
			mod1->object().speedVar().set(1.0f);
			mod1->object().azimuth().set(1.0f);
			mod1->object().azimuthVar().set(1.0f);
			mod1->object().elevation().set(1.0f);
			mod1->object().elevationVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 0);

			//Set to non-white and disconnect. Requirement should be removed.
			mod1->object().colour().set(nif::COL_RED);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);
			if (c1)
				c1->disconnect();
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 0);

			//Connecting with non-white should send requirement
			if (c1 && c2) {
				c1->onClick();
				c2->onRelease();
			}
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::COLOUR) == 1);
			if (c1)
				c1->disconnect();
		}

		//Emitters should send a lifetime requirement if either LifeSpan or LifeSpanVar is non-zero
		TEST_METHOD(LifetimeRequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			MockModifiable target;
			TestRoot root;
			Emitter* mod1 = root.newChild<BoxEmitter>();

			//Set life spans to 0 before connecting
			mod1->object().lifeSpan().set(0.0f);
			mod1->object().lifeSpanVar().set(0.0f);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			//No lifetime requirement should have been sent
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 0);

			//Other properties should not affect us
			mod1->object().colour().set(nif::COL_RED);
			mod1->object().size().set(1.0f);
			mod1->object().sizeVar().set(1.0f);
			mod1->object().speed().set(1.0f);
			mod1->object().speedVar().set(1.0f);
			mod1->object().azimuth().set(1.0f);
			mod1->object().azimuthVar().set(1.0f);
			mod1->object().elevation().set(1.0f);
			mod1->object().elevationVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 0);

			//Set either property to non-zero. Requirement should be sent.
			mod1->object().lifeSpan().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			//Setting other or the same non-zero should not resend requirement.
			mod1->object().lifeSpan().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			mod1->object().lifeSpan().set(2.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			//Set to zero. Requirement should be removed.
			mod1->object().lifeSpan().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 0);
			//Reset to zero. Requirement should not be removed again.
			target.requirements().add(Modifier::Requirement::LIFETIME);
			mod1->object().lifeSpan().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			target.requirements().remove(Modifier::Requirement::LIFETIME);

			//Do the same with the other property
			mod1->object().lifeSpanVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			//Setting other or the same non-zero should not resend requirement.
			mod1->object().lifeSpanVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			mod1->object().lifeSpanVar().set(2.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			//Set to zero. Requirement should be removed.
			mod1->object().lifeSpanVar().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 0);
			//Reset to zero. Requirement should not be removed again.
			target.requirements().add(Modifier::Requirement::LIFETIME);
			mod1->object().lifeSpanVar().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			target.requirements().remove(Modifier::Requirement::LIFETIME);

			//Setting both to non-zero should not send two requirements
			mod1->object().lifeSpan().set(1.0f);
			mod1->object().lifeSpanVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);

			//Setting one to zero when the other is non-zero should not remove the requirement
			mod1->object().lifeSpan().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);

			//Disconnecting should remove the requirement
			if (c1)
				c1->disconnect();
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 0);

			//Connecting with non-zero should send requirement
			if (c1 && c2) {
				c1->onClick();
				c2->onRelease();
			}
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::LIFETIME) == 1);
			if (c1)
				c1->disconnect();
		}

		//Emitters should send a movement requirement if either Speed or SpeedVar is non-zero
		//(this is exactly like the lifetime test)
		TEST_METHOD(MovementRequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			MockModifiable target;
			TestRoot root;
			Emitter* mod1 = root.newChild<BoxEmitter>();

			//Set properties to 0 before connecting
			mod1->object().speed().set(0.0f);
			mod1->object().speedVar().set(0.0f);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			//No requirement should have been sent
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);

			//Other properties should not affect us
			mod1->object().colour().set(nif::COL_RED);
			mod1->object().lifeSpan().set(1.0f);
			mod1->object().lifeSpanVar().set(1.0f);
			mod1->object().size().set(1.0f);
			mod1->object().sizeVar().set(1.0f);
			mod1->object().azimuth().set(1.0f);
			mod1->object().azimuthVar().set(1.0f);
			mod1->object().elevation().set(1.0f);
			mod1->object().elevationVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);

			//Set either property to non-zero. Requirement should be sent.
			mod1->object().speed().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			//Setting other or the same non-zero should not resend requirement.
			mod1->object().speed().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			mod1->object().speed().set(2.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			//Set to zero. Requirement should be removed.
			mod1->object().speed().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);
			//Reset to zero. Requirement should not be removed again.
			target.requirements().add(Modifier::Requirement::MOVEMENT);
			mod1->object().speed().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			target.requirements().remove(Modifier::Requirement::MOVEMENT);

			//Do the same with the other property
			mod1->object().speedVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			//Setting other or the same non-zero should not resend requirement.
			mod1->object().speedVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			mod1->object().speedVar().set(2.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			//Set to zero. Requirement should be removed.
			mod1->object().speedVar().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);
			//Reset to zero. Requirement should not be removed again.
			target.requirements().add(Modifier::Requirement::MOVEMENT);
			mod1->object().speedVar().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			target.requirements().remove(Modifier::Requirement::MOVEMENT);

			//Setting both to non-zero should not send two requirements
			mod1->object().speed().set(1.0f);
			mod1->object().speedVar().set(1.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);

			//Setting one to zero when the other is non-zero should not remove the requirement
			mod1->object().speed().set(0.0f);
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);

			//Disconnecting should remove the requirement
			if (c1)
				c1->disconnect();
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);

			//Connecting with non-zero should send requirement
			if (c1 && c2) {
				c1->onClick();
				c2->onRelease();
			}
			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);
			if (c1)
				c1->disconnect();
		}

		//Birth rate should send IAssignable<NiInterpolator> and receive IController<float> (single)
		TEST_METHOD(BirthRateControllerTest)
		{
			class MockController : public IController<float>
			{
			public:
				virtual LocalProperty<unsigned short>& flags() override { return m_flags; }
				virtual LocalProperty<float>& frequency() override { return m_frequency; }
				virtual LocalProperty<float>& phase() override { return m_phase; }
				virtual LocalProperty<float>& startTime() override { return m_startTime; }
				virtual LocalProperty<float>& stopTime() override { return m_stopTime; }

			private:
				LocalProperty<unsigned short> m_flags;
				LocalProperty<float> m_frequency;
				LocalProperty<float> m_phase;
				LocalProperty<float> m_startTime;
				LocalProperty<float> m_stopTime;
			};

			std::unique_ptr<Emitter> node = std::make_unique<BoxEmitter>();
			nif::NiPSysEmitterCtlr* ctlr = &node->controller();

			MockController target0;
			MockController target;
			ConnectorTester<Emitter> tester(std::move(node));

			tester.tryConnect<IAssignable<nif::NiInterpolator>, IController<float>>(Emitter::BIRTH_RATE, false, &target0);
			auto ifc = tester.tryConnect<IAssignable<nif::NiInterpolator>, IController<float>>(Emitter::BIRTH_RATE, false, &target);
			Assert::IsNotNull(ifc);

			//Setting the properties on target (but not target0) should set the corresponding on ctlr
			std::uniform_int_distribution<unsigned short> I;
			std::uniform_real_distribution<float> F;

			target0.flags().set(I(m_engine));
			target0.frequency().set(F(m_engine));
			target0.phase().set(F(m_engine));
			target0.startTime().set(F(m_engine));
			target0.stopTime().set(F(m_engine));
			Assert::IsFalse(ctlr->flags().get() == target0.flags().get());
			Assert::IsFalse(ctlr->frequency().get() == target0.frequency().get());
			Assert::IsFalse(ctlr->phase().get() == target0.phase().get());
			Assert::IsFalse(ctlr->startTime().get() == target0.startTime().get());
			Assert::IsFalse(ctlr->stopTime().get() == target0.stopTime().get());

			target.flags().set(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			Assert::IsTrue(ctlr->flags().get() == target.flags().get());
			Assert::IsTrue(ctlr->frequency().get() == target.frequency().get());
			Assert::IsTrue(ctlr->phase().get() == target.phase().get());
			Assert::IsTrue(ctlr->startTime().get() == target.startTime().get());
			Assert::IsTrue(ctlr->stopTime().get() == target.stopTime().get());

			//Assigning to the interface should assign to node->controller()->interpolator()
			nif::NiFloatInterpolator iplr;
			ifc->assign(&iplr);
			Assert::IsTrue(ctlr->interpolator().isAssigned(&iplr));
			ifc->assign(nullptr);
			Assert::IsFalse(ctlr->interpolator().isAssigned(&iplr));

			//Make sure listeners are removed
			tester.disconnect<IController<float>>(&target);

			//these calls should not reach the node
			target.flags().set(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			//defaults should have been restored, use a new node to compare with
			std::unique_ptr<Emitter> newNode = std::make_unique<BoxEmitter>();
			Assert::IsTrue(ctlr->flags().get() == newNode->controller().flags().get());
			Assert::IsTrue(ctlr->frequency().get() == newNode->controller().frequency().get());
			Assert::IsTrue(ctlr->phase().get() == newNode->controller().phase().get());
			Assert::IsTrue(ctlr->startTime().get() == newNode->controller().startTime().get());
			Assert::IsTrue(ctlr->stopTime().get() == newNode->controller().stopTime().get());
		}
	};

	TEST_CLASS(VolumeEmitterTests)
	{
		//Test connection and assignment of an emitter object
		TEST_METHOD(EmitterObject)
		{
			std::unique_ptr<VolumeEmitter> node = std::make_unique<BoxEmitter>();
			IAssignable<nif::NiNode>& ass = node->object().emitterObject();
			AssignableSenderTest(VolumeEmitter::EMITTER_OBJECT, false, std::move(node), ass);
		}
	};

	TEST_CLASS(BoxEmitterTests)
	{
		//Test that the nif objects are created and set up right?
	};

	TEST_CLASS(CylinderEmitterTests)
	{
		//Test that the nif objects are created and set up right?
	};

	TEST_CLASS(SphereEmitterTests)
	{
		//Test that the nif objects are created and set up right?
	};
}
