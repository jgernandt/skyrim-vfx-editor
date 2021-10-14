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
#include "Panel.h"
#include "CallWrapper.h"
#include "Popup.h"
#include "CompositionActions.h"

#include "imgui_internal.h"//for GImGui

void gui::Panel::frame()
{
	/*
	* This works, but not for the purpose I intended it; to catch right clicks on the backgroud.
	* The reason it doesn't work is that connectors only trigger if no other window is hovered.
	* Should be fixable, but it can wait. I'll go back to my old solution for now.
	
	ImGui::SetNextWindowSize(util::convert_to<ImVec2>::from(m_size));
	ImGui::SetNextWindowPos({ 0.0f, 0.0f });

	unsigned int flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (ImGui::Begin(m_label[0].c_str(), nullptr, flags)) {
		util::CallWrapper end(&ImGui::End);

		if (m_contextMenu) {
			if (ImGui::InvisibleButton((m_label[0] + "button").c_str(), util::convert_to<ImVec2>::from(m_size), ImGuiButtonFlags_MouseButtonRight))
				m_contextMenu->open();
		}

		Composite::frame();
	}
	else {
		ImGui::End();
	}*/

	if (m_contextMenu) {
		ImGuiContext& g = *GImGui;
		//Ideally, we should test for press-release here
		if (!g.HoveredWindow && !g.ActiveId && g.IO.MouseReleased[ImGuiMouseButton_Right]) {
			bool showPopup = true;

			//similar to IsWindowContentHoverable:
			if (g.NavWindow) {
				if (ImGuiWindow* focussed_root_window = g.NavWindow->RootWindow)
					if (focussed_root_window->WasActive)
						if (focussed_root_window->Flags & (ImGuiWindowFlags_Modal | ImGuiWindowFlags_Popup))
							showPopup = false;
			}

			if (showPopup)
				m_contextMenu->open();
		}
	}

	Composite::frame();
}

/*void gui::Panel::setSizeHint(const Floats<2>& size)
{
	//We can't allow 0 size or imgui flips (when we use it for an invisible button)
	m_size = { std::max(size[0], std::numeric_limits<float>::min()), std::max(size[1], std::numeric_limits<float>::min()) };
}*/

void gui::Panel::setContextMenu(std::unique_ptr<Popup>&& menu)
{
	asyncInvoke<RemoveChild>(m_contextMenu, this, false);
	m_contextMenu = menu.get();
	asyncInvoke<AddChild>(std::move(menu), this, false);
}

