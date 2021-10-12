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
#include "Checkbox.h"

bool gui::backend::Checkbox(const std::string& label, bool* v)
{
	return ImGui::Checkbox(label.c_str(), v);
}

gui::Floats<2> gui::backend::getCheckboxSize(const std::string& label)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 label_size = ImGui::CalcTextSize(label.c_str(), nullptr, true);
    const float square_sz = ImGui::GetFrameHeight();

    ImVec2 size(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
        label_size.y + 2.0f * style.FramePadding.y);

    return gui_type_conversion<Floats<2>>::from(size);
}
