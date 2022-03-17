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


void objects::FactoryTest<NiFloatInterpolator>::run()
{
	nif::File file(nif::File::Version::SKYRIM_SE);
	auto obj = file.create<NiFloatInterpolator>();

	objects::TestConstructor ctor(file);
	ctor.pushObject(obj);

	auto ctlr = file.create<NiTimeController>();
	//randomise controller settings
	std::mt19937 rng;
	std::uniform_int_distribution<nif::ControllerFlags> I;
	std::uniform_real_distribution<float> F;
	ctlr->flags.raise(I(rng));
	ctlr->frequency.set(F(rng));
	ctlr->phase.set(F(rng));
	ctlr->startTime.set(F(rng));
	ctlr->stopTime.set(F(rng));

	ctor.mapController(obj.get(), ctlr.get());

	node::Factory<NiFloatInterpolator>{}.up(*obj, ctor);

	Assert::IsTrue(ctor.node.first == obj.get());
	node::FloatController* node = dynamic_cast<node::FloatController*>(ctor.node.second.get());
	Assert::IsNotNull(node);

	//test controller settings
	Assert::IsTrue(node->flags().raised() == ctlr->flags.raised());
	Assert::IsTrue(node->frequency().get() == ctlr->frequency.get());
	Assert::IsTrue(node->phase().get() == ctlr->phase.get());
	Assert::IsTrue(node->startTime().get() == ctlr->startTime.get());
	Assert::IsTrue(node->stopTime().get() == ctlr->stopTime.get());
}

bool objects::FactoryTester<NiBlendFloatInterpolator>::operator()(
	const NiBlendFloatInterpolator& obj, const TestConstructor& ctor)
{
	nodeTest<node::NLFloatController>(obj, ctor);

	return false;
}

//Modifier controller tests should all be similar:
//*setup a target and target modifier
//*check that we requested a connection from our iplr to the target field on the target modifier
//*check that a controller node was created (possibly conditional)

bool objects::TestSetup<NiPSysEmitterCtlr>::operator()(NiPSysEmitterCtlr& obj, File& file)
{
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	auto mod0 = file.create<NiPSysModifier>();
	target->modifiers.insert(0, mod0);
	auto mod1 = file.create<NiPSysEmitter>();
	target->modifiers.insert(1, mod1);
	auto mod2 = file.create<NiPSysModifier>();
	target->modifiers.insert(2, mod2);

	obj.modifierName.set("bvvarnlier");
	mod1->name.set(obj.modifierName.get());

	auto iplr = file.create<NiFloatInterpolator>();
	obj.interpolator.assign(iplr);
	auto viplr = file.create<NiBoolInterpolator>();
	obj.visIplr.assign(viplr);

	return false;
}

bool objects::ConnectorTester<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& obj, const TestConstructor& ctor)
{
	//Should always request a connection between its interpolator (if any) and the target mod (mod1)
	auto target = std::static_pointer_cast<NiParticleSystem>(obj.target.assigned());
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == obj.interpolator.assigned().get());
	Assert::IsTrue(ctor.connections[0].field1 == node::ControllerBase::TARGET);
	Assert::IsTrue(ctor.connections[0].object2 == target->modifiers.at(1).get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Emitter::BirthRate::ID);

	//and map us to our interpolator (should be inherited from NiPSysModifierCtlr?)
	Assert::IsTrue(ctor.getController(obj.interpolator.assigned().get()) == &obj);

	return false;
}

bool objects::FactoryTester<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(!ctor.node.second);

	//TODO: visibility interpolator

	return false;
}

bool objects::ForwardTester<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& obj, const TestConstructor& ctor)
{
	//should forward to the interpolator if it is Float and has data or if it is BlendFloat
	if (auto&& iplr = obj.interpolator.assigned()) {
		if (iplr->type() == NiFloatInterpolator::TYPE) {
			if (static_cast<NiFloatInterpolator*>(iplr.get())->data.assigned()) {
				Assert::IsTrue(ctor.forwards.size() == 1);
				Assert::IsTrue(ctor.forwards.front() == iplr.get());
			}
			else
				Assert::IsTrue(ctor.forwards.size() == 0);
		}
		else if (iplr->type() == NiBlendFloatInterpolator::TYPE) {
			Assert::IsTrue(ctor.forwards.size() == 1);
			Assert::IsTrue(ctor.forwards.front() == iplr.get());
		}
		else
			Assert::IsTrue(ctor.forwards.size() == 0);
	}
	else
		Assert::IsTrue(ctor.forwards.size() == 0);

	//TODO: visiplr

	return false;
}

/*Pointless test?

void objects::AnimationTest<NiPSysEmitterCtlr>::run()
{
	nif::File file(nif::File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysEmitterCtlr>();

	TestSetup<NiPSysEmitterCtlr>{}.up(*obj, file);

	ni_ptr<NiParticleSystem> psys = std::static_pointer_cast<NiParticleSystem>(obj->target.assigned());
	auto&& mod = psys->modifiers.at(1);

	//Test birth rate interpolator
	ControlledBlock block;
	block.controller.assign(obj);
	block.ctlrType.set("NiPSysEmitterCtlr");
	block.ctlrID.set("ModifierName");
	block.iplrID.set("BirthRate");

	MockAnimationManager v;
	v.setCurrentBlock(&block);

	node::AnimationInit<NiPSysEmitterCtlr>{}.up(*obj, v);

	Assert::IsTrue(v.visited.empty());
	Assert::IsTrue(v.blocks.size() == 1);
	Assert::IsTrue(v.blocks.front().ctlr == obj);
	Assert::IsTrue(v.blocks.front().target == obj->target.assigned());
	Assert::IsTrue(v.blocks.front().ctlrIDProperty.get() == &obj->modifierName);
	Assert::IsTrue(v.blocks.front().nodeName == psys->name.get());
	Assert::IsTrue(v.blocks.front().propertyType == "");
	Assert::IsTrue(v.blocks.front().ctlrType == "NiPSysEmitterCtlr");
	Assert::IsTrue(v.blocks.front().ctlrID == obj->modifierName.get());
	Assert::IsTrue(v.blocks.front().iplrID == "BirthRate");

	//Test visibility iplr
	block.iplrID.set("EmitterActive");
	node::AnimationInit<NiPSysEmitterCtlr>{}.up(*obj, v);

	Assert::IsTrue(v.visited.empty());
	Assert::IsTrue(v.blocks.size() == 2);
	Assert::IsTrue(v.blocks.back().ctlr == obj);
	Assert::IsTrue(v.blocks.back().target == obj->target.assigned());
	Assert::IsTrue(v.blocks.back().ctlrIDProperty.get() == &obj->modifierName);
	Assert::IsTrue(v.blocks.back().nodeName == psys->name.get());
	Assert::IsTrue(v.blocks.back().propertyType == "");
	Assert::IsTrue(v.blocks.back().ctlrType == "NiPSysEmitterCtlr");
	Assert::IsTrue(v.blocks.back().ctlrID == obj->modifierName.get());
	Assert::IsTrue(v.blocks.back().iplrID == "EmitterActive");
}*/

void objects::FactoryTest<NiPSysEmitterCtlr>::run()
{
	//Factory behaviour might depend on the attached interpolator. We need to test all cases.

	File file(File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysEmitterCtlr>();
	TestSetup<NiPSysEmitterCtlr>{}.up(*obj, file);

	{//has data
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);
		auto data = file.create<NiFloatData>();
		iplr->data.assign(data);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		FactoryTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);

		Assert::IsTrue(iplr->data.assigned() == data);
	}
	{//no data
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		FactoryTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);
	}
	{//blend
		auto iplr = file.create<NiBlendFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		FactoryTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);
	}
}

void objects::ForwardTest<NiPSysEmitterCtlr>::run()
{
	//Forwarding behaviour might depend on the attached interpolator. We need to test all cases.

	File file(File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysEmitterCtlr>();
	TestSetup<NiPSysEmitterCtlr>{}.up(*obj, file);

	{//has data
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);
		auto data = file.create<NiFloatData>();
		iplr->data.assign(data);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Forwarder<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		ForwardTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);
	}
	{//no data
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Forwarder<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		ForwardTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);
	}
	{//blend
		auto iplr = file.create<NiBlendFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Forwarder<NiPSysEmitterCtlr>{}.up(*obj, ctor);
		ForwardTester<NiPSysEmitterCtlr>{}.up(*obj, ctor);
	}
}


bool objects::TestSetup<NiPSysGravityStrengthCtlr>::operator()(NiPSysGravityStrengthCtlr& obj, File& file)
{
	//Add a target psys and a target modifier
	auto target = file.create<NiParticleSystem>();
	file.getRoot()->children.add(target);
	obj.target.assign(target);

	auto mod0 = file.create<NiPSysModifier>();
	target->modifiers.insert(0, mod0);
	auto mod1 = file.create<NiPSysGravityModifier>();
	target->modifiers.insert(1, mod1);
	auto mod2 = file.create<NiPSysModifier>();
	target->modifiers.insert(2, mod2);

	obj.modifierName.set("bvvarnlier");
	mod1->name.set(obj.modifierName.get());

	auto iplr = file.create<NiFloatInterpolator>();
	obj.interpolator.assign(iplr);

	return false;
}

/*We don't really need a specialised procedure here, leaving it to ModifierCtlr will work fine.
* Only advantage would be that we can repair a broken nif, but we might just as well end up breaking a nif 
* that does something we didn't expect.
void objects::AnimationTester<NiPSysGravityStrengthCtlr>::operator()(const NiPSysGravityStrengthCtlr& obj, MockAnimationManager& visitor)
{
	ni_ptr<NiParticleSystem> psys = std::static_pointer_cast<NiParticleSystem>(obj.target.assigned());

	Assert::IsTrue(visitor.visited.empty());
	Assert::IsTrue(visitor.blocks.size() == 1);
	Assert::IsTrue(visitor.blocks.front().ctlr.get() == &obj);
	Assert::IsTrue(visitor.blocks.front().target == psys);
	Assert::IsTrue(visitor.blocks.front().ctlrIDProperty.get() == &obj.modifierName);
	Assert::IsTrue(visitor.blocks.front().nodeName == psys->name.get());
	Assert::IsTrue(visitor.blocks.front().propertyType == "");
	Assert::IsTrue(visitor.blocks.front().ctlrType == "NiPSysGravityStrengthCtlr");
	Assert::IsTrue(visitor.blocks.front().ctlrID == obj.modifierName.get());
	Assert::IsTrue(visitor.blocks.front().iplrID == "");
}*/

bool objects::ConnectorTester<NiPSysGravityStrengthCtlr>::operator()(const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor)
{
	auto target = std::static_pointer_cast<NiParticleSystem>(obj.target.assigned());
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == obj.interpolator.assigned().get());
	Assert::IsTrue(ctor.connections[0].field1 == node::FloatController::TARGET);
	Assert::IsTrue(ctor.connections[0].object2 == target->modifiers.at(1).get());
	Assert::IsTrue(ctor.connections[0].field2 == node::GravityModifier::STRENGTH);

	//map us to our interpolator (should be inherited from NiPSysModifierCtlr?)
	Assert::IsTrue(ctor.getController(obj.interpolator.assigned().get()) == &obj);

	return false;
}

bool objects::FactoryTester<NiPSysGravityStrengthCtlr>::operator()(const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor)
{
	//auto&& iplr = obj.interpolator.assigned(); 
	//if (ni_type type = iplr->type(); type == NiBlendFloatInterpolator::TYPE)
	//	nodeTest<node::NLFloatController>(*obj.interpolator.assigned(), ctor);
	//else
	//	nodeTest<node::FloatController>(*obj.interpolator.assigned(), ctor);
	Assert::IsTrue(!ctor.node.second);
	return false;
}

bool objects::ForwardTester<NiPSysGravityStrengthCtlr>::operator()(const NiPSysGravityStrengthCtlr& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.size() == 1);
	Assert::IsTrue(ctor.forwards.front() == obj.interpolator.assigned().get());
	return false;
}

//This should be reusable for any controllable property?
void objects::FactoryTest<NiPSysGravityStrengthCtlr>::run()
{
	File file(File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysGravityStrengthCtlr>();
	TestSetup<NiPSysGravityStrengthCtlr>{}.up(*obj, file);

	{//normal
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);
		auto data = file.create<NiFloatData>();
		iplr->data.assign(data);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
		FactoryTester<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);

		Assert::IsTrue(iplr->data.assigned() == data);
	}
	{//blend
		auto iplr = file.create<NiBlendFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Factory<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
		FactoryTester<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
	}
}
void objects::ForwardTest<NiPSysGravityStrengthCtlr>::run()
{
	File file(File::Version::SKYRIM_SE);
	auto obj = file.create<NiPSysGravityStrengthCtlr>();
	TestSetup<NiPSysGravityStrengthCtlr>{}.up(*obj, file);

	{//normal
		auto iplr = file.create<NiFloatInterpolator>();
		obj->interpolator.assign(iplr);
		auto data = file.create<NiFloatData>();
		iplr->data.assign(data);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Forwarder<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
		ForwardTester<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);

		Assert::IsTrue(iplr->data.assigned() == data);
	}
	{//blend
		auto iplr = file.create<NiBlendFloatInterpolator>();
		obj->interpolator.assign(iplr);

		TestConstructor ctor(file);
		ctor.pushObject(obj);
		node::Forwarder<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
		ForwardTester<NiPSysGravityStrengthCtlr>{}.up(*obj, ctor);
	}
}
