#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(NiFloatDataTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(KeyType)
		{
			auto obj = file.create<NiFloatData>();
			Assert::IsNotNull(obj.get());

			enumPropertyTest<nif::KeyType>(obj->keyType(), 
				{ KeyType::LINEAR, KeyType::QUADRATIC, KeyType::TBC, KeyType::XYZ_ROTATION, KeyType::CONSTANT });
		}

		TEST_METHOD(Keys)
		{
			auto obj = file.create<NiFloatData>();
			Assert::IsNotNull(obj.get());

			std::uniform_real_distribution<float> D;
			auto key = [this, &D]()
			{
				return Key<float>{ D(m_engine), D(m_engine) };
			};
			auto tan = [this, &D]()
			{
				return Tangent<float>{ D(m_engine), D(m_engine) };
			};
			auto tbc = [this, &D]()
			{
				return TBC{ D(m_engine), D(m_engine), D(m_engine) };
			};
			VectorPropertyTest(obj->iplnData().keys(), key);
			VectorPropertyTest(obj->iplnData().tangents(), tan);
			VectorPropertyTest(obj->iplnData().tbc(), tbc);
		}

	};

	TEST_CLASS(NiBoolInterpolatorTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Value)
		{
			auto obj = file.create<NiBoolInterpolator>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->value(), m_engine);
		}

		TEST_METHOD(Data)
		{
			auto obj = file.create<NiBoolInterpolator>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiBoolData>(); };
			AssignableTest<nif::NiBoolData>(obj->data(), factory);
		}
	};

	TEST_CLASS(NiFloatInterpolatorTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Value)
		{
			auto obj = file.create<NiFloatInterpolator>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->value(), m_engine);
		}

		TEST_METHOD(Data)
		{
			auto obj = file.create<NiFloatInterpolator>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiFloatData>(); };
			AssignableTest<nif::NiFloatData>(obj->data(), factory);
		}
	};

	TEST_CLASS(NiTimeControllerTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Flags)
		{
			std::shared_ptr<NiTimeController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			PropertyTest<unsigned short>(obj->flags(), m_engine);
		}

		TEST_METHOD(Frequency)
		{
			std::shared_ptr<NiTimeController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->frequency(), m_engine);
		}

		TEST_METHOD(Phase)
		{
			std::shared_ptr<NiTimeController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->phase(), m_engine);
		}

		TEST_METHOD(StartTime)
		{
			std::shared_ptr<NiTimeController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->startTime(), m_engine);
		}

		TEST_METHOD(StopTime)
		{
			std::shared_ptr<NiTimeController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->stopTime(), m_engine);
		}
	};

	TEST_CLASS(NiSingleInterpControllerTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Interpolator)
		{
			std::shared_ptr<NiSingleInterpController> obj = file.create<NiPSysEmitterCtlr>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiFloatInterpolator>(); };
			AssignableTest<nif::NiInterpolator>(obj->interpolator(), factory);
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
			nif::File file{ nif::File::Version::SKYRIM_SE };

			std::unique_ptr<FloatController> node = std::make_unique<FloatController>(file);
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

			//add the listeners
			ifc->flags().addListener(l1);
			ifc->frequency().addListener(l2);
			ifc->phase().addListener(l3);
			ifc->startTime().addListener(l4);
			ifc->stopTime().addListener(l5);

			//they should NOT be signalled on addition - not anymore!
			Assert::IsFalse(l1.isSignalled() && l1.result() == tester.getNode()->flags().get());	l1.reset();
			Assert::IsFalse(l2.isSignalled() && l2.result() == tester.getNode()->frequency().get());	l2.reset();
			Assert::IsFalse(l3.isSignalled() && l3.result() == tester.getNode()->phase().get());	l3.reset();
			Assert::IsFalse(l4.isSignalled() && l4.result() == tester.getNode()->startTime().get()); l4.reset();
			Assert::IsFalse(l5.isSignalled() && l5.result() == tester.getNode()->stopTime().get()); l5.reset();

			//Set one property at a time. Only the right listener should be signalled.
			tester.getNode()->flags().set(I(m_engine));
			Assert::IsTrue(l1.isSignalled() && l1.result() == tester.getNode()->flags().get());	l1.reset();
			Assert::IsFalse(l2.isSignalled());
			Assert::IsFalse(l3.isSignalled());
			Assert::IsFalse(l4.isSignalled());
			Assert::IsFalse(l5.isSignalled());

			tester.getNode()->frequency().set(F(m_engine));
			Assert::IsFalse(l1.isSignalled());
			Assert::IsTrue(l2.isSignalled() && l2.result() == tester.getNode()->frequency().get());	l2.reset();
			Assert::IsFalse(l3.isSignalled());
			Assert::IsFalse(l4.isSignalled());
			Assert::IsFalse(l5.isSignalled());

			tester.getNode()->phase().set(F(m_engine));
			Assert::IsFalse(l1.isSignalled());
			Assert::IsFalse(l2.isSignalled());
			Assert::IsTrue(l3.isSignalled() && l3.result() == tester.getNode()->phase().get());	l3.reset();
			Assert::IsFalse(l4.isSignalled());
			Assert::IsFalse(l5.isSignalled());

			tester.getNode()->startTime().set(F(m_engine));
			Assert::IsFalse(l1.isSignalled());
			Assert::IsFalse(l2.isSignalled());
			Assert::IsFalse(l3.isSignalled());
			Assert::IsTrue(l4.isSignalled() && l4.result() == tester.getNode()->startTime().get());	l4.reset();
			Assert::IsFalse(l5.isSignalled());

			tester.getNode()->stopTime().set(F(m_engine));
			Assert::IsFalse(l1.isSignalled());
			Assert::IsFalse(l2.isSignalled());
			Assert::IsFalse(l3.isSignalled());
			Assert::IsFalse(l4.isSignalled());
			Assert::IsTrue(l5.isSignalled() && l5.result() == tester.getNode()->stopTime().get()); l5.reset();

			//remove and make sure no one is signalled
			ifc->flags().removeListener(l1);
			ifc->frequency().removeListener(l2);
			ifc->phase().removeListener(l3);
			ifc->startTime().removeListener(l4);
			ifc->stopTime().removeListener(l5);

			tester.getNode()->flags().set(I(m_engine));
			tester.getNode()->frequency().set(F(m_engine));
			tester.getNode()->phase().set(F(m_engine));
			tester.getNode()->startTime().set(F(m_engine));
			tester.getNode()->stopTime().set(F(m_engine));

			Assert::IsFalse(l1.isSignalled());
			Assert::IsFalse(l2.isSignalled());
			Assert::IsFalse(l3.isSignalled());
			Assert::IsFalse(l4.isSignalled());
			Assert::IsFalse(l5.isSignalled());

			//disconnecting should unassign the interpolator
			tester.disconnect<IAssignable<nif::NiInterpolator>>(&target);
			Assert::IsFalse(target.isAssigned(obj));
		}

	};
}
