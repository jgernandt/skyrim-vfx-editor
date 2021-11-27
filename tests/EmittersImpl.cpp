#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;


bool objects::TestSetup<NiPSysEmitter>::operator()(NiPSysEmitter& obj, File& file)
{
	TestSetup<NiPSysModifier>::operator()(obj, file);

	//Should also set up to test for any other Emitter-specific ctlrs, if we introduce them

	auto ctlr3 = file.create<NiPSysEmitterCtlr>();
	obj.target.assigned()->controllers.insert(3, ctlr3);
	ctlr3->modifierName.set(obj.name.get());

	return false;
}

bool objects::FactoryTester<NiPSysEmitter>::operator()(const NiPSysEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::Emitter>(obj, ctor);

	Assert::IsTrue(obj.target.assigned()->controllers.size() == 4);
	controllerTest(obj, ctor);

	return false;
}

void objects::FactoryTester<NiPSysEmitter>::controllerTest(const NiPSysEmitter& obj, const TestConstructor& ctor)
{
	FactoryTester<NiPSysModifier>::controllerTest(obj, ctor);

	std::string name = obj.name.get();
	const_cast<NiPSysEmitter&>(obj).name.set("baoerbnhio");

	auto ctlr3 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(3).get());
	Assert::IsTrue(ctlr3->modifierName.get() == obj.name.get());

	const_cast<NiPSysEmitter&>(obj).name.set(name);
}


bool objects::TestSetup<NiPSysVolumeEmitter>::operator()(NiPSysVolumeEmitter& obj, File& file)
{
	TestSetup<NiPSysEmitter>::operator()(obj, file);
	obj.emitterObject.assign(file.getRoot());
	return false;
}

bool objects::ConnectorTester<NiPSysVolumeEmitter>::operator()(const NiPSysVolumeEmitter& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::VolumeEmitter::EMITTER_OBJECT);
	Assert::IsTrue(ctor.connections[0].object2 == obj.emitterObject.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Node::OBJECT);
	return false;
}

bool objects::FactoryTester<NiPSysVolumeEmitter>::operator()(const NiPSysVolumeEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::VolumeEmitter>(obj, ctor);
	return true;//need to test ctlrs here
}


bool objects::FactoryTester<NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::BoxEmitter>(obj, ctor);
	return true;//need to test ctlrs here
}


bool objects::FactoryTester<NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::CylinderEmitter>(obj, ctor);
	return true;//need to test ctlrs here
}


bool objects::FactoryTester<NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::SphereEmitter>(obj, ctor);
	return true;//need to test ctlrs here
}
