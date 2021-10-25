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
#include "ColourBar.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

void gui::ColourBar::frame(FrameDrawer& fd)
{
	using namespace ImGui;
	Floats<2> scale = fd.getCurrentScale();
	Floats<2> globalSize = { m_sizeHint[0] > 0.0f ? m_sizeHint[0] : CalcItemWidth(), m_sizeHint[1] > 0.0f ? m_sizeHint[1] : GetFrameHeight() };
	m_size = globalSize / scale;
	m_translation = { GetCursorPosX() / scale[0], GetCursorPosY() / scale[1] };//Assumes we are child of the window, which is absolutely not how we're supposed to do this
	ImVec2 size = gui_type_conversion<ImVec2>::from(globalSize);
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
