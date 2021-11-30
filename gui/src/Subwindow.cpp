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
#include "Subwindow.h"
#include "CallWrapper.h"

//We make use of Eigen::Array methods for rounding
static_assert(TO_PIXEL == static_cast<decltype(TO_PIXEL)>(&std::floor));

void gui::Subwindow::frame(FrameDrawer& fd)
{
	ImGui::SetCursorPos(gui_type_conversion<ImVec2>::from((m_translation * fd.getCurrentScale()).floor().eval()));

	util::CallWrapper end;
	if (ImGui::BeginChild(
		m_label[0].c_str(), 
		gui_type_conversion<ImVec2>::from((m_size * fd.getCurrentScale()).floor().eval()),
		false,
		m_style))
	{
		end = util::CallWrapper(ImGui::EndChild);

		ImGui::PushItemWidth(-std::numeric_limits<float>::min());
		util::CallWrapper popItemWidth(&ImGui::PopItemWidth);

		Composite::frame(fd);
	}
}

void gui::Subwindow::setStyle(Window::Style style, bool on)
{
	switch (style) {
	case Window::Style::SCROLLABLE:
		if (on)
			m_style &= ~(ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
		else
			m_style |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		break;
	}
}
