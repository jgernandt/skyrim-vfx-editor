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

bool common::EquivalenceTester<nif::NiAlphaProperty>::operator()(const NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file)
{
	Assert::IsTrue(static_cast<unsigned short>(object.mode.get()) == (native->GetFlags() & 0x201));
	Assert::IsTrue(object.srcFcn.get() == static_cast<BlendFunction>(native->GetSourceBlendFunc()));
	Assert::IsTrue(object.dstFcn.get() == static_cast<BlendFunction>(native->GetDestBlendFunc()));
	Assert::IsTrue(object.testFcn.get() == static_cast<TestFunction>(native->GetTestFunc()));
	Assert::IsTrue(object.noSorting.get() == native->GetTriangleSortMode());
	Assert::IsTrue(object.threshold.get() == native->GetTestThreshold());

	return true;
}

bool common::Randomiser<NiAlphaProperty>::operator()(NiAlphaProperty& object, File& file, std::mt19937& rng)
{
	object.mode.set(static_cast<AlphaMode>(randi<std::uint_fast16_t>(rng) & 0x201));
	object.srcFcn.set(static_cast<BlendFunction>(randi<int>(rng, { 0, 10 })));
	object.dstFcn.set(static_cast<BlendFunction>(randi<int>(rng, { 0, 10 })));
	object.testFcn.set(static_cast<TestFunction>(randi<int>(rng, { 0, 7 })));
	object.noSorting.set(randi<int>(rng, { 0, 1 }));
	object.threshold.set(randi<int>(rng, { 0, 255 }));

	return true;
}

bool common::Randomiser<NiAlphaProperty>::operator()(const NiAlphaProperty&, Niflib::NiAlphaProperty* native, File&, std::mt19937& rng)
{
	native->SetFlags(randi<unsigned short>(rng));
	native->SetTestThreshold(randi<int>(rng, { 0, 255 }));

	return true;
}

bool common::EquivalenceTester<nif::BSEffectShaderProperty>::operator()(const BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file)
{
	Assert::IsTrue(object.emissiveCol.get() == nif_type_conversion<ColRGBA>::from(native->GetEmissiveColor()));
	Assert::IsTrue(object.emissiveMult.get() == native->GetEmissiveMultiple());
	Assert::IsTrue(object.sourceTex.get() == native->GetSourceTexture());
	Assert::IsTrue(object.greyscaleTex.get() == native->GetGreyscaleTexture());
	Assert::IsTrue(object.shaderFlags1.raised() == native->GetShaderFlags1());
	Assert::IsTrue(object.shaderFlags2.raised() == native->GetShaderFlags2());

	return true;
}

bool common::Randomiser<BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.emissiveCol, rng);
	randomiseProperty(object.emissiveMult, rng);
	randomiseProperty(object.sourceTex, rng);
	randomiseProperty(object.greyscaleTex, rng);
	randomiseFlags(object.shaderFlags1, rng);
	randomiseFlags(object.shaderFlags2, rng);

	return true;
}

bool common::Randomiser<BSEffectShaderProperty>::operator()(const BSEffectShaderProperty&, Niflib::BSEffectShaderProperty* native, File&, std::mt19937& rng)
{
	native->SetEmissiveColor(randf<Niflib::Color4>(rng));
	native->SetEmissiveMultiple(randf<float>(rng));
	native->SetSourceTexture(rands(rng));
	native->SetGreyscaleTexture(rands(rng));
	native->SetShaderFlags1(static_cast<Niflib::SkyrimShaderPropertyFlags1>(randi<unsigned int>(rng)));
	native->SetShaderFlags2(static_cast<Niflib::SkyrimShaderPropertyFlags2>(randi<unsigned int>(rng)));

	return true;
}
