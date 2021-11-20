#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

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
		Assert::IsTrue(controller == file.get<Niflib::NiTimeController>(object.controllers.at(i++)));
}

void common::Randomiser<NiObjectNET>::operator()(NiObjectNET& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.name, rng);
	randomiseSet(object.extraData, file, rng);
	randomiseSequence(object.controllers, file, rng);
}

void common::Randomiser<NiObjectNET>::operator()(const NiObjectNET&, Niflib::NiObjectNET* native, std::mt19937& rng)
{
	native->SetName(rands(rng));

	native->ClearExtraData();
	for (auto&& obj : randomObjVector<NiExtraData>(rng))
		native->AddExtraData(obj);

	native->ClearControllers();
	for (auto&& obj : randomObjVector<NiTimeController>(rng))
		native->AddController(obj);
}
