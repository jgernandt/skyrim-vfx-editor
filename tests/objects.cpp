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

	TEST_CLASS(NiBillboardNode)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiBillboardNode>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiBillboardNode>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiBillboardNode>{}.run(); }
	};

	TEST_CLASS(BSEffectShaderProperty)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::BSEffectShaderProperty>{}.run(); }
	};

	TEST_CLASS(NiControllerManager)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiControllerManager>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiControllerManager>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiControllerManager>{}.run(); }
	};

	TEST_CLASS(NiControllerSequence)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiControllerSequence>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiControllerSequence>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiControllerSequence>{}.run(); }
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

	TEST_CLASS(NiPSysGravityModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysGravityModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysPositionModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysPositionModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysRotationModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysRotationModifier>{}.run(); }
	};

	TEST_CLASS(BSPSysScaleModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::BSPSysScaleModifier>{}.run(); }
	};

	TEST_CLASS(BSPSysSimpleColorModifier)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::BSPSysSimpleColorModifier>{}.run(); }
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

	TEST_CLASS(NiPSysEmitterCtlr)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysEmitterCtlr>{}.run(); }
	};

	TEST_CLASS(NiPSysGravityStrengthCtlr)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiPSysGravityStrengthCtlr>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiPSysGravityStrengthCtlr>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiPSysGravityStrengthCtlr>{}.run(); }
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

	TEST_CLASS(NiStringsExtraData)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::NiStringsExtraData>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::NiStringsExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::NiStringsExtraData>{}.run(); }
	};

	TEST_CLASS(BSBehaviorGraphExtraData)
	{
	public:
		TEST_METHOD(Connector) { ConnectorTest<nif::BSBehaviorGraphExtraData>{}.run(); }
		TEST_METHOD(Factory) { FactoryTest<nif::BSBehaviorGraphExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwardTest<nif::BSBehaviorGraphExtraData>{}.run(); }
	};
}
