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
#include "GUIEngine.h"

#include <atlbase.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>

gui::backend::ImGuiWinD3D10::ImGuiWinD3D10()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui::GetIO().IniFilename = nullptr;
}

gui::backend::ImGuiWinD3D10::~ImGuiWinD3D10()
{
	ImGui_ImplDX10_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::backend::ImGuiWinD3D10::pushTransform(const Floats<2>& translation, const Floats<2>& scale)
{
	if (m_transform.empty())
		m_transform.push({ translation[0], translation[1], scale[0], scale[1] });
	else {
		auto&& T = m_transform.top();
		m_transform.push({
			T[0] + T[2] * translation[0],
			T[1] + T[3] * translation[1],
			T[2] * scale[0],
			T[3] * scale[1] });
	}
}

void gui::backend::ImGuiWinD3D10::popTransform()
{
	if (!m_transform.empty())
		m_transform.pop();
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::getCurrentTranslation() const
{
	return m_transform.empty() ? Floats<2> { 0.0f, 0.0f } : m_transform.top().head(2);
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::getCurrentScale() const
{
	return m_transform.empty() ? Floats<2> { 1.0f, 1.0f } : m_transform.top().tail(2);
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::toGlobal(const Floats<2>& local) const
{
	if (m_transform.empty())
		return local;
	else {
		auto&& T = m_transform.top();
		return { T[0] + T[2] * local[0], T[1] + T[3] * local[1] };
	}
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::toLocal(const Floats<2>& global) const
{
	if (m_transform.empty())
		return global;
	else {
		auto&& T = m_transform.top();
		return { (global[0] - T[0]) / T[2], (global[1] - T[1]) / T[3] };
	}
}

bool gui::backend::ImGuiWinD3D10::isMouseDown(MouseButton btn) const
{
	return ImGui::IsMouseDown(guiToImGuiButton(btn));
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::getMouseMove() const
{
	Floats<2> mousePos = gui_type_conversion<Floats<2>>::from(ImGui::GetIO().MousePos);
	return { mousePos[0] - m_lastMousePos[0], mousePos[1] - m_lastMousePos[1] };
}

void gui::backend::ImGuiWinD3D10::initWin32Window(HWND hwnd)
{
	ImGui_ImplWin32_Init(hwnd);
}

void gui::backend::ImGuiWinD3D10::initDX10Window(ID3D10Device* device)
{
	ImGui_ImplDX10_Init(device);
}

bool gui::backend::ImGuiWinD3D10::loadFont()
{
	ImFont* font = ImGui::GetIO().Fonts->AddFontDefault();
	return font != nullptr;
}

bool gui::backend::ImGuiWinD3D10::loadFont(const std::filesystem::path& path)
{
	//Apparently, the range is used later on
	static const ImWchar range[] = {
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x207B, 0x207B, // superscript minus
		0,
	};
	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.u8string().c_str(), 14.0f, nullptr, range);

	return font != nullptr;
}

void gui::backend::ImGuiWinD3D10::beginFrame()
{
	m_lastMousePos = gui_type_conversion<Floats<2>>::from(ImGui::GetIO().MousePos);
	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::backend::ImGuiWinD3D10::endFrame()
{
	ImGui::Render();
	ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool gui::backend::ImGuiWinD3D10::handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

bool gui::backend::ImGuiWinD3D10::isCapturingKeyboard()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}


void gui::backend::ImGuiWinD3D10::setStyleColours()
{
	ImVec4 blueActive = { 0.66f, 0.7f, 0.86f, 0.85f };
	ImVec4 blueHighlight = { 0.8f, 0.85f, 0.95f, 0.85f };

	//Set the style here, for now
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = { 0.0f, 0.0f, 0.0f, 1.f };
	style.Colors[ImGuiCol_TextDisabled] = { 0.25f, 0.25f, 0.25f, 1.f };
	style.Colors[ImGuiCol_WindowBg] = { 0.75f, 0.75f, 0.75f, 0.85f };
	style.Colors[ImGuiCol_PopupBg] = { 0.85f, 0.85f, 0.85f, 1.0f };
	style.Colors[ImGuiCol_Border] = { 0.65f, 0.65f, 0.65f, 0.85f };
	style.Colors[ImGuiCol_FrameBg] = { 0.8f, 0.8f, 0.8f, 0.85f };
	style.Colors[ImGuiCol_FrameBgHovered] = { 0.85f, 0.85f, 0.85f, 0.85f };
	style.Colors[ImGuiCol_FrameBgActive] = { 0.9f, 0.9f, 0.9f, 0.85f };
	style.Colors[ImGuiCol_MenuBarBg] = { 0.85f, 0.85f, 0.85f, 1.0f };
	style.Colors[ImGuiCol_TitleBg] = { 0.9f, 0.9f, 0.9f, 0.85f };
	style.Colors[ImGuiCol_TitleBgActive] = { 0.95f, 0.95f, 0.95f, 0.85f };
	style.Colors[ImGuiCol_TitleBgCollapsed] = { 0.9f, 0.9f, 0.9f, 0.85f };
	style.Colors[ImGuiCol_Button] = { 0.8f, 0.8f, 0.8f, 0.85f };
	style.Colors[ImGuiCol_ButtonHovered] = blueHighlight;
	style.Colors[ImGuiCol_ButtonActive] = blueActive;
	style.Colors[ImGuiCol_Header] = blueHighlight;
	style.Colors[ImGuiCol_HeaderHovered] = blueHighlight;
	style.Colors[ImGuiCol_HeaderActive] = blueActive;
}

