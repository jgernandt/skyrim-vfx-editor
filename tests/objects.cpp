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

	TEST_CLASS(NiObjectNET)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiObjectNET>{}.run(); }
	};

	TEST_CLASS(NiAVObject)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiAVObject>{}.run(); }
	};

	TEST_CLASS(NiNode)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiNode>{}.run(); }
	};

	TEST_CLASS(NiParticleSystem)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiParticleSystem>{}.run(); }
	};

	TEST_CLASS(NiPSysModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysModifier>{}.run(); }
	};

	TEST_CLASS(NiExtraData)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiExtraData>{}.run(); }
	};

	TEST_CLASS(NiStringExtraData)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiStringExtraData>{}.run(); }
	};
}
