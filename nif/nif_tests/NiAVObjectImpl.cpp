#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace math;
using namespace nif;

void common::EquivalenceTester<nif::NiAVObject>::operator()(const NiAVObject& object, const Niflib::NiAVObject* native, File& file)
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
}

void common::Randomiser<NiAVObject>::operator()(NiAVObject& object, File& file, std::mt19937& rng)
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
}

void common::Randomiser<NiAVObject>::operator()(const NiAVObject&, Niflib::NiAVObject* native, File&, std::mt19937& rng)
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
}
