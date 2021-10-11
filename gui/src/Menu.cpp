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
#include "Menu.h"
#include "CallWrapper.h"

void gui::MenuItem::frame()
{
	if (ImGui::MenuItem(m_label.c_str()))
		onActivate();
}

void gui::Menu::frame()
{
	if (ImGui::BeginMenu(m_label.c_str())) {
		util::CallWrapper endMenu(&ImGui::EndMenu);

		if (ImGui::IsItemActivated()) {
			//instantiate menu items here?
			//assert(m_children.empty());
			//We would need some identifier for a list of commands to fill the menu with.
			//Or instead of listing commands, list factory objects?
			//So we just retrieve the list and call each factory object in turn, 
			//adding the return to our children.
		}

		Composite::frame();
	}
}

void gui::MainMenu::frame()
{
	if (ImGui::BeginMainMenuBar()) {
		util::CallWrapper endMain(&ImGui::EndMainMenuBar);
		Menu::frame();
	}
}
