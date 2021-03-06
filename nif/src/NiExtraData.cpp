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

const size_t nif::NiExtraData::TYPE = std::hash<std::string>{}("NiExtraData");
const size_t nif::NiStringExtraData::TYPE = std::hash<std::string>{}("NiStringExtraData");
const size_t nif::NiStringsExtraData::TYPE = std::hash<std::string>{}("NiStringsExtraData");

bool nif::ReadSyncer<nif::NiExtraData>::operator()(NiExtraData& object, const Niflib::NiExtraData* native, File& file)
{
	assert(native);
	object.name.set(native->GetName());
	return true;
}

bool nif::WriteSyncer<nif::NiExtraData>::operator()(const NiExtraData& object, Niflib::NiExtraData* native, const File& file)
{
	assert(native);
	native->SetName(object.name.get());
	return true;
}


bool nif::ReadSyncer<nif::NiStringExtraData>::operator()(NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file)
{
	assert(native);
	object.value.set(native->GetData());
	return true;
}

bool nif::WriteSyncer<nif::NiStringExtraData>::operator()(const NiStringExtraData& object, Niflib::NiStringExtraData* native, const File& file)
{
	assert(native);
	native->SetData(object.value.get());
	return true;
}


bool nif::ReadSyncer<nif::NiStringsExtraData>::operator()(NiStringsExtraData& object, const Niflib::NiStringsExtraData* native, File& file)
{
	assert(native);
	object.strings.clear();
	for (auto&& s : native->GetData()) {
		object.strings.push_back();
		object.strings.back().set(s);
	}
	return true;
}

bool nif::WriteSyncer<nif::NiStringsExtraData>::operator()(const NiStringsExtraData& object, Niflib::NiStringsExtraData* native, const File& file)
{
	assert(native);
	std::vector<std::string> strings(object.strings.size());
	for (size_t i = 0; i < strings.size(); i++)
		strings[i] = object.strings.at(i).get();
	native->SetData(std::move(strings));
	return true;
}
