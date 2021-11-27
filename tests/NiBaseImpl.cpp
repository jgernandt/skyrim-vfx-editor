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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::TestSetup<NiObjectNET>::operator()(NiObjectNET& obj, File& file)
{
	obj.extraData.clear();
	obj.extraData.add(file.create<NiExtraData>());
	obj.extraData.add(file.create<NiExtraData>());

	obj.controllers.clear();
	obj.controllers.insert(0, file.create<NiTimeController>());
	obj.controllers.insert(1, file.create<NiTimeController>());

	return false;
}

bool objects::ConnectorTester<NiObjectNET>::operator()(const NiObjectNET& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 2);
	for (auto&& info : ctor.connections) {
		Assert::IsTrue(info.object1 == &obj && obj.extraData.has(static_cast<NiExtraData*>(info.object2)));
		Assert::IsTrue(info.field1 == node::ObjectNET::EXTRA_DATA && info.field2 == node::ExtraData::TARGET);
	}

	//We should not ask for connections with controllers, since we don't know where they should be connected.
	//They will have to do that themselves.

	return false;
}

bool objects::ForwardTester<NiObjectNET>::operator()(const NiObjectNET& obj, const TestConstructor& ctor)
{
	//Should forward to extra data (unspecified order), then controllers (in order)
	Assert::IsTrue(ctor.forwards.size() == 4);
	Assert::IsTrue(obj.extraData.has(static_cast<NiExtraData*>(ctor.forwards[0])));
	Assert::IsTrue(obj.extraData.has(static_cast<NiExtraData*>(ctor.forwards[1])));
	Assert::IsTrue(ctor.forwards[2] == obj.controllers.at(0).get());
	Assert::IsTrue(ctor.forwards[3] == obj.controllers.at(1).get());

	return false;
}


bool objects::FactoryTester<NiAVObject>::operator()(const NiAVObject& obj, const TestConstructor& ctor)
{
	//Expect a DummyAVObject
	Assert::IsTrue(ctor.node.first == &obj);
	Assert::IsNotNull(dynamic_cast<node::DummyAVObject*>(ctor.node.second.get()));
	return false;
}
