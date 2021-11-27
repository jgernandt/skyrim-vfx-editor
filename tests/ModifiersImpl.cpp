#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::TestSetup<NiPSysModifier>::operator()(NiPSysModifier& obj, File& file)
{
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	obj.name.set("ajnseogo");

	//Should set up to test for unknown and general controllers (ModifierActive)
	auto ctlr0 = file.create<NiPSysModifierCtlr>();
	target->controllers.insert(0, ctlr0);
	auto ctlr1 = file.create<NiPSysModifierCtlr>();
	target->controllers.insert(1, ctlr1);
	auto ctlr2 = file.create<NiPSysModifierCtlr>();// -> ModifierActive, if we introduce it
	target->controllers.insert(2, ctlr2);

	//0 and 2 are ours
	ctlr0->modifierName.set(obj.name.get());
	ctlr2->modifierName.set(obj.name.get());

	return false;
}

bool objects::ConnectorTester<NiPSysModifier>::operator()(const NiPSysModifier& obj, const TestConstructor& ctor)
{
	//Our target should handle everything
	Assert::IsTrue(ctor.connections.size() == 0);
	Assert::IsTrue(ctor.modConnections.second.empty());

	return false;
}

bool objects::FactoryTester<NiPSysModifier>::operator()(const NiPSysModifier& obj, const TestConstructor& ctor)
{
	nodeTest<node::DummyModifier>(obj, ctor);

	auto ctlrs = static_cast<node::Modifier*>(ctor.node.second.get())->getControllers();
	Assert::IsTrue(ctlrs.size() == 2);
	controllerTest(ctlrs, obj.target.assigned());

	return false;
}

void objects::FactoryTester<NiPSysModifier>::controllerTest(
	const std::vector<NiPSysModifierCtlr*>& ctlrs, const ni_ptr<NiParticleSystem>& target)
{
	Assert::IsTrue(std::find(ctlrs.begin(), ctlrs.end(), target->controllers.at(0).get()) != ctlrs.end());
	Assert::IsTrue(std::find(ctlrs.begin(), ctlrs.end(), target->controllers.at(1).get()) == ctlrs.end());
	Assert::IsTrue(std::find(ctlrs.begin(), ctlrs.end(), target->controllers.at(2).get()) != ctlrs.end());
}

bool objects::ForwardTester<NiPSysModifier>::operator()(const NiPSysModifier&, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.empty());
	return false;
}

