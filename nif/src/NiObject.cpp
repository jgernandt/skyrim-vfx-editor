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
#include "NiObject.h"

#ifdef _DEBUG
int g_currentNiObjects = 0;
#endif

nif::NiObject::NiObject(native_type* obj) : m_ptr{ obj }
{
	assert(m_ptr);
	m_ptr->AddRef();

#ifdef _DEBUG
	g_currentNiObjects++;
#endif
}

nif::NiObject::~NiObject()
{
	m_ptr->SubtractRef();

#ifdef _DEBUG
	g_currentNiObjects--;
#endif
}
