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
}
