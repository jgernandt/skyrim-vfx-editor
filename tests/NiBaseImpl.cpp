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
