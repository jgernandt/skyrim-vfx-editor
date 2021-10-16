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
#include "Selector.h"

util::CallWrapper<> gui::backend::Selector(const std::string& label, const std::string& preview)
{
	if (ImGui::BeginCombo(label.c_str(), preview.c_str()))
		return util::CallWrapper(&ImGui::EndCombo);
	else
		return util::CallWrapper();
}

bool gui::backend::SelectableItem(const std::string& label, bool selected)
{
	return ImGui::Selectable(label.c_str(), selected);
}

bool gui::backend::ToggleableItem(const std::string& label, bool toggled)
{
	return ImGui::Selectable(label.c_str(), toggled, ImGuiSelectableFlags_DontClosePopups);
}
