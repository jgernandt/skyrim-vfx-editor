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
#include "nif_internal.h"

const size_t nif::NiProperty::TYPE = std::hash<std::string>{}("NiProperty");
const size_t nif::NiAlphaProperty::TYPE = std::hash<std::string>{}("NiAlphaProperty");
const size_t nif::BSShaderProperty::TYPE = std::hash<std::string>{}("BSShaderProperty");
const size_t nif::BSEffectShaderProperty::TYPE = std::hash<std::string>{}("BSEffectShaderProperty");

void nif::ReadSyncer<nif::NiAlphaProperty>::operator() (NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file)
{
	assert(native);
	object.mode.set(static_cast<AlphaMode>(native->GetFlags() & 0x201));
	object.srcFcn.set(static_cast<BlendFunction>(native->GetSourceBlendFunc()));
	object.dstFcn.set(static_cast<BlendFunction>(native->GetDestBlendFunc()));
	object.testFcn.set(static_cast<TestFunction>(native->GetTestFunc()));
	object.threshold.set(native->GetTestThreshold());
	object.noSorting.set(native->GetTriangleSortMode());
}

void nif::WriteSyncer<nif::NiAlphaProperty>::operator() (const NiAlphaProperty& object, Niflib::NiAlphaProperty* native, const File& file)
{
	assert(native);
	native->SetFlags(static_cast<unsigned short>(object.mode.get()));
	native->SetSourceBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(object.srcFcn.get()));
	native->SetDestBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(object.dstFcn.get()));
	native->SetTestFunc(static_cast<Niflib::NiAlphaProperty::TestFunc>(object.testFcn.get()));
	native->SetTestThreshold(object.threshold.get());
	native->SetTriangleSortMode(object.noSorting.get());
}

void nif::ReadSyncer<nif::BSEffectShaderProperty>::operator()(BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file)
{
	assert(native);
	object.emissiveCol.set(nif_type_conversion<ColRGBA>::from(native->GetEmissiveColor()));
	object.emissiveMult.set(native->GetEmissiveMultiple());
	object.sourceTex.set(native->GetSourceTexture());
	object.greyscaleTex.set(native->GetGreyscaleTexture());
	object.shaderFlags1.clear();
	object.shaderFlags1.set(nif_type_conversion<ShaderFlags>::from(native->GetShaderFlags1()));
	object.shaderFlags2.clear();
	object.shaderFlags2.set(nif_type_conversion<ShaderFlags>::from(native->GetShaderFlags2()));
}

void nif::WriteSyncer<nif::BSEffectShaderProperty>::operator()(const BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, const File& file)
{
	assert(native);
	native->SetEmissiveColor(nif_type_conversion<Niflib::Color4>::from(object.emissiveCol.get()));
	native->SetEmissiveMultiple(object.emissiveMult.get());
	native->SetSourceTexture(object.sourceTex.get());
	native->SetGreyscaleTexture(object.greyscaleTex.get());
	native->SetShaderFlags1(nif_type_conversion<Niflib::SkyrimShaderPropertyFlags1>::from(object.shaderFlags1.get()));
	native->SetShaderFlags2(nif_type_conversion<Niflib::SkyrimShaderPropertyFlags2>::from(object.shaderFlags2.get()));
}
