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
#include "DragInput.h"

const int gui::DragInputDataType<int>::id = ImGuiDataType_S32;
const int gui::DragInputDataType<unsigned short>::id = ImGuiDataType_U16;
const int gui::DragInputDataType<float>::id = ImGuiDataType_Float;

extern const unsigned int gui::backend::DragInput_AlwaysClamp = ImGuiSliderFlags_AlwaysClamp;
extern const unsigned int gui::backend::DragInput_Logarithmic = ImGuiSliderFlags_Logarithmic;

namespace ImGui
{
    bool JGDragInput(
        const char* label,
        ImGuiDataType data_type,
        void* p_data,
        float v_speed,
        const void* p_min,
        const void* p_max,
        const char* format,
        ImGuiSliderFlags flags);
}

unsigned int gui::backend::DragInputImpl(const std::string& label, int data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, int flags)
{
    unsigned int result = 0;
    ImGui::JGDragInput(label.c_str(), data_type, p_data, v_speed, p_min, p_max, format, flags);
    if (ImGui::IsItemActivated())
        result |= gui::WIDGET_ACTIVATED;
    if (ImGui::IsItemEdited())
        result |= gui::WIDGET_EDITED;
    if (ImGui::IsItemDeactivatedAfterEdit())
        result |= gui::WIDGET_RELEASED;
    return result;
}

float gui::DefaultLayout::height(size_t N) const
{
    return N * ImGui::GetFrameHeight() + (N - 1) * ImGui::GetStyle().ItemSpacing.y;
}

void gui::HorizontalLayout::begin(size_t N)
{
    ImVec2 pos = ImGui::GetCursorPos();
    m_pos = gui_type_conversion<Floats<2>>::from(pos);
    m_width = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemInnerSpacing.x * (N - 1)) / N;
}

void gui::HorizontalLayout::next(size_t i)
{
    float x = m_pos[0] + i * (m_width + ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::SetCursorPos({ x, m_pos[1] });
    ImGui::SetNextItemWidth(m_width);
}

float gui::HorizontalLayout::height(size_t N) const
{
    return ImGui::GetFrameHeight();
}
