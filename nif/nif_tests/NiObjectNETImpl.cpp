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
#include "EquivalenceTester.h"
#include "ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;


bool common::ForwardOrderTester<NiObject>::operator()(const NiObject& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	//Check for self traversal
	Assert::IsTrue(it != end);
	Assert::IsTrue(*it == &object);
	++it;

	return true;
}


bool common::EquivalenceTester<NiObjectNET>::operator()(const NiObjectNET& object, const Niflib::NiObjectNET* native, File& file)
{
	Assert::IsTrue(native->GetName() == object.name.get());

	auto&& extraData = native->GetExtraData();
	Assert::IsTrue(extraData.size() == object.extraData.size());
	for (auto&& data : extraData)
		Assert::IsTrue(object.extraData.has(file.get<NiExtraData>(data).get()));

	auto&& controllers = native->GetControllers();
	Assert::IsTrue(controllers.size() == object.controllers.size());
	int i = 0;
	for (auto&& controller : controllers)
		Assert::IsTrue(controller == file.getNative<NiTimeController>(object.controllers.at(i++).get()));

	return true;
}

bool common::ForwardOrderTester<NiObjectNET>::operator()(const NiObjectNET& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdSet(object.extraData, it, end);
	fwdSequence(object.controllers, it, end);

	return true;
}

bool common::Randomiser<NiObjectNET>::operator()(NiObjectNET& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.name, rng);
	randomiseSet(object.extraData, file, rng);
	randomiseSequence(object.controllers, file, rng);

	return true;
}

bool common::Randomiser<NiObjectNET>::operator()(const NiObjectNET&, Niflib::NiObjectNET* native, File&, std::mt19937& rng)
{
	native->SetName(rands(rng));

	native->ClearExtraData();
	for (auto&& obj : randomObjVector<NiExtraData>(rng))
		native->AddExtraData(obj);

	native->ClearControllers();
	for (auto&& obj : randomObjVector<NiTimeController>(rng))
		native->AddController(obj);

	return true;
}
