#include "pch.h"
#include "CppUnitTest.h"

#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"

#include "nodes.h"
#include "Constructor.inl"

namespace objects
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;

	template<typename T>
	struct ConnectorTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			ConnectorTester<T>{}.up(*obj, file);//this overload sets up obj
			node::Connector<T>{}.down(*obj, c);
			ConnectorTester<T>{}.up(*obj, c);//this overload makes the assertions
		}
	};

	template<typename T>
	struct FactoryTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			c.pushObject(obj);
			FactoryTester<T>{}.up(*obj, c, file);//this overload sets up obj and c
			node::Factory<T>{}.up(*obj, c);
			FactoryTester<T>{}.up(*obj, c);//this overload makes the assertions
		}
	};

	template<typename T>
	struct ForwardTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<T>();

			TestConstructor c(file);
			ForwardTester<T>{}.up(*obj, c, file);//this overload sets up obj and c
			node::Forwarder<T>{}.down(*obj, c);
			ForwardTester<T>{}.up(*obj, c);//this overload makes the assertions
		}
	};

	TEST_CLASS(NiNode)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiNode>{}.run(); }
	};
}