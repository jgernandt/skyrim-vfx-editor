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

static nif::File::CreateFcn g_NiNodeFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiNode::TYPE), & nif::make_NiObject<nif::NiNode>);

static nif::File::CreateFcn g_BSFadeNodeFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::BSFadeNode::TYPE), & nif::make_NiObject<nif::BSFadeNode>);


void nif::NiSyncer<nif::NiNode>::syncReadImpl(File& file, NiNode* object, Niflib::NiNode* native) const
{
	//TODO
	//This belongs higher up!
	//static int count = 0;
	//name.set("Node" + std::to_string(++count));
	//native().SetFlags(14);
}
void nif::NiSyncer<nif::NiNode>::syncWriteImpl(const File& file, NiNode* object, Niflib::NiNode* native) const
{
	//TODO
}
