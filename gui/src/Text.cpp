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
#include "Text.h"
#include "CallWrapper.h"

void gui::Text::frame()
{
	util::CallWrapper pop;
	if (m_wrap) {
		ImGui::PushTextWrapPos(0.0f);
		pop = util::CallWrapper(&ImGui::PopTextWrapPos);
	}
	ImGui::TextUnformatted(m_text.c_str());
}

gui::Floats<2> gui::Text::getSizeHint() const
{
	//Not going to work if we wrap!
	ImVec2 size = ImGui::CalcTextSize(m_text.c_str());
	return { size.x, size.y };
}

unsigned int gui::backend::text(const std::string& str)
{
	ImGui::Text(str.c_str());
	return 0;
}

gui::Floats<2> gui::backend::textSize(const std::string& str)
{
	ImVec2 size = ImGui::CalcTextSize(str.c_str());
	return gui_type_conversion<Floats<2>>::from(size);
}
