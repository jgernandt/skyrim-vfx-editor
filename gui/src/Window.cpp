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
#include "Window.h"
#include "Visitor.h"
#include "CallWrapper.h"
#include "CompositionActions.h"
#include "gui_types.h"

gui::Window::Window(const std::string& title) :
	m_title(title), 
	m_style { ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse }
{
	if (ImGui::GetCurrentContext()) {
		m_colours[COL_TITLE] = gui_type_conversion<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_TitleBg]);
		m_colours[COL_TITLE_ACTIVE] = gui_type_conversion<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		m_colours[COL_POPUP] = gui_type_conversion<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_PopupBg]);
		/*m_colours[COL_BACKGROUND] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		m_colours[COL_BORDER] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_Border]);
		m_colours[COL_TEXT] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_Text]);
		m_colours[COL_BUTTON] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_Button]);
		m_colours[COL_BUTTON_HOVER] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
		m_colours[COL_BUTTON_ACTIVE] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		m_colours[COL_FRAME] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
		m_colours[COL_FRAME_HOVER] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
		m_colours[COL_FRAME_ACTIVE] = util::convert_to<gui::ColRGBA>::from(ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);*/
	}
	//else we were created before the context. We'll still work, but our colours may be wrong.
	//(only a problem during tests, so far)
}

void gui::Window::accept(Visitor& v)
{
	v.visit(*this);
}

#include "imgui_internal.h"
void gui::Window::frame(FrameDrawer& fd)
{

	using namespace ImGui;

	Floats<2> scale = fd.getCurrentScale();
	Floats<2> size = m_size * scale;
	SetNextWindowSize({ std::floorf(size[0]), std::floorf(size[1]) });

	//Position
	//ImGui handles dragging windows during Begin/EndFrame. We need to pull out the current position of 
	//our window from imgui now, before we start tampering with it.
	//But how to distinguish dragging the window with the mouse from transforming an ancestor?

	if (ImGuiWindow* window = FindWindowByName(m_title[0].c_str())) {
		//Floats<2> currentGlobal = gui_type_conversion<Floats<2>>::from(window->Pos);//not accounting for new ancestor transforms
		//Floats<2> translation_from_dragging = currentGlobal - m_lastGlobalPos;//in global scale
		m_translation += (gui_type_conversion<Floats<2>>::from(window->Pos) - m_lastGlobalPos) / scale;
	}

	Floats<2> pos = fd.toGlobal(m_translation);
	SetNextWindowPos({ std::floorf(pos[0]), std::floorf(pos[1]) });

	PushStyleColor(ImGuiCol_TitleBg, gui_type_conversion<ImVec4>::from(m_colours[COL_TITLE]));
	PushStyleColor(ImGuiCol_TitleBgActive, gui_type_conversion<ImVec4>::from(m_colours[COL_TITLE_ACTIVE]));
	PushStyleColor(ImGuiCol_TitleBgCollapsed, gui_type_conversion<ImVec4>::from(m_colours[COL_TITLE]));
	PushStyleColor(ImGuiCol_PopupBg, gui_type_conversion<ImVec4>::from(m_colours[COL_POPUP]));
	/*PushStyleColor(ImGuiCol_WindowBg, util::convert_to<ImVec4>::from(m_colours[COL_BACKGROUND]));
	PushStyleColor(ImGuiCol_Border, util::convert_to<ImVec4>::from(m_colours[COL_BORDER]));
	PushStyleColor(ImGuiCol_Text, util::convert_to<ImVec4>::from(m_colours[COL_TEXT]));
	PushStyleColor(ImGuiCol_Button, util::convert_to<ImVec4>::from(m_colours[COL_BUTTON]));
	PushStyleColor(ImGuiCol_ButtonHovered, util::convert_to<ImVec4>::from(m_colours[COL_BUTTON_HOVER]));
	PushStyleColor(ImGuiCol_ButtonActive, util::convert_to<ImVec4>::from(m_colours[COL_BUTTON_ACTIVE]));
	PushStyleColor(ImGuiCol_FrameBg, util::convert_to<ImVec4>::from(m_colours[COL_FRAME]));
	PushStyleColor(ImGuiCol_FrameBgHovered, util::convert_to<ImVec4>::from(m_colours[COL_FRAME_HOVER]));
	PushStyleColor(ImGuiCol_FrameBgActive, util::convert_to<ImVec4>::from(m_colours[COL_FRAME_ACTIVE]));*/
	util::CallWrapper popStyle(&PopStyleColor, 4);

	bool keepOpen = true;
	bool* p_open = m_closable ? &keepOpen : nullptr;

	util::CallWrapper ender(&ImGui::End);
	if (Begin(m_title[0].c_str(), p_open, m_style)) {

		PushItemWidth(-std::numeric_limits<float>::min());
		util::CallWrapper popItemWidth(&PopItemWidth);

		m_lastGlobalPos = gui_type_conversion<Floats<2>>::from(GetWindowPos());

		Composite::frame(fd);
	}
	else {
		//When do we get here? I thought it was when window is collapsed, but no...
		End();
	}

	if (!keepOpen)
		onClose();
}

void gui::Window::setTranslation(const Floats<2>& t)
{
	m_translation = t;
	//m_lastGlobalPos = getGlobalPosition();//wrong, will undo the translation next frame
}

void gui::Window::onClose()
{
	asyncInvoke<RemoveChild>(this, getParent(), false);//default to irreversible deletion. Bad idea?
}

void gui::Window::setStyle(Style style, bool on)
{
	switch (style) {
	case Style::SCROLLABLE:
		if (on)
			m_style &= ~(ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
		else
			m_style |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		break;
	}
}
