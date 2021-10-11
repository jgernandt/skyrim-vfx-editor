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
#include "ColourBar.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

void gui::ColourBar::frame()
{
	using namespace ImGui;
	m_size = { m_sizeHint[0] > 0.0f ? m_sizeHint[0] : CalcItemWidth(), m_sizeHint[1] > 0.0f ? m_sizeHint[1] : GetFrameHeight() };
	m_position = { GetCursorPosX(), GetCursorPosY() };//Assumes we are child of the window, which is absolutely not how we're supposed to do this
	ImVec2 size = gui_type_conversion<ImVec2>::from(m_size);
	ImVec2 TLSS{ GetCursorScreenPos() };
	ImVec2 BRSS{ TLSS + size };

	Dummy(size);

	ImDrawList* dl = GetWindowDrawList();
	RenderColorRectWithAlphaCheckerboard(dl, TLSS, BRSS, 0, size.y / 2.0f, ImVec2(0.0f, 0.0f));

	Drawer d;
	d.setTargetLayer(Layer::WINDOW);
	d.pushTransform({ TLSS.x, TLSS.y }, { size.x, size.y });//transform to ((0, 0), (1, 1))
	if (m_gradient)
		m_gradient->draw(d);
}