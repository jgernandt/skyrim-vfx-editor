#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::FactoryTester<NiPSysEmitter>::operator()(NiPSysEmitter& obj, TestConstructor& ctor, File& file)
{
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	obj.name.set("ajnseogo");

	auto ctlr0 = file.create<NiPSysModifierCtlr>();
	target->controllers.insert(0, ctlr0);
	auto ctlr1 = file.create<NiPSysModifierCtlr>();
	target->controllers.insert(1, ctlr1);
	auto ctlr2 = file.create<NiPSysEmitterCtlr>();
	target->controllers.insert(2, ctlr2);

	//0 and 2 are ours
	ctlr0->modifierName.set(obj.name.get());
	ctlr2->modifierName.set(obj.name.get());

	return false;
}

void objects::FactoryTester<NiPSysEmitter>::controllerTest(const NiPSysEmitter& obj, const TestConstructor& ctor)
{
	//This should test all the controller types for Emitters, but let's copy the Modifier's test for now

	std::string name = obj.name.get();
	const_cast<NiPSysEmitter&>(obj).name.set("oawgnvauvb");//I just had to make it const, right?

	auto ctlr0 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(0).get());
	Assert::IsTrue(ctlr0->modifierName.get() == obj.name.get());

	auto ctlr1 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(1).get());
	Assert::IsFalse(ctlr1->modifierName.get() == obj.name.get());

	auto ctlr2 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(2).get());
	Assert::IsTrue(ctlr2->modifierName.get() == obj.name.get());

	const_cast<NiPSysEmitter&>(obj).name.set(name);
}


bool objects::ConnectorTester<NiPSysBoxEmitter>::operator()(NiPSysBoxEmitter& obj, File& file)
{
	obj.emitterObject.assign(file.getRoot());
	return false;
}

bool objects::ConnectorTester<NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::VolumeEmitter::EMITTER_OBJECT);
	Assert::IsTrue(ctor.connections[0].object2 == obj.emitterObject.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Node::OBJECT);
	return false;
}

bool objects::FactoryTester<NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::BoxEmitter>(&obj, ctor);
	controllerTest(obj, ctor);
	return false;
}


bool objects::ConnectorTester<NiPSysCylinderEmitter>::operator()(NiPSysCylinderEmitter& obj, File& file)
{
	obj.emitterObject.assign(file.getRoot());
	return false;
}

bool objects::ConnectorTester<NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::VolumeEmitter::EMITTER_OBJECT);
	Assert::IsTrue(ctor.connections[0].object2 == obj.emitterObject.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Node::OBJECT);
	return false;
}

bool objects::FactoryTester<NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::CylinderEmitter>(&obj, ctor);
	controllerTest(obj, ctor);
	return false;
}


bool objects::ConnectorTester<NiPSysSphereEmitter>::operator()(NiPSysSphereEmitter& obj, File& file)
{
	obj.emitterObject.assign(file.getRoot());
	return false;
}

bool objects::ConnectorTester<NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::VolumeEmitter::EMITTER_OBJECT);
	Assert::IsTrue(ctor.connections[0].object2 == obj.emitterObject.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Node::OBJECT);
	return false;
}

bool objects::FactoryTester<NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter& obj, const TestConstructor& ctor)
{
	nodeTest<node::SphereEmitter>(&obj, ctor);
	controllerTest(obj, ctor);
	return false;
}
