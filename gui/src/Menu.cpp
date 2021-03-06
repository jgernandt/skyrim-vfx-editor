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
#include "Menu.h"
#include "CallWrapper.h"

void gui::MenuItem::frame(FrameDrawer& fd)
{
	if (ImGui::MenuItem(m_label.c_str()))
		onActivate();
}

void gui::Menu::frame(FrameDrawer& fd)
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

		Composite::frame(fd);
	}
}

void gui::MainMenu::frame(FrameDrawer& fd)
{
	if (ImGui::BeginMainMenuBar()) {
		util::CallWrapper endMain(&ImGui::EndMainMenuBar);
		Menu::frame(fd);
	}
}
