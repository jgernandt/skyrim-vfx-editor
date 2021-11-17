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
#include "BSEffectShaderProperty.h"
#include "File.h"

static nif::File::CreateFcn g_NiAlphaPropertyFactory = 
	nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiAlphaProperty::TYPE), &nif::make_NiObject<nif::NiAlphaProperty>);

static nif::File::CreateFcn g_BSEffectShaderPropertyFactory =
	nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::BSEffectShaderProperty::TYPE), & nif::make_NiObject<nif::BSEffectShaderProperty>);

void nif::NiSyncer<nif::NiAlphaProperty>::syncReadImpl(
	File& file, NiAlphaProperty* object, Niflib::NiAlphaProperty* native) const
{
	assert(object && native);
	object->mode.set(static_cast<AlphaMode>(native->GetFlags() & 0x201));
	object->srcFcn.set(static_cast<BlendFunction>(native->GetSourceBlendFunc()));
	object->dstFcn.set(static_cast<BlendFunction>(native->GetDestBlendFunc()));
	object->testFcn.set(static_cast<TestFunction>(native->GetTestFunc()));
	object->threshold.set(native->GetTestThreshold());
	object->sorting.set(!native->GetTriangleSortMode());
}

void nif::NiSyncer<nif::NiAlphaProperty>::syncWriteImpl(
	const File& file, NiAlphaProperty* object, Niflib::NiAlphaProperty* native) const
{
	assert(object && native);
	native->SetFlags(static_cast<unsigned short>(object->mode.get()));
	native->SetSourceBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(object->srcFcn.get()));
	native->SetDestBlendFunc(static_cast<Niflib::NiAlphaProperty::BlendFunc>(object->dstFcn.get()));
	native->SetTestFunc(static_cast<Niflib::NiAlphaProperty::TestFunc>(object->testFcn.get()));
	native->SetTestThreshold(object->threshold.get());
	native->SetTriangleSortMode(!object->sorting.get());
}


void nif::NiSyncer<nif::BSEffectShaderProperty>::syncReadImpl(
	File& file, BSEffectShaderProperty* object, Niflib::BSEffectShaderProperty* native) const
{
	//TODO
}

void nif::NiSyncer<nif::BSEffectShaderProperty>::syncWriteImpl(
	const File& file, BSEffectShaderProperty* object, Niflib::BSEffectShaderProperty* native) const
{
	//TODO
}
