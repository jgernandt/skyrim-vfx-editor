#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::FactoryTester<NiExtraData>::operator()(const NiExtraData& obj, const TestConstructor& ctor)
{
	//Expect a DummyExtraData
	Assert::IsTrue(ctor.node.first == &obj);
	Assert::IsNotNull(dynamic_cast<node::DummyExtraData*>(ctor.node.second.get()));
	return false;
}


void objects::FactoryTest<NiStringExtraData>::run()
{
	File file(File::Version::SKYRIM_SE);

	{//WeaponType
		TestConstructor ctor(file);
		auto obj1 = file.create<NiStringExtraData>();
		obj1->name.set("Prn");

		ctor.pushObject(obj1);
		node::Factory<NiStringExtraData>{}.up(*obj1, ctor);
		ctor.popObject();

		Assert::IsTrue(ctor.node.first == obj1.get());
		Assert::IsNotNull(dynamic_cast<node::WeaponTypeData*>(ctor.node.second.get()));
	}
	{//Default
		TestConstructor ctor(file);
		auto obj2 = file.create<NiStringExtraData>();

		ctor.pushObject(obj2);
		node::Factory<NiStringExtraData>{}.up(*obj2, ctor);
		ctor.popObject();

		Assert::IsTrue(ctor.node.first == obj2.get());
		Assert::IsNotNull(dynamic_cast<node::StringData*>(ctor.node.second.get()));
	}
}
