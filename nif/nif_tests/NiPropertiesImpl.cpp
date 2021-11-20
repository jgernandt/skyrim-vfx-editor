#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void common::EquivalenceTester<nif::NiAlphaProperty>::operator()(const NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file)
{
	Assert::IsTrue(static_cast<unsigned short>(object.mode.get()) == (native->GetFlags() & 0x201));
	Assert::IsTrue(object.srcFcn.get() == static_cast<BlendFunction>(native->GetSourceBlendFunc()));
	Assert::IsTrue(object.dstFcn.get() == static_cast<BlendFunction>(native->GetDestBlendFunc()));
	Assert::IsTrue(object.testFcn.get() == static_cast<TestFunction>(native->GetTestFunc()));
	Assert::IsTrue(object.noSorting.get() == native->GetTriangleSortMode());
	Assert::IsTrue(object.threshold.get() == native->GetTestThreshold());
}

void common::Randomiser<NiAlphaProperty>::operator()(NiAlphaProperty& object, File& file, std::mt19937& rng)
{
	object.mode.set(static_cast<AlphaMode>(randi<std::uint_fast16_t>(rng) & 0x201));
	object.srcFcn.set(static_cast<BlendFunction>(randi<int>(rng, { 0, 10 })));
	object.dstFcn.set(static_cast<BlendFunction>(randi<int>(rng, { 0, 10 })));
	object.testFcn.set(static_cast<TestFunction>(randi<int>(rng, { 0, 7 })));
	object.noSorting.set(randi<int>(rng, { 0, 1 }));
	object.threshold.set(randi<int>(rng, { 0, 255 }));
}

void common::Randomiser<NiAlphaProperty>::operator()(const NiAlphaProperty&, Niflib::NiAlphaProperty* native, std::mt19937& rng)
{
	native->SetFlags(randi<unsigned short>(rng));
	native->SetTestThreshold(randi<int>(rng, { 0, 255 }));
}

void common::EquivalenceTester<nif::BSEffectShaderProperty>::operator()(const BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file)
{
	Assert::IsTrue(object.emissiveCol.get() == nif_type_conversion<ColRGBA>::from(native->GetEmissiveColor()));
	Assert::IsTrue(object.emissiveMult.get() == native->GetEmissiveMultiple());
	Assert::IsTrue(object.sourceTex.get() == native->GetSourceTexture());
	Assert::IsTrue(object.greyscaleTex.get() == native->GetGreyscaleTexture());
	Assert::IsTrue(object.shaderFlags1.get() == native->GetShaderFlags1());
	Assert::IsTrue(object.shaderFlags2.get() == native->GetShaderFlags2());
}

void common::Randomiser<BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.emissiveCol, rng);
	randomiseProperty(object.emissiveMult, rng);
	randomiseProperty(object.sourceTex, rng);
	randomiseProperty(object.greyscaleTex, rng);
	randomiseFlags(object.shaderFlags1, rng);
	randomiseFlags(object.shaderFlags2, rng);
}

void common::Randomiser<BSEffectShaderProperty>::operator()(const BSEffectShaderProperty&, Niflib::BSEffectShaderProperty* native, std::mt19937& rng)
{
	native->SetEmissiveColor(randf<Niflib::Color4>(rng));
	native->SetEmissiveMultiple(randf<float>(rng));
	native->SetSourceTexture(rands(rng));
	native->SetGreyscaleTexture(rands(rng));
	native->SetShaderFlags1(static_cast<Niflib::SkyrimShaderPropertyFlags1>(randi<unsigned int>(rng)));
	native->SetShaderFlags2(static_cast<Niflib::SkyrimShaderPropertyFlags2>(randi<unsigned int>(rng)));
}
