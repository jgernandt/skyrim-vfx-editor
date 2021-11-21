#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;


void common::ForwardOrderTester<NiObject>::operator()(const NiObject& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	//Check for self traversal
	Assert::IsTrue(it != end);
	Assert::IsTrue(*it == &object);
	++it;
}


void common::EquivalenceTester<NiObjectNET>::operator()(const NiObjectNET& object, const Niflib::NiObjectNET* native, File& file)
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
		Assert::IsTrue(controller == file.getNative<NiTimeController>(object.controllers.at(i++)));
}

void common::ForwardOrderTester<NiObjectNET>::operator()(const NiObjectNET& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	//Controllers should be traversed from first to last
	for (auto&& controller : object.controllers) {
		Assert::IsTrue(it != end);
		Assert::IsTrue(*it == controller);
		++it;
	}

	//Order of traversal of a set is not specified
	for (size_t i = 0; i < object.extraData.size(); i++) {
		Assert::IsTrue(it != end);
		//We don't need to check the type, we're just looking at the address
		Assert::IsTrue(object.extraData.has(static_cast<NiExtraData*>(*it)));
		++it;
	}
}

void common::Randomiser<NiObjectNET>::operator()(NiObjectNET& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.name, rng);
	randomiseSet(object.extraData, file, rng);
	randomiseSequence(object.controllers, file, rng);
}

void common::Randomiser<NiObjectNET>::operator()(const NiObjectNET&, Niflib::NiObjectNET* native, File&, std::mt19937& rng)
{
	native->SetName(rands(rng));

	native->ClearExtraData();
	for (auto&& obj : randomObjVector<NiExtraData>(rng))
		native->AddExtraData(obj);

	native->ClearControllers();
	for (auto&& obj : randomObjVector<NiTimeController>(rng))
		native->AddController(obj);
}
