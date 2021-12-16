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

bool objects::TestSetup<NiControllerManager>::operator()(NiControllerManager& obj, File& file)
{
	auto root = file.getRoot();
	obj.target.assign(root);

	auto bged = file.create<BSBehaviorGraphExtraData>();
	bged->name.set(node::BGED_NAME);
	root->extraData.add(bged);

	//Two controller sequences with a total of 3 unique controllers
	obj.ctlrSequences.clear();
	auto seq0 = file.create<NiControllerSequence>();
	seq0->blocks.resize(2);
	auto ctlr0 = file.create<NiTimeController>();
	auto ctlr1 = file.create<NiTimeController>();
	seq0->blocks.front().controller.assign(ctlr0);
	seq0->blocks.back().controller.assign(ctlr1);
	obj.ctlrSequences.add(seq0);

	auto seq1 = file.create<NiControllerSequence>();
	seq1->blocks.resize(2);
	auto ctlr2 = file.create<NiTimeController>();
	seq1->blocks.front().controller.assign(ctlr0);
	seq1->blocks.back().controller.assign(ctlr2);
	obj.ctlrSequences.add(seq1);

	return false;
}

void objects::AnimationTester<NiControllerManager>::operator()(const NiControllerManager& obj, MockAnimationManager& visitor)
{
	//Should have forwarded to each unique controller
	Assert::IsTrue(visitor.visited.size() == 3);
	for (auto&& seq : obj.ctlrSequences) {
		Assert::IsTrue(std::find(visitor.visited.begin(), visitor.visited.end(), seq->blocks.front().controller.assigned().get()) != visitor.visited.end());
		Assert::IsTrue(std::find(visitor.visited.begin(), visitor.visited.end(), seq->blocks.back().controller.assigned().get()) != visitor.visited.end());
	}
	Assert::IsTrue(visitor.blocks.empty());
}

bool objects::FactoryTester<NiControllerManager>::operator()(const NiControllerManager& obj, const TestConstructor& ctor)
{
	//we should have a node and should have claimed our BGED
	nodeTest<node::ControllerManager>(obj, ctor);
	auto node = static_cast<node::ControllerManager*>(ctor.node.second.get());
	Assert::IsTrue(obj.target.assigned() && obj.target.assigned()->extraData.has(node->getBGED().get()));

	//we should have picked up the objects in the palette
	//Assert::IsTrue(obj.objectPalette.assigned()->objects.size() == 2);
	//auto av0 = obj.objectPalette.assigned()->objects.front().object.assigned();
	//auto av1 = obj.objectPalette.assigned()->objects.back().object.assigned();
	//Assert::IsNotNull(av0.get());
	//Assert::IsNotNull(av1.get());
	//av0->name.set("agorihaoeri");
	//av1->name.set("sbrieugb");
	//Assert::IsTrue(av0->name.get() == obj.objectPalette.assigned()->objects.front().name.get());
	//Assert::IsTrue(av1->name.get() == obj.objectPalette.assigned()->objects.back().name.get());

	//Do we care to guard against a ControllerManager not attached to the root?
	//I don't think that would interfere with us, so just ignore it.

	return false;
}

bool objects::ConnectorTester<NiControllerManager>::operator()(const NiControllerManager& obj, const TestConstructor& ctor)
{
	//We should connect to the root
	Assert::IsTrue(ctor.connections.size() == 1);
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::ControllerManager::Root::ID);
	Assert::IsTrue(ctor.connections[0].object2 == obj.target.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::Root::Behaviour::ID);
	return false;
}

bool objects::ForwardTester<NiControllerManager>::operator()(const NiControllerManager& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.size() == 2);
	for (NiObject* fwd : ctor.forwards)
		Assert::IsTrue(obj.ctlrSequences.has(static_cast<NiControllerSequence*>(fwd)));

	return false;
}
