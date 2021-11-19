#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiAVObject>::operator()(NiAVObject& object, Niflib::NiAVObject* native, File& file)
{
	flags = I(rng);
	native->SetFlags(flags);

	tr = { F(rng), F(rng), F(rng) };
	native->SetLocalTranslation(tr);

	rot.setEuler({
		F(rng) * 180.0f - 90.0f,
		F(rng) * 180.0f - 90.0f,
		F(rng) * 180.0f - 90.0f,
		math::EulerOrder::XYZ });

	native->SetLocalRotation(nif_type_conversion<Niflib::Matrix33>::from(rot));

	scale = F(rng);
	native->SetLocalScale(scale);
}

void nif_tests::SyncTestTraverser<NiAVObject>::operator()(NiAVObject& object, Niflib::NiAVObject* native, File& file, int)
{
	Assert::IsTrue(object.flags.get() == flags);
	translation_t t = object.transform.translation.get();
	Assert::AreEqual(t[0], tr.x);
	Assert::AreEqual(t[1], tr.y);
	Assert::AreEqual(t[2], tr.z);

	auto q1 = rot.getQuaternion();
	auto q2 = object.transform.rotation.get().getQuaternion();
	Assert::AreEqual(q1.s, q2.s, 1.0e-5f);
	Assert::AreEqual(q1.v[0], q1.v[0], 1.0e-5f);
	Assert::AreEqual(q1.v[1], q1.v[1], 1.0e-5f);
	Assert::AreEqual(q1.v[2], q1.v[2], 1.0e-5f);

	Assert::AreEqual(object.transform.scale.get(), scale);

	flags = I(rng);
	object.flags.clear(-1);
	object.flags.set(flags);

	tr = { F(rng), F(rng), F(rng) };
	object.transform.translation.set(nif_type_conversion<translation_t>::from(tr));

	rot.setEuler({
		F(rng) * 180.0f - 90.0f,
		F(rng) * 180.0f - 90.0f,
		F(rng) * 180.0f - 90.0f,
		math::EulerOrder::XYZ });

	object.transform.rotation.set(rot);

	scale = F(rng);
	object.transform.scale.set(scale);
}

void nif_tests::SyncTestTraverser<NiAVObject>::operator()(NiAVObject& object, Niflib::NiAVObject* native, File& file, int, int)
{
	Assert::IsTrue(native->GetFlags() == flags);

	Assert::IsTrue(native->GetLocalTranslation() == tr);

	auto q1 = rot.getQuaternion();
	auto q2 = nif_type_conversion<rotation_t>::from(native->GetLocalRotation()).getQuaternion();
	Assert::AreEqual(q1.s, q2.s, 1.0e-5f);
	Assert::AreEqual(q1.v[0], q1.v[0], 1.0e-5f);
	Assert::AreEqual(q1.v[1], q1.v[1], 1.0e-5f);
	Assert::AreEqual(q1.v[2], q1.v[2], 1.0e-5f);

	Assert::IsTrue(native->GetLocalScale() == scale);
}
