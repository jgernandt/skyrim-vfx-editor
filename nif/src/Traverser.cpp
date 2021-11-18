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

//This means we will call the most derived override on every part of the object.
//That may not always be what you want. Maybe you just want the most derived
//override on the most derived part of the object, and then send it on
//to children, extra data, controllers.
//We'll probably want more fine control here, perhaps by separating the forwarding
//behaviour from the calling of derived functions.

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
