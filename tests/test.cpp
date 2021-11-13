#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace test
{
	TEST_CLASS(CommonTests)
	{
		std::mt19937 m_engine;

		template<size_t N>
		struct PropertyTester : ObservableBase<IProperty<std::array<float, N>>>
		{
			virtual std::array<float, N> get() const override
			{
				return data;
			}
			virtual void set(const std::array<float, N>& a) override
			{
				if (a != data) {
					data = a;
					//perturb the data
					for (size_t i = 0; i < N; i++)
						data[i] += data[i] * delta(engine) * (sign(engine) ? 1.0f : -1.0f);
					for (auto&& l : getListeners())
						l->onSet(data);
				}
			}

			//error bounds
			float lower = 1.0e-3f;
			float upper = 1.0e-1f;

		private:
			std::array<float, N> data{ 0.0f };
			std::mt19937 engine;
			std::uniform_int_distribution<int> sign{ 0, 1 };
			std::uniform_real_distribution<float> delta{ lower, upper };
		};

		TEST_METHOD(PropertyTest)
		{
			PropertyTester<3> tester;
			::PropertyTest<std::array<float, 3>>(tester, m_engine, tester.upper, true, 100);
			::PropertyTest<std::array<float, 3>>(tester, m_engine, tester.lower, false, 100);
		}
	};
}
