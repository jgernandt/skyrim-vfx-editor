#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::ConnectorTester<NiPSysModifier>::operator()(NiPSysModifier& obj, File& file)
{
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	return false;
}

bool objects::ConnectorTester<NiPSysModifier>::operator()(const NiPSysModifier& obj, const TestConstructor& ctor)
{
	//Our target should handle everything
	Assert::IsTrue(ctor.connections.size() == 0);
	Assert::IsTrue(ctor.modConnections.second.empty());

	return false;
}

bool objects::FactoryTester<NiPSysModifier>::operator()(NiPSysModifier& obj, TestConstructor& ctor, File& file)
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

bool objects::FactoryTester<NiPSysModifier>::operator()(const NiPSysModifier& obj, const TestConstructor& ctor)
{
	//We need to pick up any controllers that belong to us.
	//Derived modifiers will never get here. They will create their node and return false.
	//They will have to do the same thing (which kind of sucks, should be reusable!).

	Assert::IsTrue(ctor.node.first == &obj);
	Assert::IsNotNull(dynamic_cast<node::DummyModifier*>(ctor.node.second.get()));

	//How to directly test if we picked up the controllers?
	//We could check if they respond to name a change, but that's indirect and may give a false negative.
	//Good enough for now, I guess.
	std::string name = obj.name.get();
	const_cast<NiPSysModifier&>(obj).name.set("oawgnvauvb");//I just had to make it const, right?

	auto ctlr0 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(0).get());
	Assert::IsTrue(ctlr0->modifierName.get() == obj.name.get());

	auto ctlr1 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(1).get());
	Assert::IsFalse(ctlr1->modifierName.get() == obj.name.get());

	auto ctlr2 = static_cast<NiPSysModifierCtlr*>(obj.target.assigned()->controllers.at(2).get());
	Assert::IsTrue(ctlr2->modifierName.get() == obj.name.get());

	const_cast<NiPSysModifier&>(obj).name.set(name);

	return false;
}

bool objects::ForwardTester<NiPSysModifier>::operator()(NiPSysModifier& obj, TestConstructor& ctor, File& file)
{
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	return false;
}

bool objects::ForwardTester<NiPSysModifier>::operator()(const NiPSysModifier&, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.empty());
	return false;
}

