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

const size_t nif::NiNode::TYPE = std::hash<std::string>{}("NiNode");
const size_t nif::BSFadeNode::TYPE = std::hash<std::string>{}("BSFadeNode");


bool nif::Forwarder<nif::NiNode>::operator()(NiNode& object, NiTraverser& traverser)
{
	for (auto&& child : object.children) {
		assert(child);
		child->receive(traverser);
	}
	return true;
}

bool nif::ReadSyncer<nif::NiNode>::operator()(NiNode& object, const Niflib::NiNode* native, File& file)
{
	assert(native);

	object.children.clear();
	auto&& children = native->GetChildren();
	for (auto&& child : children)
		object.children.add(file.get<NiAVObject>(child));
	return true;
}

bool nif::WriteSyncer<nif::NiNode>::operator()(const NiNode& object, Niflib::NiNode* native, const File& file)
{
	assert(native);

	native->ClearChildren();
	for (auto&& child : object.children)
		native->AddChild(file.getNative<NiAVObject>(child.get()));
	return true;
}
