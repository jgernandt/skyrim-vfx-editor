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
#include "AnimationTester.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "nodes_internal.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::TestSetup<NiNode>::operator()(NiNode& obj, nif::File& file)
{
	obj.children.clear();
	obj.children.add(file.create<NiAVObject>());
	obj.children.add(file.create<NiAVObject>());

	return false;
}

void objects::AnimationTester<NiNode>::operator()(const NiNode& obj, MockAnimationManager& visitor)
{
	//Should have forwarded to manager only.
	Assert::IsTrue(visitor.visited.size() == 1);
	Assert::IsTrue(visitor.visited.front() == obj.controllers.at(1).get());
	Assert::IsTrue(visitor.blocks.empty());
}

void objects::AnimationTest<NiNode>::run()
{
	nif::File file(nif::File::Version::SKYRIM_SE);
	auto obj = file.create<NiNode>();

	//Including this in TestSetup would mess up the other tests
	obj->controllers.insert(0, file.create<NiTimeController>());
	obj->controllers.insert(1, file.create<NiControllerManager>());
	obj->controllers.insert(2, file.create<NiTimeController>());

	MockAnimationManager v;

	node::AnimationInit<NiNode>{}.up(*obj, v);
	AnimationTester<NiNode>{}(*obj, v);
}

bool objects::ConnectorTester<NiNode>::operator()(const NiNode& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 2);
	for (auto&& info : ctor.connections) {
		Assert::IsTrue(info.object1 == &obj && obj.children.has(static_cast<NiAVObject*>(info.object2)));
		Assert::IsTrue(info.field1 == node::Node::CHILDREN && info.field2 == node::AVObject::PARENT);
	}

	return false;
}

bool objects::FactoryTester<NiNode>::operator()(const NiNode& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.node.first == &obj);
	Assert::IsNotNull(dynamic_cast<node::Node*>(ctor.node.second.get()));
	return false;
}

bool objects::ForwardTester<NiNode>::operator()(const NiNode& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.size() == 2);
	for (NiObject* fwd : ctor.forwards)
		Assert::IsTrue(obj.children.has(static_cast<NiAVObject*>(fwd)));

	return false;
}


bool objects::FactoryTester<NiBillboardNode>::operator()(const NiBillboardNode& obj, const TestConstructor& ctor)
{
	nodeTest<node::BillboardNode>(obj, ctor);
	return false;
}
