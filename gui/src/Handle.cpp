//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "Handle.h"
#include "Drawer.h"

constexpr gui::MouseButton imguiToGuiButton(int button)
{
	switch (button) {
	case ImGuiButtonFlags_MouseButtonLeft:
		return gui::MouseButton::LEFT;
	case ImGuiButtonFlags_MouseButtonMiddle:
		return gui::MouseButton::MIDDLE;
	case ImGuiButtonFlags_MouseButtonRight:
		return gui::MouseButton::RIGHT;
	default:
		return gui::MouseButton::NONE;
	}
}

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

void gui::Handle::frame()
{
	using namespace ImGui;

	ImVec2 start_pos = GetCursorPos();//we should probably reset this when we're done
	SetCursorPos(gui_type_conversion<ImVec2>::from(m_position));

	InvisibleButton(m_label[0].c_str(), gui_type_conversion<ImVec2>::from(m_size), ImGuiButtonFlags_MouseButtonLeft);

	if (IsItemActivated())
		onClick(imguiToGuiButton(ImGuiButtonFlags_MouseButtonLeft));

	if (IsItemActive()) {
		m_active = true;
		if (GetIO().MouseDelta.x != 0.0f || GetIO().MouseDelta.y != 0.0f)
			onMove({ GetIO().MouseDelta.x, GetIO().MouseDelta.y });
	}

	if (IsItemDeactivated()) {
		m_active = false;
		onRelease(imguiToGuiButton(ImGuiButtonFlags_MouseButtonLeft));
	}

	//Might be weird to always register as hovered when active. We'll see.
	if (m_active || IsItemHovered())
		m_hovered = true;
	else
		m_hovered = false;


	SetCursorPos(start_pos);
}