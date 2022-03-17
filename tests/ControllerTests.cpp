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

namespace objects
{
	//We may have to prioritise interpolator factories for all controller types.
	//NifSkope cannot handle NiInterpolator, unsure how the game would react to them.
	//Also unsure how the game reacts no null iplrs in controller sequences. Need to test!

	TEST_CLASS(InterpolatorFactory)
	{
	public:

		template<typename T, typename IplrT>
		void iplrFactoryTest(const std::string& iplrID = std::string())
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };
			auto obj = file.create<T>();
			node::IplrFactoryVisitor v(file, iplrID);
			obj->receive(v);
			if constexpr (std::is_same<IplrT, void>::value) {
				Assert::IsNull(v.iplr.get());
			}
			else {
				Assert::IsNotNull(v.iplr.get());
				Assert::IsTrue(v.iplr->type() == IplrT::TYPE);
			}
		}

		TEST_METHOD(NiTimeController)
		{
			iplrFactoryTest<nif::NiTimeController, void>();
		}

		TEST_METHOD(NiPSysEmitterCtlr)
		{
			iplrFactoryTest<nif::NiPSysEmitterCtlr, nif::NiFloatInterpolator>("BirthRate");
			iplrFactoryTest<nif::NiPSysEmitterCtlr, nif::NiBoolInterpolator>("EmitterActive");
		}

		TEST_METHOD(NiPSysGravityStrengthCtlr)
		{
			iplrFactoryTest<nif::NiPSysGravityStrengthCtlr, nif::NiFloatInterpolator>();
		}
	};
}

namespace nodes
{
	using namespace nif;

	class MockControllable final : public node::IControllable
	{
	public:
		MockControllable()
		{
			m_ctlr = std::make_shared<NiTimeController>();
			m_ctlrIDProperty = std::make_shared<Property<std::string>>();
		}

		virtual Ref<NiInterpolator>& iplr() override { return m_iplr; }
		virtual Ref<NiAVObject>& node() { return m_node; }
		virtual ni_ptr<NiTimeController> ctlr() override { return m_ctlr; }
		virtual std::string propertyType() override { return std::string(); }
		virtual std::string ctlrType() override { return std::string(); }
		virtual std::string ctlrID() override { return std::string(); }
		virtual std::string iplrID() override { return std::string(); }
		virtual ni_ptr<Property<std::string>> ctlrIDProperty() override { return m_ctlrIDProperty; }

	private:
		Ref<NiInterpolator> m_iplr;
		Ref<NiAVObject> m_node;
		ni_ptr<NiTimeController> m_ctlr;
		ni_ptr<Property<std::string>> m_ctlrIDProperty;
	};

	class Listener : public PropertyListener<float>
	{
	public:
		virtual void onSet(const float& f) override
		{
			m_signalled = true;
			m_set = f;
		}

		bool wasSet()
		{
			bool result = m_signalled;
			m_signalled = false;
			m_set = std::numeric_limits<float>::quiet_NaN();
			return result;
		}

		bool wasSet(float f)
		{
			bool result = m_signalled && m_set == f;
			m_signalled = false;
			m_set = std::numeric_limits<float>::quiet_NaN();
			return result;
		}

	private:
		float m_set{ std::numeric_limits<float>::quiet_NaN() };
		bool m_signalled{ false };
	};

	class FlagsListener : public FlagSetListener<ControllerFlags>
	{
	public:
		virtual void onRaise(ControllerFlags flags) override
		{
			m_set = flags;
		}
		virtual void onClear(ControllerFlags flags) override
		{
			m_cleared = flags;
		}

		bool wasRaised()
		{
			bool result = m_set != 0;
			m_set = 0;
			return result;
		}
		bool wasRaised(ControllerFlags flags)
		{
			bool result = m_set == flags;
			m_set = 0;
			return result;
		}
		bool wasCleared()
		{
			bool result = m_cleared != 0;
			m_cleared = 0;
			return result;
		}
		bool wasCleared(ControllerFlags flags)
		{
			bool result = m_cleared == flags;
			m_cleared = 0;
			return result;
		}

	private:
		ControllerFlags m_set{ 0 };
		ControllerFlags m_cleared{ 0 };
	};

	TEST_CLASS(FloatController)
	{
		std::mt19937 m_engine;

	public:
		//Target connector should receive IControllable (single) and send IController<float>.
		//Multiconnector would probably work here, but doesn't seem very useful. Let's wait with that.
		TEST_METHOD(Connector_Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiFloatInterpolator>();

			MockControllable target0;
			MockControllable target;
			ConnectorTester<node::FloatController> tester(node::Default<node::FloatController>{}.create(file, obj));

			tester.tryConnect<node::IController<float>, node::IControllable>(node::FloatController::TARGET, false, &target0);
			auto ifc = tester.tryConnect<node::IController<float>, node::IControllable>(node::FloatController::TARGET, false, &target);
			Assert::IsNotNull(ifc);

			Assert::IsTrue(target0.iplr().assigned() == nullptr);
			Assert::IsTrue(target.iplr().assigned() == obj);

			//Test the interface
			//Add listeners to the properties and make sure they get called
			FlagsListener l1;
			Listener l2;
			Listener l3;
			Listener l4;
			Listener l5;

			std::uniform_int_distribution<ControllerFlags> I;
			std::uniform_real_distribution<float> F;

			tester.getNode()->flags().clear(-1);//need to clear defaults

			//add the listeners
			ifc->flags().addListener(l1);
			ifc->frequency().addListener(l2);
			ifc->phase().addListener(l3);
			ifc->startTime().addListener(l4);
			ifc->stopTime().addListener(l5);

			//Set one property at a time. Only the right listener should be signalled.
			tester.getNode()->flags().raise(I(m_engine));
			Assert::IsTrue(l1.wasRaised(tester.getNode()->flags().raised()));
			Assert::IsFalse(l2.wasSet());
			Assert::IsFalse(l3.wasSet());
			Assert::IsFalse(l4.wasSet());
			Assert::IsFalse(l5.wasSet());

			tester.getNode()->frequency().set(F(m_engine));
			Assert::IsFalse(l1.wasRaised());
			Assert::IsTrue(l2.wasSet(tester.getNode()->frequency().get()));
			Assert::IsFalse(l3.wasSet());
			Assert::IsFalse(l4.wasSet());
			Assert::IsFalse(l5.wasSet());

			tester.getNode()->phase().set(F(m_engine));
			Assert::IsFalse(l1.wasRaised());
			Assert::IsFalse(l2.wasSet());
			Assert::IsTrue(l3.wasSet(tester.getNode()->phase().get()));
			Assert::IsFalse(l4.wasSet());
			Assert::IsFalse(l5.wasSet());

			tester.getNode()->startTime().set(F(m_engine));
			Assert::IsFalse(l1.wasRaised());
			Assert::IsFalse(l2.wasSet());
			Assert::IsFalse(l3.wasSet());
			Assert::IsTrue(l4.wasSet(tester.getNode()->startTime().get()));
			Assert::IsFalse(l5.wasSet());

			tester.getNode()->stopTime().set(F(m_engine));
			Assert::IsFalse(l1.wasRaised());
			Assert::IsFalse(l2.wasSet());
			Assert::IsFalse(l3.wasSet());
			Assert::IsFalse(l4.wasSet());
			Assert::IsTrue(l5.wasSet(tester.getNode()->stopTime().get()));

			//remove and make sure no one is signalled
			ifc->flags().removeListener(l1);
			ifc->frequency().removeListener(l2);
			ifc->phase().removeListener(l3);
			ifc->startTime().removeListener(l4);
			ifc->stopTime().removeListener(l5);

			tester.getNode()->flags().clear(-1);
			tester.getNode()->flags().raise(I(m_engine));
			tester.getNode()->frequency().set(F(m_engine));
			tester.getNode()->phase().set(F(m_engine));
			tester.getNode()->startTime().set(F(m_engine));
			tester.getNode()->stopTime().set(F(m_engine));

			Assert::IsFalse(l1.wasCleared());
			Assert::IsFalse(l1.wasRaised());
			Assert::IsFalse(l2.wasSet());
			Assert::IsFalse(l3.wasSet());
			Assert::IsFalse(l4.wasSet());
			Assert::IsFalse(l5.wasSet());

			//disconnecting should unassign the interpolator
			tester.disconnect<node::IControllable>(&target);
			Assert::IsTrue(target.iplr().assigned() == nullptr);
		}
	};

	TEST_CLASS(NLFloatController)
	{
	public:
		TEST_METHOD(Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiBlendFloatInterpolator>();

			auto node = node::Default<node::NLFloatController>{}.create(file, obj);
			auto am = std::make_shared<node::AnimationManager>();
			node->setAnimationManager(am);
			Assert::IsTrue(am->blocks().size() == 0);

			MockControllable target0;
			MockControllable target;

			auto av0 = file.create<NiAVObject>();
			target0.node().assign(av0);
			target.node().assign(av0);

			ConnectorTester<node::NLFloatController> tester(std::move(node));

			tester.tryConnect<node::IController<float>, node::IControllable>(node::NLFloatController::TARGET, false, &target0);
			auto ifc = tester.tryConnect<node::IController<float>, node::IControllable>(node::NLFloatController::TARGET, false, &target);
			Assert::IsNotNull(ifc);
			//we only care about flags here
			Assert::IsTrue(ifc->flags().hasRaised(CTLR_MNGR_CTRLD));

			Assert::IsFalse(target0.iplr().assigned() == obj);
			Assert::IsTrue(target.iplr().assigned() == obj);

			//Should also register this block with the AnimationManager, if the target is assigned.
			//This would be nicer to test if we could mock out the manager too. 
			//We only really want to know if register/unregisterBlock was called.
			Assert::IsTrue(am->blocks().size() == 1);
			Assert::IsTrue(am->blocks().front().controller == target.ctlr());
			Assert::IsTrue(am->blocks().front().target.assigned() == av0);
			auto av1 = file.create<NiAVObject>();
			target.node().assign(av1);
			Assert::IsTrue(am->blocks().size() == 1);
			Assert::IsTrue(am->blocks().front().controller == target.ctlr());
			Assert::IsTrue(am->blocks().front().target.assigned() == av1);
			target.node().assign(nullptr);
			Assert::IsTrue(am->blocks().size() == 0);
			target.node().assign(av0);
			Assert::IsTrue(am->blocks().size() == 1);

			//disconnect
			tester.disconnect<node::IControllable>(&target);
			Assert::IsTrue(target.iplr().assigned() == nullptr);
			Assert::IsTrue(am->blocks().size() == 0);
		}
	};
}
