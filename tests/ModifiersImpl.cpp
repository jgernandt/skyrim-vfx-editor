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


bool objects::TestSetup<NiPSysGravityModifier>::operator()(NiPSysGravityModifier& obj, File& file)
{
	TestSetup<NiPSysModifier>::operator()(obj, file);
	obj.gravityObject.assign(file.getRoot());

	//Set up our specific ctlrs
	auto strengthCtlr = file.create<NiPSysGravityStrengthCtlr>();
	strengthCtlr->modifierName.set(obj.name.get());
	auto target = obj.target.assigned();
	target->controllers.insert(target->controllers.size(), strengthCtlr);
	auto strengthIplr = file.create<NiFloatInterpolator>();
	strengthCtlr->interpolator.assign(strengthIplr);

	return false;
}

bool objects::ConnectorTester<NiPSysGravityModifier>::operator()(const NiPSysGravityModifier& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::GravityModifier::GRAVITY_OBJECT);
	Assert::IsTrue(ctor.connections[0].object2 == obj.gravityObject.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Node::OBJECT);
	return false;
}

bool objects::FactoryTester<NiPSysGravityModifier>::operator()(const NiPSysGravityModifier& obj, const TestConstructor& ctor)
{
	if (obj.forceType.get() == FORCE_PLANAR)
		nodeTest<node::PlanarForceField>(obj, ctor);
	else if (obj.forceType.get() == FORCE_SPHERICAL)
		nodeTest<node::SphericalForceField>(obj, ctor);
	else
		nodeTest<node::Modifier>(obj, ctor);//unspecified type

	auto ctlrs = static_cast<node::Modifier*>(ctor.node.second.get())->getControllers();
	Assert::IsTrue(ctlrs.size() == 3);
	controllerTest(ctlrs, obj.target.assigned());
	//Complete with our specific controllers
	Assert::IsTrue(std::find(ctlrs.begin(), ctlrs.end(), obj.target.assigned()->controllers.at(3).get()) != ctlrs.end());

	return false;
}

void objects::FactoryTest<NiPSysGravityModifier>::run()
{
	File file(File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysGravityModifier>();
	TestSetup<NiPSysGravityModifier>{}.up(*obj, file);

	{//Planar
		obj->forceType.set(FORCE_PLANAR);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysGravityModifier>{}.up(*obj, ctor);
		FactoryTester<NiPSysGravityModifier>{}.up(*obj, ctor);
	}
	{//Spherical
		obj->forceType.set(FORCE_SPHERICAL);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysGravityModifier>{}.up(*obj, ctor);
		FactoryTester<NiPSysGravityModifier>{}.up(*obj, ctor);
	}
	{//Unknown
		obj->forceType.set(FORCE_UNKNOWN);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysGravityModifier>{}.up(*obj, ctor);
		FactoryTester<NiPSysGravityModifier>{}.up(*obj, ctor);
	}
}


bool objects::TestSetup<NiPSysRotationModifier>::operator()(NiPSysRotationModifier& obj, File& file)
{
	TestSetup<NiPSysModifier>::operator()(obj, file);

	//TODO: Set up our specific ctlrs

	return false;
}

bool objects::FactoryTester<NiPSysRotationModifier>::operator()(const NiPSysRotationModifier& obj, const TestConstructor& ctor)
{
	nodeTest<node::RotationModifier>(obj, ctor);

	//TODO: Complete with our specific controllers
	auto ctlrs = static_cast<node::Modifier*>(ctor.node.second.get())->getControllers();
	Assert::IsTrue(ctlrs.size() == 2);
	controllerTest(ctlrs, obj.target.assigned());

	return false;
}


bool objects::FactoryTester<BSPSysScaleModifier>::operator()(const BSPSysScaleModifier& obj, const TestConstructor& ctor)
{
	nodeTest<node::ScaleModifier>(obj, ctor);

	auto ctlrs = static_cast<node::Modifier*>(ctor.node.second.get())->getControllers();
	Assert::IsTrue(ctlrs.size() == 2);
	controllerTest(ctlrs, obj.target.assigned());

	return false;
}


bool objects::FactoryTester<BSPSysSimpleColorModifier>::operator()(const BSPSysSimpleColorModifier& obj, const TestConstructor& ctor)
{
	nodeTest<node::SimpleColourModifier>(obj, ctor);

	auto ctlrs = static_cast<node::Modifier*>(ctor.node.second.get())->getControllers();
	Assert::IsTrue(ctlrs.size() == 2);
	controllerTest(ctlrs, obj.target.assigned());

	return false;
}
