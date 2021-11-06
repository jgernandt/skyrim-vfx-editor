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
#include "Popup.h"
#include "CallWrapper.h"

#include "Button.h"
#include "Item.h"
#include "Text.h"
#include "CompositionActions.h"
#include "IInvoker.h"

#include "imgui_internal.h"

gui::PopupBase::PopupBase(const std::string& title) : 
	m_id(title),
	m_style{ ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse }
{
}

//imgui requires the open call to be in the immediate vicinity of the begin call (same level of their window stack)
void gui::PopupBase::open()
{
	m_shouldOpen = true;
}

void gui::PopupBase::frame(FrameDrawer& fd)
{
	using namespace ImGui;

	if (m_shouldOpen) {
		ImGui::OpenPopup(m_id[0].c_str(), 0);
		m_isOpen = true;
		m_firstFrame = true;
		m_shouldOpen = false;
		onOpen();
	}
	else if (m_isOpen) {
		if (!ImGui::IsPopupOpen(m_id[0].c_str())) {
			m_isOpen = false;
			onClose();
		}
		else {
			ImGuiContext& g = *GImGui;
			assert(g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == g.CurrentWindow->GetID(m_id[0].c_str()));

			if (m_firstFrame) {
				m_translation = -fd.toGlobal({ 0.0f, 0.0f }) / fd.getCurrentScale();
				m_lastGlobalPos = { 0.0f, 0.0f };
				m_firstFrame = false;
			}

			if (ImGuiWindow* window = g.OpenPopupStack[g.BeginPopupStack.Size].Window) {
				//Floats<2> currentGlobal = gui_type_conversion<Floats<2>>::from(window->Pos);//not accounting for new ancestor transforms
				//Floats<2> translation_from_dragging = currentGlobal - m_lastGlobalPos;//in global scale

				m_translation += (gui_type_conversion<Floats<2>>::from(window->Pos) - m_lastGlobalPos) / fd.getCurrentScale();
			}

			Floats<2> pos = fd.toGlobal(m_translation);
			m_lastGlobalPos = { std::floorf(pos[0]), std::floorf(pos[1]) };
			SetNextWindowPos(gui_type_conversion<ImVec2>::from(m_lastGlobalPos));
		}
	}
}

void gui::PopupBase::setStyle(Window::Style style, bool on)
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

void gui::Popup::frame(FrameDrawer& fd)
{
	PopupBase::frame(fd);

	if (m_isOpen) {
		Floats<2> size = m_size * fd.getCurrentScale();
		ImGui::SetNextWindowSize({ std::floorf(size[0]), std::floorf(size[1]) });

		if (ImGui::BeginPopup(m_id[0].c_str(), m_style)) {
			util::CallWrapper end(&ImGui::EndPopup);

			ImGui::PushItemWidth(-std::numeric_limits<float>::min());
			util::CallWrapper popWidth(&ImGui::PopItemWidth);

			Composite::frame(fd);
		}
		else {
			//we messed something up!
			assert(false);
		}
	}
}

//Modal has yet to be updated to the new open/close mechanism (yes, it's a mess)

void gui::Modal::frame(FrameDrawer& fd)
{
	PopupBase::frame(fd);

	if (m_isOpen) {
		ImGui::SetNextWindowSize({ m_size[0], m_size[1] });
	}

	if (ImGui::BeginPopupModal(m_id[0].c_str(), nullptr, ImGuiWindowFlags_NoResize)) {
		util::CallWrapper end(&ImGui::EndPopup);

		ImGui::PushItemWidth(-std::numeric_limits<float>::min());
		util::CallWrapper popWidth(&ImGui::PopItemWidth);

		Composite::frame(fd);
	}
	else {
		//So, here's what I've figured out:
		//If OpenPopup is called BEFORE BeginPopup is called for the first time, 
		//AND a call to Begin with a new window id is made AFTER the call to BeginPopup,
		//we end up here. BeginPopupModal returns false and the popup never shows up.
		//As long as OpenPopup is not called until after the very first call to BeginPopup, we are fine.
		//If we get here, we didn't follow these rules.
		assert(!m_isOpen);
	}
}

void gui::Modal::close()
{
	ImGui::CloseCurrentPopup();
	m_isOpen = false;
}

gui::MessageBox::MessageBox(const std::string& title, const std::string& msg) :
	Modal(title)
{
	m_size = { 200.0f, 0.0f };
	auto text = newChild<Text>(msg);
	text->setWrap();
	auto item = std::make_unique<Item>(std::make_unique<RightAlign>());
	item->newChild<Button>("Ok", std::bind(&gui::MessageBox::close, this));
	addChild(std::move(item));
}

void gui::MessageBox::frame(FrameDrawer& fd)
{
	Modal::frame(fd);
	//We must not be open on the first frame
	if (!m_isOpen && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
		open();
}

void gui::MessageBox::close()
{
	ImGui::CloseCurrentPopup();
	//m_isOpen = false;//leave it, or we'll open again before we are destroyed
	asyncInvoke<RemoveChild>(this, getParent(), false);
}
