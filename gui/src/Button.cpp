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
#include "Button.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"//for ImVec operators

void gui::Button::frame()
{
	if (ImGui::Button(m_label[0].c_str(), gui_type_conversion<ImVec2>::from(m_size)))
		onActivate();
}

gui::Floats<2> gui::Button::getSizeHint() const
{
	if (m_size[0] > 0.0f && m_size[1] > 0.0f)
		return m_size;
	else {
		ImVec2 lSize = ImGui::CalcTextSize(m_label[0].c_str(), nullptr, true);
		return { m_size[0] <= 0.0f ? lSize.x + 2.0f * ImGui::GetStyle().FramePadding.x : m_size[0],
			m_size[1] <= 0.0f ? lSize.y + 2.0f * ImGui::GetStyle().FramePadding.y : m_size[1] };
	}
}
