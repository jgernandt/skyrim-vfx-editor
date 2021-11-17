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

void nif::NiSyncer<nif::NiExtraData>::syncReadImpl(
	File& file, NiExtraData* object, Niflib::NiExtraData* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiExtraData>::syncWriteImpl(
	const File& file, NiExtraData* object, Niflib::NiExtraData* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiStringExtraData>::syncReadImpl(
	File& file, NiStringExtraData* object, Niflib::NiStringExtraData* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiStringExtraData>::syncWriteImpl(
	const File& file, NiStringExtraData* object, Niflib::NiStringExtraData* native) const
{
	//TODO
}
