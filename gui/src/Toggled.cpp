//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include "Toggled.h"

void gui::Toggled::frame()
{
	if (m_condition && m_condition())
		m_component->frame();
	else {
		//Should we move the cursor, reset next item width, or something like that?
		//What would you expect if you use a decorator like this?
		//If we don't move the cursor, we should also report 0 size.
	}
}

gui::Floats<2> gui::Toggled::getSizeHint() const
{
	if (m_condition && m_condition())
		return m_component->getSizeHint();
	else
		return { 0.0f, 0.0f };
}
