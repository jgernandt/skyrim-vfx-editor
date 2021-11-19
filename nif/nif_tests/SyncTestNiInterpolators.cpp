#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiBoolData>::operator()(NiBoolData& object, Niflib::NiBoolData* native, File& file)
{
	keyType.preRead(native, &Niflib::NiBoolData::SetKeyType, rng, I);

	native->GetKeysRef().resize(3);
	for (auto&& key : native->GetKeysRef()) {
		key = { F(rng), static_cast<unsigned char>(B(rng)), static_cast<unsigned char>(B(rng)),
			static_cast<unsigned char>(B(rng)), F(rng), F(rng), F(rng) };
	}
	keys = native->GetKeys();
}

void nif_tests::SyncTestTraverser<NiBoolData>::operator()(NiBoolData& object, Niflib::NiBoolData* native, File& file, int)
{
	test(object, native);

	keyType.preWrite(object.keyType, rng, I);

	object.keys.pop_front();
	object.keys.push_back();
	object.keys.back().time.set(F(rng));
	object.keys.back().value.set(B(rng));
	object.keys.back().fwdTan.set(B(rng));
	object.keys.back().bwdTan.set(B(rng));
	object.keys.back().tension.set(F(rng));
	object.keys.back().bias.set(F(rng));
	object.keys.back().continuity.set(F(rng));

	keys.clear();
	for (auto&& key : object.keys) {
		keys.push_back({ key.time.get(), key.value.get(), key.fwdTan.get(), 
			key.bwdTan.get(), key.tension.get(), key.bias.get(), key.continuity.get() });
	}
}

void nif_tests::SyncTestTraverser<NiBoolData>::operator()(NiBoolData& object, Niflib::NiBoolData* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<NiBoolData>::test(NiBoolData& object, Niflib::NiBoolData* native)
{
	keyType.test(object.keyType, native, &Niflib::NiBoolData::GetKeyType);

	Assert::IsTrue(native->GetKeysRef().size() == keys.size());
	Assert::IsTrue(object.keys.size() == keys.size());

	for (int i = 0; (size_t)i < keys.size(); i++) {
		Assert::IsTrue(native->GetKeysRef()[i].time == keys[i].time);
		Assert::IsTrue(native->GetKeysRef()[i].data == keys[i].data);
		Assert::IsTrue(native->GetKeysRef()[i].forward_tangent == keys[i].forward_tangent);
		Assert::IsTrue(native->GetKeysRef()[i].backward_tangent == keys[i].backward_tangent);
		Assert::IsTrue(native->GetKeysRef()[i].tension == keys[i].tension);
		Assert::IsTrue(native->GetKeysRef()[i].bias == keys[i].bias);
		Assert::IsTrue(native->GetKeysRef()[i].continuity == keys[i].continuity);

		Assert::IsTrue(object.keys.at(i).time.get() == keys[i].time);
		Assert::IsTrue(object.keys.at(i).value.get() == (bool)keys[i].data);
		Assert::IsTrue(object.keys.at(i).fwdTan.get() == (bool)keys[i].forward_tangent);
		Assert::IsTrue(object.keys.at(i).bwdTan.get() == (bool)keys[i].backward_tangent);
		Assert::IsTrue(object.keys.at(i).tension.get() == keys[i].tension);
		Assert::IsTrue(object.keys.at(i).bias.get() == keys[i].bias);
		Assert::IsTrue(object.keys.at(i).continuity.get() == keys[i].continuity);
	}
}


void nif_tests::SyncTestTraverser<NiFloatData>::operator()(NiFloatData& object, Niflib::NiFloatData* native, File& file)
{
	keyType.preRead(native, &Niflib::NiFloatData::SetKeyType, rng, I);

	native->GetKeysRef().resize(3);
	for (auto&& key : native->GetKeysRef())
		key = { F(rng), F(rng), F(rng),	F(rng), F(rng), F(rng), F(rng) };
	keys = native->GetKeys();
}

void nif_tests::SyncTestTraverser<NiFloatData>::operator()(NiFloatData& object, Niflib::NiFloatData* native, File& file, int)
{
	test(object, native);

	keyType.preWrite(object.keyType, rng, I);

	object.keys.pop_front();
	object.keys.push_back();
	object.keys.back().time.set(F(rng));
	object.keys.back().value.set(F(rng));
	object.keys.back().fwdTan.set(F(rng));
	object.keys.back().bwdTan.set(F(rng));
	object.keys.back().tension.set(F(rng));
	object.keys.back().bias.set(F(rng));
	object.keys.back().continuity.set(F(rng));

	keys.clear();
	for (auto&& key : object.keys) {
		keys.push_back({ key.time.get(), key.value.get(), key.fwdTan.get(),
			key.bwdTan.get(), key.tension.get(), key.bias.get(), key.continuity.get() });
	}
}

void nif_tests::SyncTestTraverser<NiFloatData>::operator()(NiFloatData& object, Niflib::NiFloatData* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<NiFloatData>::test(NiFloatData& object, Niflib::NiFloatData* native)
{
	keyType.test(object.keyType, native, &Niflib::NiFloatData::GetKeyType);

	Assert::IsTrue(native->GetKeysRef().size() == keys.size());
	Assert::IsTrue(object.keys.size() == keys.size());

	for (int i = 0; (size_t)i < keys.size(); i++) {
		Assert::IsTrue(native->GetKeysRef()[i].time == keys[i].time);
		Assert::IsTrue(native->GetKeysRef()[i].data == keys[i].data);
		Assert::IsTrue(native->GetKeysRef()[i].forward_tangent == keys[i].forward_tangent);
		Assert::IsTrue(native->GetKeysRef()[i].backward_tangent == keys[i].backward_tangent);
		Assert::IsTrue(native->GetKeysRef()[i].tension == keys[i].tension);
		Assert::IsTrue(native->GetKeysRef()[i].bias == keys[i].bias);
		Assert::IsTrue(native->GetKeysRef()[i].continuity == keys[i].continuity);

		Assert::IsTrue(object.keys.at(i).time.get() == keys[i].time);
		Assert::IsTrue(object.keys.at(i).value.get() == keys[i].data);
		Assert::IsTrue(object.keys.at(i).fwdTan.get() == keys[i].forward_tangent);
		Assert::IsTrue(object.keys.at(i).bwdTan.get() == keys[i].backward_tangent);
		Assert::IsTrue(object.keys.at(i).tension.get() == keys[i].tension);
		Assert::IsTrue(object.keys.at(i).bias.get() == keys[i].bias);
		Assert::IsTrue(object.keys.at(i).continuity.get() == keys[i].continuity);
	}
}
