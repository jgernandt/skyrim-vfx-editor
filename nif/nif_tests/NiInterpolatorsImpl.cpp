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
