#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::TestSetup<NiNode>::operator()(NiNode& obj, nif::File& file)
{
	obj.children.clear();
	obj.children.add(file.create<NiAVObject>());
	obj.children.add(file.create<NiAVObject>());

	return false;
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
