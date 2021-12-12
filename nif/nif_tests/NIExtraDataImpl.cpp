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
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool common::EquivalenceTester<nif::NiExtraData>::operator()(const NiExtraData& object, const Niflib::NiExtraData* native, File& file)
{
	Assert::IsTrue(object.name.get() == native->GetName());

	return true;
}

bool common::Randomiser<NiExtraData>::operator()(NiExtraData& object, File& file, std::mt19937& rng)
{
	object.name.set(rands(rng));

	return true;
}

bool common::Randomiser<NiExtraData>::operator()(const NiExtraData&, Niflib::NiExtraData* native, File&, std::mt19937& rng)
{
	native->SetName(rands(rng));

	return true;
}

bool common::EquivalenceTester<nif::NiStringExtraData>::operator()(const NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file)
{
	Assert::IsTrue(object.value.get() == native->GetData());

	return true;
}

bool common::Randomiser<NiStringExtraData>::operator()(NiStringExtraData& object, File& file, std::mt19937& rng)
{
	object.value.set(rands(rng));

	return true;
}

bool common::Randomiser<NiStringExtraData>::operator()(const NiStringExtraData&, Niflib::NiStringExtraData* native, File&, std::mt19937& rng)
{
	native->SetData(rands(rng));

	return true;
}


bool common::EquivalenceTester<NiStringsExtraData>::operator()(const NiStringsExtraData& object, const Niflib::NiStringsExtraData* native, File& file)
{
	auto&& nativeStrings = native->GetData();
	Assert::IsTrue(nativeStrings.size() == object.strings.size());
	for (size_t i = 0; i < object.strings.size(); i++)
		Assert::IsTrue(nativeStrings[i] == object.strings.at(i).get());
	return true;
}

bool common::Randomiser<NiStringsExtraData>::operator()(NiStringsExtraData& object, File& file, std::mt19937& rng)
{
	std::uniform_int_distribution<int> I{ 3, 6 };
	object.strings.resize(I(rng));
	for (auto&& s : object.strings)
		s.set(rands(rng));
	return true;
}

bool common::Randomiser<NiStringsExtraData>::operator()(const NiStringsExtraData&, Niflib::NiStringsExtraData* native, File& file, std::mt19937& rng)
{
	std::uniform_int_distribution<int> I{ 3, 6 };
	std::vector<std::string> strings(I(rng));
	for (auto&& s : strings)
		s = rands(rng);
	native->SetData(std::move(strings));

	return true;
}


bool common::EquivalenceTester<NiTextKeyExtraData>::operator()(const NiTextKeyExtraData& object, const Niflib::NiTextKeyExtraData* native, File& file)
{
	auto&& keys = native->GetKeys();
	Assert::IsTrue(keys.size() == object.keys.size());
	for (size_t i = 0; i < keys.size(); i++) {
		Assert::IsTrue(object.keys.at(i).time.get() == keys[i].time);
		Assert::IsTrue(object.keys.at(i).value.get() == keys[i].data);
		Assert::IsTrue(keys[i].forward_tangent == std::string());
		Assert::IsTrue(keys[i].backward_tangent == std::string());
		Assert::IsTrue(keys[i].tension == 0.0f);
		Assert::IsTrue(keys[i].bias == 0.0f);
		Assert::IsTrue(keys[i].continuity == 0.0f);
	}

	return true;
}

bool common::Randomiser<NiTextKeyExtraData>::operator()(NiTextKeyExtraData& object, File& file, std::mt19937& rng)
{
	std::uniform_int_distribution I(3, 6);
	object.keys.resize(I(rng));
	for (auto&& key : object.keys) {
		key.time.set(randf<float>(rng));
		key.value.set(rands(rng));
	}

	return true;
}

bool common::Randomiser<NiTextKeyExtraData>::operator()(
	const NiTextKeyExtraData&, Niflib::NiTextKeyExtraData* native, File& file, std::mt19937& rng)
{
	std::uniform_int_distribution I(3, 6);
	std::vector<Niflib::Key<std::string>> keys(I(rng));
	for (auto&& key : keys) {
		key.time = randf<float>(rng);
		key.data = rands(rng);
	}
	native->SetKeys(keys);

	return true;
}


bool common::EquivalenceTester<BSBehaviorGraphExtraData>::operator()(
	const BSBehaviorGraphExtraData& object, const Niflib::BSBehaviorGraphExtraData* native, File& file)
{
	Assert::IsTrue(object.fileName.get() == native->GetBehaviourGraphFile());
	Assert::IsTrue(object.controlsBaseSkeleton.get() == native->GetControlsBaseSkeleton());
	return true;
}

bool common::Randomiser<BSBehaviorGraphExtraData>::operator()(BSBehaviorGraphExtraData& object, File& file, std::mt19937& rng)
{
	object.fileName.set(rands(rng));
	object.controlsBaseSkeleton.set(!object.controlsBaseSkeleton.get());
	return true;
}

bool common::Randomiser<BSBehaviorGraphExtraData>::operator()(
	const BSBehaviorGraphExtraData&, Niflib::BSBehaviorGraphExtraData* native, File& file, std::mt19937& rng)
{
	native->SetBehaviourGraphFile(rands(rng));
	native->SetControlsBaseSkeleton(!native->GetControlsBaseSkeleton());
	return true;
}
