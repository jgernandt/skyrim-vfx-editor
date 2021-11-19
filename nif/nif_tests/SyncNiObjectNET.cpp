#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiObjectNET>::operator()(NiObjectNET& object, Niflib::NiObjectNET* native, File& file)
{
	native_data0 = new Niflib::NiExtraData;
	native_data1 = new Niflib::NiExtraData;
	native_data2 = new Niflib::NiExtraData;
	data0 = file.get<NiExtraData>(native_data0);
	data1 = file.get<NiExtraData>(native_data1);
	data2 = file.get<NiExtraData>(native_data2);

	native_ctlr0 = new Niflib::NiTimeController;
	native_ctlr1 = new Niflib::NiTimeController;
	native_ctlr2 = new Niflib::NiTimeController;
	ctlr0 = file.get<NiTimeController>(native_ctlr0);
	ctlr1 = file.get<NiTimeController>(native_ctlr1);
	ctlr2 = file.get<NiTimeController>(native_ctlr2);

	name = "gnosrihgnjsoeir";
	native->SetName(name);

	native->AddExtraData(native_data0);
	native->AddExtraData(native_data1);
	native->AddExtraData(native_data2);

	//pushes to front
	native->AddController(native_ctlr2);
	native->AddController(native_ctlr1);
	native->AddController(native_ctlr0);
}

void nif_tests::SyncTestTraverser<NiObjectNET>::operator()(NiObjectNET& object, Niflib::NiObjectNET* native, File& file, int)
{
	Assert::IsTrue(object.name.get() == name);

	Assert::IsTrue(object.extraData.size() == 3);
	Assert::IsTrue(object.extraData.has(data0.get()));
	Assert::IsTrue(object.extraData.has(data1.get()));
	Assert::IsTrue(object.extraData.has(data2.get()));

	Assert::IsTrue(object.controllers.size() == 3);
	Assert::IsTrue(object.controllers.find(ctlr0.get()) == 0);
	Assert::IsTrue(object.controllers.find(ctlr1.get()) == 1);
	Assert::IsTrue(object.controllers.find(ctlr2.get()) == 2);

	name = "oeasiugershg";
	object.name.set(name);
	object.extraData.remove(data0.get());
	object.controllers.erase(1);
}

void nif_tests::SyncTestTraverser<NiObjectNET>::operator()(NiObjectNET& object, Niflib::NiObjectNET* native, File& file, int, int)
{
	Assert::IsTrue(native->GetName() == name);

	auto extraData = native->GetExtraData();
	Assert::IsTrue(extraData.size() == 2);
	Assert::IsTrue(extraData.front() == native_data1 || extraData.front() == native_data2);
	Assert::IsTrue(extraData.back() == native_data1 || extraData.back() == native_data2);

	auto controllers = native->GetControllers();
	Assert::IsTrue(controllers.size() == 2);
	Assert::IsTrue(controllers.front() == native_ctlr0);
	Assert::IsTrue(controllers.back() == native_ctlr2);
}
