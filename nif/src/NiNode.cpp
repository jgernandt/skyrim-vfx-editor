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
#include "NiNode.h"
#include "File.h"

void nif::NiSyncer<nif::NiNode>::syncRead(File& file, NiNode* object, Niflib::NiNode* native)
{
	assert(object && native);

	object->children.clear();
	auto&& children = native->GetChildren();
	for (auto&& child : children)
		object->children.add(file.get<NiAVObject>(child));
}
void nif::NiSyncer<nif::NiNode>::syncWrite(const File& file, NiNode* object, Niflib::NiNode* native)
{
	assert(object && native);

	native->ClearChildren();
	for (auto&& child : object->children)
		native->AddChild(file.get<Niflib::NiAVObject>(child));
}
