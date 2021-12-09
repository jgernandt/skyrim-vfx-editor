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


void objects::FactoryTest<NiStringsExtraData>::run()
{
	File file(File::Version::SKYRIM_SE);

	{//AttachT (bone)
		TestConstructor ctor(file);
		auto obj = file.create<NiStringsExtraData>();
		obj->name.set("AttachT");
		obj->strings.push_back();
		obj->strings.back().set("Bone");

		ctor.pushObject(obj);
		node::Factory<NiStringsExtraData>{}.up(*obj, ctor);
		ctor.popObject();

		nodeTest<node::AttachPointData>(*obj, ctor);
	}
	{//AttachT (MultiTechnique)
		TestConstructor ctor(file);
		auto obj = file.create<NiStringsExtraData>();
		obj->name.set("AttachT");
		obj->strings.push_back();
		obj->strings.back().set("MultiTechnique");

		ctor.pushObject(obj);
		node::Factory<NiStringsExtraData>{}.up(*obj, ctor);
		ctor.popObject();

		Assert::IsTrue(!ctor.node.second);
	}
	{//Other
		TestConstructor ctor(file);
		auto obj = file.create<NiStringsExtraData>();

		ctor.pushObject(obj);
		node::Factory<NiStringsExtraData>{}.up(*obj, ctor);
		ctor.popObject();

		nodeTest<node::DummyExtraData>(*obj, ctor);
	}
}
