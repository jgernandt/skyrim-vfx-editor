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
#include "Composition.h"
#include "SetProperty.h"

namespace gui
{
	//We might want to move some functionality (like size and position) of IComponent into a Widget subclass.
	//Then again, we might not. Let's consider it.

	//Meanwhile, let's just use this header as a general dump for temporary fixes and vague ideas.

	class Popup;//for setContextMenu

	constexpr unsigned int WIDGET_ACTIVATED = 1;
	constexpr unsigned int WIDGET_EDITED = 2;
	constexpr unsigned int WIDGET_RELEASED = 4;

	//Implemented in ColourInput
	float getDefaultHeight();//scaled with the ui
	float getDefaultHeightConst();//not scaled
}