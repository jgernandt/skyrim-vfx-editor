#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::ConnectorTester<NiExtraData>::operator()(const NiExtraData& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 0);
	return false;
}

bool objects::FactoryTester<NiExtraData>::operator()(const NiExtraData& obj, const TestConstructor& ctor)
{
	//Expect a DummyExtraData
	auto it = ctor.nodes.find(&obj);
	Assert::IsTrue(it != ctor.nodes.end());
	Assert::IsNotNull(dynamic_cast<node::DummyExtraData*>(it->second.get()));
	return false;
}

bool objects::ForwardTester<NiExtraData>::operator()(const NiExtraData& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.size() == 0);
	return false;
}


bool objects::ConnectorTester<NiStringExtraData>::operator()(const NiStringExtraData& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 0);
	return false;
}

void objects::FactoryTest<NiStringExtraData>::run()
{
	File file(File::Version::SKYRIM_SE);

	TestConstructor ctor(file);

	//WeaponType
	auto obj1 = file.create<NiStringExtraData>();
	obj1->name.set("Prn");

	ctor.pushObject(obj1);
	node::Factory<NiStringExtraData>{}.up(*obj1, ctor);
	ctor.popObject();

	auto it = ctor.nodes.find(obj1.get());
	Assert::IsTrue(it != ctor.nodes.end());
	Assert::IsNotNull(dynamic_cast<node::WeaponTypeData*>(it->second.get()));

	//Default
	auto obj2 = file.create<NiStringExtraData>();

	ctor.pushObject(obj2);
	node::Factory<NiStringExtraData>{}.up(*obj2, ctor);
	ctor.popObject();

	it = ctor.nodes.find(obj2.get());
	Assert::IsTrue(it != ctor.nodes.end());
	Assert::IsNotNull(dynamic_cast<node::StringData*>(it->second.get()));
}

bool objects::ForwardTester<NiStringExtraData>::operator()(const NiStringExtraData& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.forwards.size() == 0);
	return false;
}
