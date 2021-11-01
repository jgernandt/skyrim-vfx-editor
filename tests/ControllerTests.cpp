#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(NiBoolInterpolatorTests)
	{
		nif::NiBoolInterpolator obj;
		std::mt19937 m_engine;

		TEST_METHOD(Value)
		{
			PropertyTest<bool>(obj.value(), m_engine);
		}

		TEST_METHOD(Data)
		{
			AssignableTest<nif::NiBoolData>(obj.data());
		}
	};

	TEST_CLASS(NiFloatInterpolatorTests)
	{
		nif::NiFloatInterpolator obj;
		std::mt19937 m_engine;

		TEST_METHOD(Value)
		{
			PropertyTest<float>(obj.value(), m_engine);
		}

		TEST_METHOD(Data)
		{
			AssignableTest<nif::NiFloatData>(obj.data());
		}
	};

	TEST_CLASS(NiTimeControllerTests)
	{
		nif::NiPSysEmitterCtlr concrete_obj;
		nif::NiTimeController& obj = concrete_obj;
		std::mt19937 m_engine;

		TEST_METHOD(Flags)
		{
			PropertyTest<unsigned short>(obj.flags(), m_engine);
		}

		TEST_METHOD(Frequency)
		{
			PropertyTest<float>(obj.frequency(), m_engine);
		}

		TEST_METHOD(Phase)
		{
			PropertyTest<float>(obj.phase(), m_engine);
		}

		TEST_METHOD(StartTime)
		{
			PropertyTest<float>(obj.startTime(), m_engine);
		}

		TEST_METHOD(StopTime)
		{
			PropertyTest<float>(obj.stopTime(), m_engine);
		}
	};

	TEST_CLASS(NiSingleInterpControllerTests)
	{
		nif::NiPSysEmitterCtlr concrete_obj;
		nif::NiSingleInterpController& obj = concrete_obj;
		std::mt19937 m_engine;

		TEST_METHOD(Interpolator)
		{
			AssignableTest<nif::NiInterpolator, nif::NiFloatInterpolator>(obj.interpolator());
		}
	};
}

namespace node
{
	TEST_CLASS(FloatControllerTests)
	{
		std::mt19937 m_engine;

	public:

		//Target connector should receive IAssignable<NiInterpolator> (single)
		//and send IController<float>, or something like that.
		//Multiconnector would probably work here, but doesn't seem very useful. Let's wait with that.
		TEST_METHOD(Target)
		{
			std::unique_ptr<FloatController> node = std::make_unique<FloatController>();
			nif::NiInterpolator* obj = &node->object();

			MockAssignable<nif::NiInterpolator> target0;
			MockAssignable<nif::NiInterpolator> target;
			ConnectorTester<FloatController> tester(std::move(node));

			tester.tryConnect<IController<float>, IAssignable<nif::NiInterpolator>>(FloatController::TARGET, false, &target0);
			auto ifc = tester.tryConnect<IController<float>, IAssignable<nif::NiInterpolator>>(FloatController::TARGET, false, &target);
			Assert::IsNotNull(ifc);

			Assert::IsTrue(!target0.isAssigned(obj));
			Assert::IsTrue(target.isAssigned(obj));

			//Test the interface
			//Add listeners to the properties and make sure they get called
			MockPropertyListener<unsigned short> l1;
			MockPropertyListener<float> l2;
			MockPropertyListener<float> l3;
			MockPropertyListener<float> l4;
			MockPropertyListener<float> l5;

			std::uniform_int_distribution<unsigned short> I;
			std::uniform_real_distribution<float> F;

			ifc->flags().addListener(l1);
			Assert::IsTrue(l1.isSignalled() && l1.result() == tester.getNode()->flags().get());
			l1.reset();
			tester.getNode()->flags().set(I(m_engine));
			Assert::IsTrue(l1.isSignalled() && l1.result() == tester.getNode()->flags().get());
			l1.reset();

			ifc->frequency().addListener(l2);
			Assert::IsTrue(l2.isSignalled() && l2.result() == tester.getNode()->frequency().get());
			l2.reset();
			tester.getNode()->frequency().set(F(m_engine));
			Assert::IsTrue(l2.isSignalled() && l2.result() == tester.getNode()->frequency().get());
			l2.reset();

			ifc->phase().addListener(l3);
			Assert::IsTrue(l3.isSignalled() && l3.result() == tester.getNode()->phase().get());
			l3.reset();
			tester.getNode()->phase().set(F(m_engine));
			Assert::IsTrue(l3.isSignalled() && l3.result() == tester.getNode()->phase().get());
			l3.reset();

			ifc->startTime().addListener(l4);
			Assert::IsTrue(l4.isSignalled() && l4.result() == tester.getNode()->startTime().get());
			l4.reset();
			tester.getNode()->startTime().set(F(m_engine));
			Assert::IsTrue(l4.isSignalled() && l4.result() == tester.getNode()->startTime().get());
			l4.reset();

			ifc->stopTime().addListener(l5);
			Assert::IsTrue(l5.isSignalled() && l5.result() == tester.getNode()->stopTime().get());
			l5.reset();
			tester.getNode()->stopTime().set(F(m_engine));
			Assert::IsTrue(l5.isSignalled() && l5.result() == tester.getNode()->stopTime().get());
			l5.reset();

			//Make sure everything was disconnected
			tester.disconnect<IAssignable<nif::NiInterpolator>>(&target);
			Assert::IsFalse(target.isAssigned(obj));

			tester.getNode()->flags().set(I(m_engine));
			Assert::IsFalse(l1.isSignalled());

			tester.getNode()->frequency().set(F(m_engine));
			Assert::IsFalse(l2.isSignalled());

			tester.getNode()->phase().set(F(m_engine));
			Assert::IsFalse(l3.isSignalled());

			tester.getNode()->startTime().set(F(m_engine));
			Assert::IsFalse(l4.isSignalled());

			tester.getNode()->stopTime().set(F(m_engine));
			Assert::IsFalse(l5.isSignalled());
		}

	};
}
