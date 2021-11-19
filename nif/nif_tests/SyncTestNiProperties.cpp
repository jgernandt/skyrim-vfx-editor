#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiAlphaProperty>::operator()(NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file)
{
	native->SetFlags(static_cast<unsigned short>(mode));
	native->SetSourceBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(srcFcn));
	native->SetDestBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(dstFcn));
	native->SetTestFunc(static_cast<Niflib::NiAlphaProperty::TestFunc>(testFcn));
	native->SetTestThreshold(threshold);
	native->SetTriangleSortMode(noSorting);
}

void nif_tests::SyncTestTraverser<NiAlphaProperty>::operator()(NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file, int)
{
	test(object, native);

	object.mode.set(mode = AlphaMode::TEST);
	object.srcFcn.set(srcFcn = BlendFunction::ONE);
	object.dstFcn.set(dstFcn = BlendFunction::SRC_ALPHA);
	object.testFcn.set(testFcn = TestFunction::NEVER);
	object.threshold.set(threshold = 21);
	object.noSorting.set(noSorting = !noSorting);
}

void nif_tests::SyncTestTraverser<NiAlphaProperty>::operator()(NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<NiAlphaProperty>::test(NiAlphaProperty& object, Niflib::NiAlphaProperty* native)
{
	Assert::IsTrue(object.mode.get() == mode);
	Assert::IsTrue(object.srcFcn.get() == srcFcn);
	Assert::IsTrue(object.dstFcn.get() == dstFcn);
	Assert::IsTrue(object.testFcn.get() == testFcn);
	Assert::IsTrue(object.threshold.get() == threshold);
	Assert::IsTrue(object.noSorting.get() == noSorting);

	Assert::IsTrue((native->GetFlags() & static_cast<unsigned short>(AlphaMode::BOTH)) == static_cast<unsigned short>(mode));
	Assert::IsTrue(native->GetSourceBlendFunc() == static_cast<Niflib::NiAlphaProperty::BlendFunc>(srcFcn));
	Assert::IsTrue(native->GetDestBlendFunc() == static_cast<Niflib::NiAlphaProperty::BlendFunc>(dstFcn));
	Assert::IsTrue(native->GetTestFunc() == static_cast<Niflib::NiAlphaProperty::TestFunc>(testFcn));
	Assert::IsTrue(native->GetTestThreshold() == threshold);
	Assert::IsTrue(native->GetTriangleSortMode() == noSorting);
}

void nif_tests::SyncTestTraverser<BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file)
{
	emissiveCol.preRead(native, &Niflib::BSEffectShaderProperty::SetEmissiveColor, rng, F);
	emissiveMult.preRead(native, &Niflib::BSEffectShaderProperty::SetEmissiveMultiple, rng, F);
	sourceTex.preRead(native, &Niflib::BSEffectShaderProperty::SetSourceTexture);
	greyscaleTex.preRead(native, &Niflib::BSEffectShaderProperty::SetGreyscaleTexture);
	shaderFlags1.preRead(native, &Niflib::BSEffectShaderProperty::SetShaderFlags1, rng, I);
	shaderFlags2.preRead(native, &Niflib::BSEffectShaderProperty::SetShaderFlags2, rng, I);
}

void nif_tests::SyncTestTraverser<BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file, int)
{
	test(object, native);

	emissiveCol.preWrite(object.emissiveCol, rng, F);
	emissiveMult.preWrite(object.emissiveMult, rng, F);
	sourceTex.preWrite(object.sourceTex);
	greyscaleTex.preWrite(object.greyscaleTex);
	shaderFlags1.preWrite(object.shaderFlags1, rng, I);
	shaderFlags2.preWrite(object.shaderFlags2, rng, I);
}

void nif_tests::SyncTestTraverser<BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<BSEffectShaderProperty>::test(BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native)
{
	emissiveCol.test(object.emissiveCol, native, &Niflib::BSEffectShaderProperty::GetEmissiveColor);
	emissiveMult.test(object.emissiveMult, native, &Niflib::BSEffectShaderProperty::GetEmissiveMultiple);
	sourceTex.test(object.sourceTex, native, &Niflib::BSEffectShaderProperty::GetSourceTexture);
	greyscaleTex.test(object.greyscaleTex, native, &Niflib::BSEffectShaderProperty::GetGreyscaleTexture);
	shaderFlags1.test(object.shaderFlags1, native, &Niflib::BSEffectShaderProperty::GetShaderFlags1);
	shaderFlags2.test(object.shaderFlags2, native, &Niflib::BSEffectShaderProperty::GetShaderFlags2);
}
