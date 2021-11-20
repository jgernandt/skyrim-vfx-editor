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
#include "NiExtraData.h"
#include "File.h"

const size_t nif::NiExtraData::TYPE = std::hash<std::string>{}("NiExtraData");
const size_t nif::NiStringExtraData::TYPE = std::hash<std::string>{}("NiStringExtraData");

void nif::ReadSyncer<nif::NiExtraData>::operator()(NiExtraData& object, const Niflib::NiExtraData* native, File& file)
{
	assert(native);
	object.name.set(native->GetName());
}

void nif::WriteSyncer<nif::NiExtraData>::operator()(const NiExtraData& object, Niflib::NiExtraData* native, const File& file)
{
	assert(native);
	native->SetName(object.name.get());
}

void nif::ReadSyncer<nif::NiStringExtraData>::operator()(NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file)
{
	assert(native);
	object.value.set(native->GetData());
}

void nif::WriteSyncer<nif::NiStringExtraData>::operator()(const NiStringExtraData& object, Niflib::NiStringExtraData* native, const File& file)
{
	assert(native);
	native->SetData(object.value.get());
}
