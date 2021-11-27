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

	TEST_CLASS(BSEffectShaderProperty)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::BSEffectShaderProperty>{}.run(); }
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

	TEST_CLASS(NiPSysAgeDeathModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysAgeDeathModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysBoundUpdateModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysPositionModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysPositionModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysBoxEmitter)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysBoxEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysCylinderEmitter)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysCylinderEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysSphereEmitter)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysSphereEmitter>{}.run(); }
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
