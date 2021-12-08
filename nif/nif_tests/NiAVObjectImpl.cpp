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
using namespace math;
using namespace nif;

bool common::EquivalenceTester<nif::NiAVObject>::operator()(const NiAVObject& object, const Niflib::NiAVObject* native, File& file)
{
	Assert::IsTrue(object.flags.raised() == native->GetFlags());
	Assert::IsTrue(object.transform.translation.get() == nif_type_conversion<translation_t>::from(native->GetLocalTranslation()));

	auto q1 = object.transform.rotation.get().getQuaternion();
	auto q2 = nif_type_conversion<rotation_t>::from(native->GetLocalRotation()).getQuaternion();
	Assert::AreEqual(q1.s, q2.s, 1.0e-5f);
	Assert::AreEqual(q1.v[0], q1.v[0], 1.0e-5f);
	Assert::AreEqual(q1.v[1], q1.v[1], 1.0e-5f);
	Assert::AreEqual(q1.v[2], q1.v[2], 1.0e-5f);

	Assert::IsTrue(object.transform.scale.get() == native->GetLocalScale());

	return true;
}

bool common::Randomiser<NiAVObject>::operator()(NiAVObject& object, File& file, std::mt19937& rng)
{
	//flags is 32 bit, but it was 16 in Niflib's days. I can't be bothered to fix that right now.
	randomiseFlags(object.flags, rng, { 0, std::numeric_limits<unsigned short>::max() });
	randomiseProperty(object.transform.translation, rng);

	std::uniform_real_distribution<float> F;
	Rotation rot;
	rot.setEuler({
		degf(F(rng) * 180.0f - 90.0f),
		degf(F(rng) * 180.0f - 90.0f),
		degf(F(rng) * 180.0f - 90.0f),
		EulerOrder::XYZ });

	object.transform.rotation.set(rot);

	randomiseProperty(object.transform.scale, rng);

	return true;
}

bool common::Randomiser<NiAVObject>::operator()(const NiAVObject&, Niflib::NiAVObject* native, File&, std::mt19937& rng)
{
	native->SetFlags(randi<unsigned short>(rng));

	std::uniform_real_distribution<float> F;
	native->SetLocalTranslation({ F(rng), F(rng), F(rng) });

	Rotation rot;
	rot.setEuler({
		degf(F(rng) * 180.0f - 90.0f),
		degf(F(rng) * 180.0f - 90.0f),
		degf(F(rng) * 180.0f - 90.0f),
		EulerOrder::XYZ });

	native->SetLocalRotation(nif_type_conversion<Niflib::Matrix33>::from(rot));

	native->SetLocalScale(F(rng));

	return true;
}
