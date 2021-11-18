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
#include "Traverser.h"

void nif::NiTraverser::traverse(NiObject&)
{
}

void nif::NiTraverser::traverse(NiObjectNET& obj)
{
	traverse(static_cast<NiObject&>(obj));
	//extra data
	//controllers
}

void nif::NiTraverser::traverse(NiAVObject& obj)
{
	traverse(static_cast<NiObjectNET&>(obj));
}

void nif::NiTraverser::traverse(NiNode& obj)
{
	traverse(static_cast<NiAVObject&>(obj));
	//children
}
