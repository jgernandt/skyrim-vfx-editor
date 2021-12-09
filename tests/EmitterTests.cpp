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

	TEST_CLASS(Emitter)
	{
		std::mt19937 m_engine;

	public:

		//Emitters should send a colour requirement if their colour is non-white
		TEST_METHOD(ColourRequirement)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysBoxEmitter>();
			MockModifiable target;
			TestRoot root;
			auto node = node::Default<node::BoxEmitter>{}.create(file, obj);
			auto mod1 = node.get();
			root.addChild(std::move(node));

			//Set to white before connecting
			obj->colour.set(COL_WHITE);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (node::Field* f1 = mod1->getField(node::Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			//No colour requirement should have been sent
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to non-white. Requirement should be sent.
			obj->colour.set(COL_RED);
			Assert::IsTrue(target.reqsAdded.size() == 1 && target.reqsAdded[0] == node::ModRequirement::COLOUR);
			target.reqsAdded.clear();
			Assert::IsTrue(target.reqsRemoved.empty());

			//Reset same. No additional requirement should be sent.
			obj->colour.set(COL_RED);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set other non-white. No additional requirement should be sent.
			obj->colour.set(COL_BLUE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to white. Requirement should be removed.
			obj->colour.set(COL_WHITE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.size() == 1 && target.reqsRemoved[0] == node::ModRequirement::COLOUR);
			target.reqsRemoved.clear();

			//Reset to white. Requirement should not be removed again.
			obj->colour.set(COL_WHITE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Other properties should not require colour
			obj->lifeSpan.set(1.0f);
			obj->lifeSpanVar.set(1.0f);
			obj->size.set(1.0f);
			obj->sizeVar.set(1.0f);
			obj->speed.set(1.0f);
			obj->speedVar.set(1.0f);
			obj->azimuth.set(math::degf(90.0f));
			obj->azimuthVar.set(math::degf(90.0f));
			obj->elevation.set(math::degf(90.0f));
			obj->elevationVar.set(math::degf(90.0f));
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to non-white and disconnect. Requirement should be removed.
			obj->colour.set(COL_RED);
			target.reqsAdded.clear();
			target.reqsRemoved.clear();

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.size() == 1 && target.reqsRemoved[0] == node::ModRequirement::COLOUR);
			target.reqsRemoved.clear();

			//Connecting with non-white should send requirement
			if (c1 && c2) {
				c1->onClick();
				c2->onRelease();
			}

			Assert::IsTrue(target.reqsAdded.size() == 1 && target.reqsAdded[0] == node::ModRequirement::COLOUR);
			target.reqsAdded.clear();
			Assert::IsTrue(target.reqsRemoved.empty());
		}

		//Birth rate should send IControllable and receive IController<float> (single)
		TEST_METHOD(Connector_BirthRate)
		{
			class MockController : public node::IController<float>
			{
			public:
				virtual FlagSet<ControllerFlags>& flags() override { return m_flags; }
				virtual Property<float>& frequency() override { return m_frequency; }
				virtual Property<float>& phase() override { return m_phase; }
				virtual Property<float>& startTime() override { return m_startTime; }
				virtual Property<float>& stopTime() override { return m_stopTime; }

			private:
				FlagSet<ControllerFlags> m_flags;
				Property<float> m_frequency;
				Property<float> m_phase;
				Property<float> m_startTime;
				Property<float> m_stopTime;
			};

			File file{ File::Version::SKYRIM_SE };
			auto ctlr = file.create<NiPSysEmitterCtlr>();


			MockController target0;
			MockController target;
			ConnectorTester<node::Emitter> tester(node::Default<node::BoxEmitter>{}.create(file, nullptr, ctlr));

			tester.tryConnect<node::IControllable, node::IController<float>>(node::Emitter::BIRTH_RATE, false, &target0);
			auto ifc = tester.tryConnect<node::IControllable, node::IController<float>>(node::Emitter::BIRTH_RATE, false, &target);
			Assert::IsNotNull(ifc);

			//Setting the properties on target (but not target0) should set the corresponding on ctlr
			std::uniform_int_distribution<unsigned short> I;
			std::uniform_real_distribution<float> F;

			target0.flags().clear(-1);//better clear any defaults
			target0.flags().raise(I(m_engine));
			target0.frequency().set(F(m_engine));
			target0.phase().set(F(m_engine));
			target0.startTime().set(F(m_engine));
			target0.stopTime().set(F(m_engine));
			Assert::IsFalse(ctlr->flags.raised() == target0.flags().raised());
			Assert::IsFalse(ctlr->frequency.get() == target0.frequency().get());
			Assert::IsFalse(ctlr->phase.get() == target0.phase().get());
			Assert::IsFalse(ctlr->startTime.get() == target0.startTime().get());
			Assert::IsFalse(ctlr->stopTime.get() == target0.stopTime().get());

			target.flags().clear(-1);//better clear any defaults
			target.flags().raise(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			Assert::IsTrue(ctlr->flags.raised() == target.flags().raised());
			Assert::IsTrue(ctlr->frequency.get() == target.frequency().get());
			Assert::IsTrue(ctlr->phase.get() == target.phase().get());
			Assert::IsTrue(ctlr->startTime.get() == target.startTime().get());
			Assert::IsTrue(ctlr->stopTime.get() == target.stopTime().get());

			//Assigning to the interface should assign to ctlr->interpolator
			auto iplr = file.create<NiFloatInterpolator>();
			Assert::IsNotNull(iplr.get());
			ifc->iplr().assign(iplr);
			Assert::IsTrue(ctlr->interpolator.assigned() == iplr);
			ifc->iplr().assign(nullptr);
			Assert::IsTrue(ctlr->interpolator.assigned() == nullptr);

			//TODO: Test the other fields of the interface (no point until we start on nonlinear animations)

			//Make sure listeners are removed
			tester.disconnect<node::IController<float>>(&target);

			//these calls should not reach the node (defaults should have been restored)
			target.flags().clear(-1);
			target.flags().raise(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			Assert::IsTrue(ctlr->flags.raised() == node::DEFAULT_CTLR_FLAGS);
			Assert::IsTrue(ctlr->frequency.get() == node::DEFAULT_FREQUENCY);
			Assert::IsTrue(ctlr->phase.get() == node::DEFAULT_PHASE);
			Assert::IsTrue(ctlr->startTime.get() == node::DEFAULT_STARTTIME);
			Assert::IsTrue(ctlr->stopTime.get() == node::DEFAULT_STOPTIME);
		}
	};
	
	TEST_CLASS(VolumeEmitter)
	{
		//Test connection and assignment of an emitter object
		TEST_METHOD(EmitterObject)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysBoxEmitter>();
			AssignableSenderTest(node::Default<node::BoxEmitter>{}.create(file, obj), obj->emitterObject, node::VolumeEmitter::EMITTER_OBJECT, false);
		}
	};
}
