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
#include "Popup.h"
#include "CallWrapper.h"

#include "Button.h"
#include "Text.h"
#include "CompositionActions.h"
#include "IInvoker.h"

void gui::PopupBase::open()
{
	m_isOpen = true;
	ImGui::OpenPopup(m_id[0].c_str(), 0);
}

void gui::Popup::frame()
{
	if (m_isOpen) {
		ImGui::SetNextWindowSize({ m_size[0], m_size[1] });

		if (ImGui::BeginPopup(m_id[0].c_str())) {
			util::CallWrapper end(&ImGui::EndPopup);

			ImGui::PushItemWidth(-std::numeric_limits<float>::min());
			util::CallWrapper popWidth(&ImGui::PopItemWidth);

			Composite::frame();
		}

		//Is this right? Test!
		if (!ImGui::IsPopupOpen(m_id[0].c_str()))
			m_isOpen = false;
	}
}

void gui::Modal::frame()
{
	if (m_isOpen) {
		ImGui::SetNextWindowSize({ m_size[0], m_size[1] });
	}

	if (ImGui::BeginPopupModal(m_id[0].c_str())) {
		util::CallWrapper end(&ImGui::EndPopup);

		ImGui::PushItemWidth(-std::numeric_limits<float>::min());
		util::CallWrapper popWidth(&ImGui::PopItemWidth);

		Composite::frame();
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
	addChild(std::make_unique<Button>("Ok", std::bind(&gui::MessageBox::close, this)));
}

void gui::MessageBox::frame()
{
	Modal::frame();
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
