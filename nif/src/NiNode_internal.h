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

#pragma once
#include "NiNode.h"
#include "NiObject_internal.h"

namespace nif
{
	//NiNode
	template<> struct type_map<Niflib::NiNode> { using type = NiNode; };
	template<> struct type_map<NiNode> { using type = Niflib::NiNode; };

	template<> struct ReadSyncer<NiNode> : VerticalTraverser<NiNode, ReadSyncer>
	{
		bool operator() (NiNode& object, const Niflib::NiNode* native, File& file);
	};
	template<> struct WriteSyncer<NiNode> : VerticalTraverser<NiNode, WriteSyncer>
	{
		bool operator() (const NiNode& object, Niflib::NiNode* native, const File& file);
	};


	//NiBillboardNode
	template<> struct type_map<Niflib::NiBillboardNode> { using type = NiBillboardNode; };
	template<> struct type_map<NiBillboardNode> { using type = Niflib::NiBillboardNode; };

	template<> struct ReadSyncer<NiBillboardNode> : VerticalTraverser<NiBillboardNode, ReadSyncer>
	{
		bool operator() (NiBillboardNode& object, const Niflib::NiBillboardNode* native, File& file);
	};
	template<> struct WriteSyncer<NiBillboardNode> : VerticalTraverser<NiBillboardNode, WriteSyncer>
	{
		bool operator() (const NiBillboardNode& object, Niflib::NiBillboardNode* native, const File& file);
	};


	//BSFadeNode
	template<> struct type_map<Niflib::BSFadeNode> { using type = BSFadeNode; };
	template<> struct type_map<BSFadeNode> { using type = Niflib::BSFadeNode; };
}
