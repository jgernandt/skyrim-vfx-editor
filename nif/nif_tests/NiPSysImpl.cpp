#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void common::EquivalenceTester<NiParticleSystem>::operator()(const NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file)
{
	Assert::IsTrue(object.data.assigned() == file.get<NiPSysData>(native->GetData()).get());

	auto&& mods = native->GetModifiers();
	Assert::IsTrue(mods.size() == object.modifiers.size());
	for (size_t i = 0; i < mods.size(); i++)
		Assert::IsTrue(object.modifiers.at(i) == file.get<NiPSysModifier>(mods[i]));

	Assert::IsTrue(object.shaderProperty.assigned() == file.get<BSShaderProperty>(native->GetShaderProperty()).get());
	Assert::IsTrue(object.alphaProperty.assigned() == file.get<NiAlphaProperty>(native->GetAlphaProperty()).get());
	Assert::IsTrue(object.worldSpace.get() == native->GetWorldSpace());
}

void common::Randomiser<NiParticleSystem>::operator()(NiParticleSystem& object, File& file, std::mt19937& rng)
{
	object.data.assign(file.create<NiPSysData>());
	randomiseSequence(object.modifiers, file, rng);
	object.shaderProperty.assign(file.create<BSShaderProperty>());
	object.alphaProperty.assign(file.create<NiAlphaProperty>());
	object.worldSpace.set(randi<int>(rng, { 0, 1 }));
}

void common::Randomiser<NiParticleSystem>::operator()(const NiParticleSystem&, Niflib::NiParticleSystem* native, std::mt19937& rng)
{
	native->SetData(new Niflib::NiPSysData);

	auto&& mods = native->GetModifiers();
	mods.clear();
	for (auto&& mod : randomObjVector<NiPSysModifier>(rng))
		mods.push_back(mod);

	native->SetShaderProperty(new Niflib::BSShaderProperty);
	native->SetAlphaProperty(new Niflib::NiAlphaProperty);
	native->SetWorldSpace(randi<int>(rng, { 0, 1 }));
}


void common::EquivalenceTester<NiPSysData>::operator()(const NiPSysData& object, const Niflib::NiPSysData* native, File& file)
{
	Assert::IsTrue(object.maxCount.get() == native->GetBSMaxVertices());

	auto&& offsets1 = object.subtexOffsets.get();
	auto&& offsets2 = native->GetSubtextureOffsets();
	Assert::IsTrue(offsets1.size() == offsets2.size());
	for (size_t i = 0; i < offsets1.size(); i++)
		Assert::IsTrue(offsets1[i] == nif_type_conversion<SubtextureOffset>::from(offsets2[i]));

	Assert::IsTrue(object.hasColour.get() == native->GetHasVertexColors());
	Assert::IsTrue(object.hasRotationAngles.get() == native->GetHasRotationAngles());
	Assert::IsTrue(object.hasRotationSpeeds.get() == native->GetHasRotationSpeeds());
}

void common::Randomiser<NiPSysData>::operator()(NiPSysData& object, File& file, std::mt19937& rng)
{
	object.maxCount.set(randi<unsigned short>(rng));
	object.subtexOffsets.set(randfv<SubtextureOffset>(rng));
	object.hasColour.set(randi<int>(rng, { 0, 1 }));
	object.hasRotationAngles.set(randi<int>(rng, { 0, 1 }));
	object.hasRotationSpeeds.set(randi<int>(rng, { 0, 1 }));
}

void common::Randomiser<NiPSysData>::operator()(const NiPSysData&, Niflib::NiPSysData* native, std::mt19937& rng)
{
	native->SetBSMaxVertices(randi<unsigned short>(rng));
	native->GetSubtextureOffsets() = randfv<Niflib::Vector4>(rng);
	native->SetHasVertexColors(randi<int>(rng, { 0, 1 }));
	native->SetHasRotationAngles(randi<int>(rng, { 0, 1 }));
	native->SetHasRotationSpeeds(randi<int>(rng, { 0, 1 }));
}
