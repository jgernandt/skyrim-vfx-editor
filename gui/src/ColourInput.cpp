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
#include "ColourInput.h"

float gui::getDefaultHeight()
{
	if (ImGui::GetCurrentContext())
		return ImGui::GetFrameHeight();
	else
		return 0.0f;
}

unsigned int gui::backend::ColourInput(const std::string& label, float* arr)
{
	unsigned int flags = ImGuiColorEditFlags_NoInputs | //ImGuiColorEditFlags_NoLabel |
		ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;

	unsigned int result = 0;
	ImGui::ColorEdit4(label.c_str(), arr, flags);
	if (ImGui::IsItemActivated())
		result |= gui::WIDGET_ACTIVATED;
	if (ImGui::IsItemEdited())
		result |= gui::WIDGET_EDITED;
	if (ImGui::IsItemDeactivatedAfterEdit())
		result |= gui::WIDGET_RELEASED;
	return result;
}