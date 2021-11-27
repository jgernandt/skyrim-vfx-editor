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
	auto target = static_cast<NiParticleSystem*>(obj.target.assigned().get());
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == obj.interpolator.assigned().get());
	Assert::IsTrue(ctor.connections[0].field1 == node::FloatController::TARGET);
	Assert::IsTrue(ctor.connections[0].object2 == target->modifiers.at(1).get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Emitter::BIRTH_RATE);
	return false;
}

bool objects::FactoryTester<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& obj, const TestConstructor& ctor)
{
	if (auto&& iplr = obj.interpolator.assigned()) {
		if (iplr->type() == NiFloatInterpolator::TYPE) {
			if (static_cast<NiFloatInterpolator*>(iplr.get())->data.assigned())
				nodeTest<node::FloatController>(*iplr, ctor);
			else
				Assert::IsTrue(!ctor.node.second);
		}
		else if (iplr->type() == NiBlendFloatInterpolator::TYPE)
			Assert::IsTrue(!ctor.node.second);//might be something else later
		else
			Assert::IsTrue(!ctor.node.second);
	}
	else
		Assert::IsTrue(!ctor.node.second);

	//Later, may also want to look at the visibility interpolator

	return false;
}

void objects::FactoryTest<NiPSysEmitterCtlr>::run()
{
	//Should produce a FloatController if we have an interpolator with data attached. Else, nothing.
	//Should probably produce some other type of controller if we have a BlendFloat attached, but that's for another time.

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
