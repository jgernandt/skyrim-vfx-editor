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
#include "FramePadded.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"//for ImVec operators

//This only really works for text
void gui::FramePadded::frame()
{
	assert(m_component);
	//ImGui::SetCursorPos(ImGui::GetCursorPos() + ImGui::GetStyle().FramePadding);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().FramePadding.x);
	ImGui::AlignTextToFramePadding();
	m_component->frame();
}

gui::Floats<2> gui::FramePadded::getSizeHint() const
{
	assert(m_component);
	Floats<2> base = m_component->getSizeHint();
	return { base[0] + 2.0f * ImGui::GetStyle().FramePadding.x, base[1] + 2.0f * ImGui::GetStyle().FramePadding.y };
}
