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
