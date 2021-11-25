#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes_internal.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(FloatController)
	{
		std::mt19937 m_engine;

	public:

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

		//Target connector should receive IAssignable<NiInterpolator> (single)
		//and send IController<float>, or something like that.
		//Multiconnector would probably work here, but doesn't seem very useful. Let's wait with that.
		TEST_METHOD(Connector_Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiFloatInterpolator>();

			Assignable<NiInterpolator> target0;
			Assignable<NiInterpolator> target;
			ConnectorTester<node::FloatController> tester(node::Default<node::FloatController>{}.create(file, obj));

			tester.tryConnect<node::IController<float>, Assignable<NiInterpolator>>(node::FloatController::TARGET, false, &target0);
			auto ifc = tester.tryConnect<node::IController<float>, Assignable<NiInterpolator>>(node::FloatController::TARGET, false, &target);
			Assert::IsNotNull(ifc);

			Assert::IsTrue(target0.assigned() == nullptr);
			Assert::IsTrue(target.assigned() == obj);

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
			tester.disconnect<Assignable<NiInterpolator>>(&target);
			Assert::IsTrue(target.assigned() == nullptr);
		}

	};
}
