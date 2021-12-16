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

bool common::EquivalenceTester<nif::NiBoolData>::operator()(const NiBoolData& object, const Niflib::NiBoolData* native, File& file)
{
	Assert::IsTrue(object.keyType.get() == nif_type_conversion<KeyType>::from(native->GetKeyType()));

	auto&& keys = native->GetKeys();
	Assert::IsTrue(object.keys.size() == keys.size());
	for (size_t i = 0; i < keys.size(); i++) {
		Assert::IsTrue(object.keys.at(i).time.get() == keys[i].time);
		Assert::IsTrue(object.keys.at(i).value.get() == (bool)keys[i].data);
		Assert::IsTrue(object.keys.at(i).fwdTan.get() == (bool)keys[i].forward_tangent);
		Assert::IsTrue(object.keys.at(i).bwdTan.get() == (bool)keys[i].backward_tangent);
		Assert::IsTrue(object.keys.at(i).tension.get() == keys[i].tension);
		Assert::IsTrue(object.keys.at(i).bias.get() == keys[i].bias);
		Assert::IsTrue(object.keys.at(i).continuity.get() == keys[i].continuity);
	}

	return true;
}

bool common::Randomiser<NiBoolData>::operator()(NiBoolData& object, File& file, std::mt19937& rng)
{
	object.keyType.set(randi<int>(rng, { 1, 5 }));

	std::uniform_int_distribution<int> size(1, 5);
	std::uniform_real_distribution<float> F;
	std::uniform_int_distribution<int> B{ 0, 1 };

	object.keys.resize(size(rng));
	for (auto&& key : object.keys) {
		key.time.set(F(rng));
		key.value.set(B(rng));
		key.fwdTan.set(B(rng));
		key.bwdTan.set(B(rng));
		key.tension.set(F(rng));
		key.bias.set(F(rng));
		key.continuity.set(F(rng));
	}

	return true;
}

bool common::Randomiser<NiBoolData>::operator()(const NiBoolData&, Niflib::NiBoolData* native, File&, std::mt19937& rng)
{
	native->SetKeyType(static_cast<Niflib::KeyType>(randi<int>(rng, { 1, 5 })));

	std::uniform_int_distribution<int> size(1, 5);
	std::uniform_real_distribution<float> F;
	std::uniform_int_distribution<int> B{ 0, 1 };

	native->GetKeysRef().resize(size(rng));
	for (auto&& key : native->GetKeysRef()) {
		key = { F(rng), static_cast<unsigned char>(B(rng)), static_cast<unsigned char>(B(rng)),
			static_cast<unsigned char>(B(rng)), F(rng), F(rng), F(rng) };
	}

	return true;
}

bool common::EquivalenceTester<nif::NiFloatData>::operator()(const NiFloatData& object, const Niflib::NiFloatData* native, File& file)
{
	Assert::IsTrue(object.keyType.get() == nif_type_conversion<KeyType>::from(native->GetKeyType()));

	auto&& keys = native->GetKeys();
	Assert::IsTrue(object.keys.size() == keys.size());
	for (size_t i = 0; i < keys.size(); i++) {
		Assert::IsTrue(object.keys.at(i).time.get() == keys[i].time);
		Assert::IsTrue(object.keys.at(i).value.get() == keys[i].data);
		Assert::IsTrue(object.keys.at(i).fwdTan.get() == keys[i].forward_tangent);
		Assert::IsTrue(object.keys.at(i).bwdTan.get() == keys[i].backward_tangent);
		Assert::IsTrue(object.keys.at(i).tension.get() == keys[i].tension);
		Assert::IsTrue(object.keys.at(i).bias.get() == keys[i].bias);
		Assert::IsTrue(object.keys.at(i).continuity.get() == keys[i].continuity);
	}

	return true;
}

bool common::Randomiser<NiFloatData>::operator()(NiFloatData& object, File& file, std::mt19937& rng)
{
	object.keyType.set(randi<int>(rng, { 1, 5 }));

	std::uniform_int_distribution<int> size(1, 5);
	std::uniform_real_distribution<float> F;

	object.keys.resize(size(rng));
	for (auto&& key : object.keys) {
		key.time.set(F(rng));
		key.value.set(F(rng));
		key.fwdTan.set(F(rng));
		key.bwdTan.set(F(rng));
		key.tension.set(F(rng));
		key.bias.set(F(rng));
		key.continuity.set(F(rng));
	}

	return true;
}

bool common::Randomiser<NiFloatData>::operator()(const NiFloatData&, Niflib::NiFloatData* native, File&, std::mt19937& rng)
{
	native->SetKeyType(static_cast<Niflib::KeyType>(randi<int>(rng, { 1, 5 })));

	std::uniform_int_distribution<int> size(1, 5);
	std::uniform_real_distribution<float> F;

	native->GetKeysRef().resize(size(rng));
	for (auto&& key : native->GetKeysRef()) {
		key = { F(rng), F(rng), F(rng),	F(rng), F(rng), F(rng), F(rng) };
	}

	return true;
}


bool common::EquivalenceTester<NiBoolInterpolator>::operator()(const NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file)
{
	Assert::IsTrue(object.value.get() == native->GetBoolValue());
	Assert::IsTrue(object.data.assigned() == file.get<NiBoolData>(native->GetData()).get());

	return true;
}

bool common::ForwardOrderTester<NiBoolInterpolator>::operator()(
	const NiBoolInterpolator& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdRef(object.data, it, end);

	return true;
}

bool common::Randomiser<NiBoolInterpolator>::operator()(NiBoolInterpolator& object, File& file, std::mt19937& rng)
{
	object.value.set(randi<int>(rng, { 0, 1 }));
	object.data.assign(file.create<NiBoolData>());

	return true;
}

bool common::Randomiser<NiBoolInterpolator>::operator()(const NiBoolInterpolator&, Niflib::NiBoolInterpolator* native, File&, std::mt19937& rng)
{
	native->SetBoolValue(randi<int>(rng, { 0, 1 }));
	native->SetData(new Niflib::NiBoolData);

	return true;
}


bool common::EquivalenceTester<NiFloatInterpolator>::operator()(const NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file)
{
	Assert::IsTrue(object.value.get() == native->GetFloatValue());
	Assert::IsTrue(object.data.assigned() == file.get<NiFloatData>(native->GetData()).get());

	return true;
}

bool common::ForwardOrderTester<NiFloatInterpolator>::operator()(
	const NiFloatInterpolator& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdRef(object.data, it, end);

	return true;
}

bool common::Randomiser<NiFloatInterpolator>::operator()(NiFloatInterpolator& object, File& file, std::mt19937& rng)
{
	object.value.set(randf<float>(rng));
	object.data.assign(file.create<NiFloatData>());

	return true;
}

bool common::Randomiser<NiFloatInterpolator>::operator()(const NiFloatInterpolator&, Niflib::NiFloatInterpolator* native, File&, std::mt19937& rng)
{
	native->SetFloatValue(randf<float>(rng));
	native->SetData(new Niflib::NiFloatData);

	return true;
}


bool common::EquivalenceTester<NiTimeController>::operator()(const NiTimeController& object, const Niflib::NiTimeController* native, File& file)
{
	Assert::IsTrue(object.flags.raised() == native->GetFlags());
	Assert::IsTrue(object.frequency.get() == native->GetFrequency());
	Assert::IsTrue(object.phase.get() == native->GetPhase());
	Assert::IsTrue(object.startTime.get() == native->GetStartTime());
	Assert::IsTrue(object.stopTime.get() == native->GetStopTime());
	Assert::IsTrue(object.target.assigned() == file.get<NiObjectNET>(native->GetTarget()));

	return true;
}

bool common::Randomiser<NiTimeController>::operator()(NiTimeController& object, File& file, std::mt19937& rng)
{
	randomiseFlags(object.flags, rng, { 0, std::numeric_limits<unsigned short>::max() });
	randomiseProperty(object.frequency, rng);
	randomiseProperty(object.phase, rng);
	randomiseProperty(object.startTime, rng);
	randomiseProperty(object.stopTime, rng);

	auto target = file.create<NiAVObject>();
	file.getRoot()->children.add(target);
	object.target.assign(target);

	return true;
}

bool common::Randomiser<NiTimeController>::operator()(const NiTimeController&, Niflib::NiTimeController* native, File& file, std::mt19937& rng)
{
	native->SetFlags(randi<unsigned short>(rng));
	native->SetFrequency(randf<float>(rng));
	native->SetPhase(randf<float>(rng));
	native->SetStartTime(randf<float>(rng));
	native->SetStopTime(randf<float>(rng));
	//weak ref
	Niflib::Ref<Niflib::NiAVObject> native_target = new Niflib::NiAVObject;
	file.getNative<NiNode>(file.getRoot().get())->AddChild(native_target);
	native->SetTarget(native_target);
	auto target = file.get<NiAVObject>(native_target);
	file.getRoot()->children.add(target);

	return true;
}


bool common::EquivalenceTester<NiSingleInterpController>::operator()(const NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file)
{
	Assert::IsTrue(object.interpolator.assigned() == file.get<NiInterpolator>(native->GetInterpolator()).get());

	return true;
}

bool common::ForwardOrderTester<NiSingleInterpController>::operator()(
	const NiSingleInterpController& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdRef(object.interpolator, it, end);

	return true;
}

bool common::Randomiser<NiSingleInterpController>::operator()(NiSingleInterpController& object, File& file, std::mt19937& rng)
{
	object.interpolator.assign(file.create<NiInterpolator>());

	return true;
}

bool common::Randomiser<NiSingleInterpController>::operator()(const NiSingleInterpController&, Niflib::NiSingleInterpController* native, File&, std::mt19937& rng)
{
	native->SetInterpolator(new Niflib::NiInterpolator);

	return true;
}


bool common::EquivalenceTester<NiControllerManager>::operator()(const NiControllerManager& object, const Niflib::NiControllerManager* native, File& file)
{
	Assert::IsTrue(object.cumulative.get() == native->GetCumulative());

	auto&& seqs = native->GetControllerSequences();
	Assert::IsTrue(seqs.size() == object.ctlrSequences.size());
	for (auto&& seq : seqs)
		Assert::IsTrue(object.ctlrSequences.has(file.get<NiControllerSequence>(seq).get()));

	Assert::IsTrue(object.objectPalette.assigned() == file.get<NiDefaultAVObjectPalette>(native->GetObjectPalette()));

	return true;
}

bool common::ForwardOrderTester<NiControllerManager>::operator()(
	const NiControllerManager& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdSet(object.ctlrSequences, it, end);
	fwdRef(object.objectPalette, it, end);
	return true;
}

bool common::Randomiser<NiControllerManager>::operator()(NiControllerManager& object, File& file, std::mt19937& rng)
{
	object.cumulative.set(!object.cumulative.get());
	randomiseSet(object.ctlrSequences, file, rng);
	object.objectPalette.assign(file.create<NiDefaultAVObjectPalette>());
	return true;
}

bool common::Randomiser<NiControllerManager>::operator()(
	const NiControllerManager&, Niflib::NiControllerManager* native, File& file, std::mt19937& rng)
{
	native->SetCumulative(!native->GetCumulative());
	native->SetControllerSequences(randomObjVector<NiControllerSequence>(rng));
	native->SetObjectPalette(new Niflib::NiDefaultAVObjectPalette);
	return true;
}


bool common::EquivalenceTester<NiControllerSequence>::operator()(
	const NiControllerSequence& object, const Niflib::NiControllerSequence* native, File& file)
{
	Assert::IsTrue(object.name.get() == native->GetName());
	Assert::IsTrue(object.arrayGrowBy.get() == native->GetArrayGrowBy());

	auto&& blocks = native->GetControlledBlocks();
	Assert::IsTrue(object.blocks.size() == blocks.size());
	for (size_t i = 0; i < blocks.size(); i++) {
		Assert::IsTrue(object.blocks.at(i).interpolator.assigned() == file.get<NiInterpolator>(blocks[i].interpolator));
		Assert::IsTrue(object.blocks.at(i).controller.assigned() == file.get<NiTimeController>(blocks[i].controller));
		Assert::IsTrue(object.blocks.at(i).priority.get() == blocks[i].priority);
		Assert::IsTrue(object.blocks.at(i).nodeName.get() == blocks[i].nodeName);
		Assert::IsTrue(object.blocks.at(i).propertyType.get() == blocks[i].propertyType);
		Assert::IsTrue(object.blocks.at(i).ctlrType.get() == blocks[i].controllerType);
		Assert::IsTrue(object.blocks.at(i).ctlrID.get() == blocks[i].variable1);
		Assert::IsTrue(object.blocks.at(i).iplrID.get() == blocks[i].variable2);
	}

	Assert::IsTrue(object.weight.get() == native->GetWeight());
	Assert::IsTrue(object.textKeys.assigned() == file.get<NiTextKeyExtraData>(native->GetTextKeyExtraData()));
	Assert::IsTrue(object.cycleType.get() == static_cast<CycleType>(native->GetCycleType()));
	Assert::IsTrue(object.frequency.get() == native->GetFrequency());
	Assert::IsTrue(object.startTime.get() == native->GetStartTime());
	Assert::IsTrue(object.stopTime.get() == native->GetStopTime());
	Assert::IsTrue(object.manager.assigned() == file.get<NiControllerManager>(native->GetParent()));
	Assert::IsTrue(object.accumRootName.get() == native->GetTargetName());

	return true;
}

bool common::ForwardOrderTester<NiControllerSequence>::operator()(
	const NiControllerSequence& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	for (auto&& block : object.blocks) {
		fwdRef(block.interpolator, it, end);
		//I don't think we want to forward to the controller, do we?
	}
	fwdRef(object.textKeys, it, end);
	return true;
}

bool common::Randomiser<NiControllerSequence>::operator()(NiControllerSequence& object, File& file, std::mt19937& rng)
{
	object.name.set(rands(rng));
	randomiseProperty(object.arrayGrowBy, rng);

	std::uniform_int_distribution<int> size(1, 5);
	object.blocks.resize(size(rng));
	for (auto&& block : object.blocks) {
		block.interpolator.assign(file.create<NiInterpolator>());
		block.controller.assign(file.create<NiTimeController>());
		block.priority.set(randi<int>(rng, { 0, 255 }));
		block.nodeName.set(rands(rng));
		block.propertyType.set(rands(rng));
		block.ctlrType.set(rands(rng));
		block.ctlrID.set(rands(rng));
		block.iplrID.set(rands(rng));
	}

	randomiseProperty(object.weight, rng);
	object.textKeys.assign(file.create<NiTextKeyExtraData>());
	object.cycleType.set(static_cast<CycleType>(randi<unsigned int>(rng)));
	randomiseProperty(object.frequency, rng);
	randomiseProperty(object.startTime, rng);
	randomiseProperty(object.stopTime, rng);

	auto manager = file.create<NiControllerManager>();
	file.keepAlive(manager);
	object.manager.assign(manager);

	object.accumRootName.set(rands(rng));

	return true;
}

bool common::Randomiser<NiControllerSequence>::operator()(const NiControllerSequence&, Niflib::NiControllerSequence* native, File& file, std::mt19937& rng)
{
	native->SetName(rands(rng));
	native->SetArrayGrowBy(randi<unsigned int>(rng));

	std::uniform_int_distribution<int> size(1, 5);
	std::vector<Niflib::ControllerLink> blocks(size(rng));
	for (auto&& block : blocks) {
		block.interpolator = new Niflib::NiInterpolator;
		block.controller = new Niflib::NiTimeController;
		block.priority = randi<int>(rng, { 0, 255 });
		block.nodeName = rands(rng);
		block.propertyType = rands(rng);
		block.controllerType = rands(rng);
		block.variable1 = rands(rng);
		block.variable2 = rands(rng);
	}
	native->SetControlledBlocks(std::move(blocks));

	native->SetWeight(randf<float>(rng));
	native->SetTextKey(new Niflib::NiTextKeyExtraData);
	native->SetCycleType(static_cast<Niflib::CycleType>(randi<unsigned int>(rng)));
	native->SetFrequency(randf<float>(rng));
	native->SetStartTime(randf<float>(rng));
	native->SetStopTime(randf<float>(rng));

	Niflib::NiControllerManagerRef native_manager = new Niflib::NiControllerManager;
	auto manager = file.get<NiControllerManager>(native_manager);
	file.keepAlive(manager);
	native_manager->AddSequence(native);//also sets target on us

	native->SetTargetName(rands(rng));

	return true;
}

bool common::EquivalenceTester<NiDefaultAVObjectPalette>::operator()(
	const NiDefaultAVObjectPalette& object, const Niflib::NiDefaultAVObjectPalette* native, File& file)
{
	Assert::IsTrue(object.scene.assigned() == file.get<NiAVObject>(native->GetScene()));
	auto&& objects = native->GetObjects();
	Assert::IsTrue(objects.size() == object.objects.size());
	for (size_t i = 0; i < objects.size(); i++) {
		Assert::IsNotNull(objects[i].avObject);
		Assert::IsTrue(object.objects.at(i).assigned() == file.get<NiAVObject>(objects[i].avObject));
		Assert::IsTrue(objects[i].name == object.objects.at(i).assigned()->name.get());
	}

	return true;
}

bool common::Randomiser<NiDefaultAVObjectPalette>::operator()(NiDefaultAVObjectPalette& object, File& file, std::mt19937& rng)
{
	auto scene = file.create<NiAVObject>();
	file.keepAlive(scene);
	object.scene.assign(scene);

	std::uniform_int_distribution<int> I(3, 6);
	object.objects.resize(I(rng));
	for (auto&& obj : object.objects) {
		auto o = file.create<NiAVObject>();
		file.keepAlive(o);
		obj.assign(o);
		o->name.set(rands(rng));
	}

	return true;
}

bool common::Randomiser<NiDefaultAVObjectPalette>::operator()(const NiDefaultAVObjectPalette&, Niflib::NiDefaultAVObjectPalette* native, File& file, std::mt19937& rng)
{
	Niflib::NiAVObjectRef native_scene = new Niflib::NiAVObject;
	auto scene = file.get<NiAVObject>(native_scene);
	file.keepAlive(scene);
	native->SetScene(native_scene);

	std::uniform_int_distribution<int> I(3, 6);
	std::vector<Niflib::AVObject> vec(I(rng));
	for (auto&& obj : vec) {
		Niflib::NiAVObjectRef native_obj = new Niflib::NiAVObject;
		native_obj->SetName(rands(rng));
		auto o = file.get<NiAVObject>(native_obj);
		file.keepAlive(o);

		obj.name = native_obj->GetName();
		obj.avObject = native_obj;
	}
	native->SetObjects(std::move(vec));

	return true;
}
