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

#undef min
#undef max

#include "imgui_internal.h"

constexpr float FONT_SIZE_BASE = 14.0f;

//Apparently, this needs to persist
const ImWchar GLYPH_RANGE[] = {
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x207B, 0x207B, // superscript minus
	0,
};

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

void gui::backend::ImGuiWinD3D10::circle(const Floats<2>& centre, float radius, const ColRGBA& col, bool global)
{
	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		if (global)
			drawList->AddCircleFilled(
				gui_type_conversion<ImVec2>::from(centre),
				radius,
				gui_type_conversion<ImU32>::from(col));
		else
			drawList->AddCircleFilled(
				gui_type_conversion<ImVec2>::from(toGlobal(centre)),
				radius * getCurrentScale()[0],			//Only using x scale here (we could do geom average until we have ellipses)
				gui_type_conversion<ImU32>::from(col));
}

void gui::backend::ImGuiWinD3D10::line(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col, float width, bool global)
{
	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		if (global)
			drawList->AddLine(
				gui_type_conversion<ImVec2>::from(p1),
				gui_type_conversion<ImVec2>::from(p2),
				gui_type_conversion<ImU32>::from(col),
				width);
		else
			drawList->AddLine(
				gui_type_conversion<ImVec2>::from(toGlobal(p1)), 
				gui_type_conversion<ImVec2>::from(toGlobal(p2)), 
				gui_type_conversion<ImU32>::from(col),
				width);
}

void gui::backend::ImGuiWinD3D10::rectangle(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col, bool global)
{
	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		if (global)
			drawList->AddRectFilled(
				gui_type_conversion<ImVec2>::from(p1),
				gui_type_conversion<ImVec2>::from(p2),
				gui_type_conversion<ImU32>::from(col));
		else
			drawList->AddRectFilled(
				gui_type_conversion<ImVec2>::from(toGlobal(p1)), 
				gui_type_conversion<ImVec2>::from(toGlobal(p2)), 
				gui_type_conversion<ImU32>::from(col));
}

void gui::backend::ImGuiWinD3D10::rectangleGradient(const Floats<2>& p1, const Floats<2>& p2, 
	const ColRGBA& tl, const ColRGBA& tr, const ColRGBA& bl, const ColRGBA& br, bool global)
{
	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		if (global)
			drawList->AddRectFilledMultiColor(
				gui_type_conversion<ImVec2>::from(p1),
				gui_type_conversion<ImVec2>::from(p2),
				gui_type_conversion<ImU32>::from(tl),
				gui_type_conversion<ImU32>::from(tr),
				gui_type_conversion<ImU32>::from(br),
				gui_type_conversion<ImU32>::from(bl));
		else
			drawList->AddRectFilledMultiColor(
				gui_type_conversion<ImVec2>::from(toGlobal(p1)), 
				gui_type_conversion<ImVec2>::from(toGlobal(p2)),
				gui_type_conversion<ImU32>::from(tl), 
				gui_type_conversion<ImU32>::from(tr), 
				gui_type_conversion<ImU32>::from(br), 
				gui_type_conversion<ImU32>::from(bl));
}

void gui::backend::ImGuiWinD3D10::triangle(const Floats<2>& p1, const Floats<2>& p2, const Floats<2>& p3, const ColRGBA& col, bool global)
{
	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		if (global)
			drawList->AddTriangleFilled(
				gui_type_conversion<ImVec2>::from(p1),
				gui_type_conversion<ImVec2>::from(p2),
				gui_type_conversion<ImVec2>::from(p3),
				gui_type_conversion<ImU32>::from(col));
		else
			drawList->AddTriangleFilled(
				gui_type_conversion<ImVec2>::from(toGlobal(p1)), 
				gui_type_conversion<ImVec2>::from(toGlobal(p2)), 
				gui_type_conversion<ImVec2>::from(toGlobal(p3)), 
				gui_type_conversion<ImU32>::from(col));
}

util::CallWrapper gui::backend::ImGuiWinD3D10::pushClipArea(const Floats<2>& p1, const Floats<2>& p2, bool intersect)
{
	Floats<2> min = toGlobal({ std::min(p1[0], p2[0]), std::min(p1[1], p2[1]) });
	Floats<2> max = toGlobal({ std::max(p1[0], p2[0]), std::max(p1[1], p2[1]) });
	ImGui::PushClipRect({ min[0], min[1] }, { max[0], max[1] }, intersect);

	return util::CallWrapper(&ImGuiWinD3D10::popClipArea, this);
}

void gui::backend::ImGuiWinD3D10::popClipArea()
{
	ImGui::PopClipRect();
}

util::CallWrapper gui::backend::ImGuiWinD3D10::pushTransform(const Floats<2>& translation, const Floats<2>& scale)
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

	return util::CallWrapper(&ImGuiWinD3D10::popTransform, this);
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

void gui::backend::ImGuiWinD3D10::loadFontScale(float scale)
{
	float fontSize = (FONT_SIZE_BASE * scale);
	if (fontSize != m_secondFontScale && !m_fontBuf.empty()) {
		m_secondFontScale = fontSize;
		m_reloadSecond = true;
	}
}

void gui::backend::ImGuiWinD3D10::pushUIScale(float scale)
{
	float fontSize = (FONT_SIZE_BASE * scale);

#ifdef DEBUG
	if (ImGui::Begin("Metrics/Debugger##JGDebug")) {
		ImGui::Text("Scale %.2f", scale);
		ImGui::Text("Font size %.0f", fontSize);
	}
	ImGui::End();
#endif

	ImFont* font = nullptr;
	if (fontSize == FONT_SIZE_BASE || m_fontBuf.empty()) {
		font = ImGui::GetIO().Fonts->Fonts[0];
	}
	else if (ImGui::GetIO().Fonts->Fonts.Size > 1 && fontSize == ImGui::GetIO().Fonts->Fonts[1]->FontSize) {
		font = ImGui::GetIO().Fonts->Fonts[1];
	}
	else {
		//we messed something up!
	}
	assert(font);

	ImGui::PushFont(font);

	float relScale = m_uiScale.empty() ? scale : scale / m_uiScale.top();
	m_uiScale.push(scale);

	//We're leaving out some table and tree vars. And rounding of corners. Fix if we need.
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { (style.WindowPadding.x * relScale), (style.WindowPadding.y * relScale) });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { (style.WindowMinSize.x * relScale), (style.WindowMinSize.y * relScale) });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { (style.FramePadding.x * relScale), (style.FramePadding.y * relScale) });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { (style.ItemSpacing.x * relScale), (style.ItemSpacing.y * relScale) });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { (style.ItemInnerSpacing.x * relScale), (style.ItemInnerSpacing.y * relScale) });
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, (style.ScrollbarSize * relScale));
	ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, (style.GrabMinSize * relScale));
}

void gui::backend::ImGuiWinD3D10::popUIScale()
{
	ImGui::PopFont();
	ImGui::PopStyleVar(7);
	m_uiScale.pop();
}

bool gui::backend::ImGuiWinD3D10::isMouseDown(Mouse::Button btn) const
{
	return ImGui::IsMouseDown(guiToImGuiButton(btn));
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::getMouseMove() const
{
	Floats<2> mousePos = gui_type_conversion<Floats<2>>::from(ImGui::GetIO().MousePos);
	return toLocal(mousePos - m_lastMousePos);
}

gui::Floats<2> gui::backend::ImGuiWinD3D10::getMousePosition() const
{
	return toLocal(gui_type_conversion<Floats<2>>::from(ImGui::GetIO().MousePos));
}

bool gui::backend::ImGuiWinD3D10::isWheelHandled() const
{
	return m_wheelHandled;
	//ImGuiContext* c = ImGui::GetCurrentContext();
	//return c ? c->ActiveIdUsingMouseWheel || c->HoveredIdUsingMouseWheel : false;
}

void gui::backend::ImGuiWinD3D10::setWheelHandled()
{
	m_wheelHandled = true;
}

float gui::backend::ImGuiWinD3D10::getWheelDelta() const
{
	return ImGui::GetIO().MouseWheel;
}

void gui::backend::ImGuiWinD3D10::initWin32Window(HWND hwnd)
{
	ImGui_ImplWin32_Init(hwnd);
}

void gui::backend::ImGuiWinD3D10::initDX10Window(ID3D10Device* device)
{
	ImGui_ImplDX10_Init(device);
}

bool gui::backend::ImGuiWinD3D10::setDefaultFont()
{
	m_defaultFontPath = std::filesystem::path();
	m_fontBuf.clear();
	ImFont* font = ImGui::GetIO().Fonts->AddFontDefault();
	return font != nullptr;
}

bool gui::backend::ImGuiWinD3D10::setDefaultFont(const std::filesystem::path& path)
{
	bool result = false;
	std::ifstream file(path, std::ifstream::binary);
	if (file.is_open()) {
		file.seekg(0, std::ios_base::end);
		m_fontBuf.resize(static_cast<size_t>(file.tellg()));
		file.seekg(0);
		file.read(m_fontBuf.data(), m_fontBuf.size());
		file.close();

		ImFontConfig cfg;
		cfg.FontDataOwnedByAtlas = false;
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(m_fontBuf.data(), m_fontBuf.size(), FONT_SIZE_BASE, &cfg, GLYPH_RANGE);

		if (font != nullptr) {
			m_defaultFontPath = path;
			result = true;
		}
	}
	return result;
}

void UpdateFontTexture();

void gui::backend::ImGuiWinD3D10::beginFrame()
{
	m_lastMousePos = gui_type_conversion<Floats<2>>::from(ImGui::GetIO().MousePos);
	m_wheelHandled = false;

	bool rebuild = false;
	if (m_reloadSecond) {
		ImFontAtlas* fonts = ImGui::GetIO().Fonts;
		fonts->Clear();

		ImFontConfig cfg;
		cfg.FontDataOwnedByAtlas = false;
		fonts->AddFontFromMemoryTTF(m_fontBuf.data(), m_fontBuf.size(), FONT_SIZE_BASE, &cfg, GLYPH_RANGE);
		fonts->AddFontFromMemoryTTF(m_fontBuf.data(), m_fontBuf.size(), m_secondFontScale, &cfg, GLYPH_RANGE);

		rebuild = true;
		m_reloadSecond = false;
	}
	if (rebuild) {
		//ImGui::GetIO().Fonts->Build();//redundant? imgui.h says it's called by GetTexData*
		UpdateFontTexture();
	}

	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::backend::ImGuiWinD3D10::endFrame()
{
#ifdef DEBUG
	if (m_frameCount++ % 10 == 0) {
		long long t = m_timer.elapsed();
		m_frameRate = 10.0f / (static_cast<float>(t - m_lastTime) * 1e-6f);
		m_lastTime = t;
	}

	ImGuiContext* c = ImGui::GetCurrentContext();
	assert(c && !c->Viewports.empty() && c->Viewports[0]);
	ImGui::SetNextWindowSize({ 150.0f, 0.0f });
	ImGui::SetNextWindowPos({ c->Viewports[0]->Size[0] - 175.0f, 25.0f });
	if (ImGui::Begin("Metrics/Debugger##JGDebug")) {
		ImGui::Text("%.1f FPS", m_frameRate);
	}
	ImGui::End();
#endif

	assert(m_clipArea.empty() && m_transform.empty());//or we forgot to pop it

	//This seems like a reliable way to keep the main menu bar on top. Haven't noticed any side effects yet.
	ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("##MainMenuBar"));

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


//ImGui backend addition

// DirectX data
struct ImplDX10_Data
{
	ID3D10Device* pd3dDevice;
	IDXGIFactory* pFactory;
	ID3D10Buffer* pVB;
	ID3D10Buffer* pIB;
	ID3D10VertexShader* pVertexShader;
	ID3D10InputLayout* pInputLayout;
	ID3D10Buffer* pVertexConstantBuffer;
	ID3D10PixelShader* pPixelShader;
	ID3D10SamplerState* pFontSampler;
	ID3D10ShaderResourceView* pFontTextureView;
	ID3D10RasterizerState* pRasterizerState;
	ID3D10BlendState* pBlendState;
	ID3D10DepthStencilState* pDepthStencilState;
	int                         VertexBufferSize;
	int                         IndexBufferSize;

	ImplDX10_Data() = delete;
};

void UpdateFontTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	ImplDX10_Data* bd = ImGui::GetCurrentContext() ? reinterpret_cast<ImplDX10_Data*>(io.BackendRendererUserData) : nullptr;
	if (!bd)
		throw std::runtime_error("No context or renderer data");

	if (bd->pFontSampler) {
		bd->pFontSampler->Release();
		bd->pFontSampler = nullptr;
	}
	if (bd->pFontTextureView) {
		bd->pFontTextureView->Release(); 
		bd->pFontTextureView = nullptr; 
		ImGui::GetIO().Fonts->SetTexID(nullptr);
	}

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	{
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D10Texture2D* pTexture = NULL;
		D3D10_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = pixels;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		bd->pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
		IM_ASSERT(pTexture != NULL);

		// Create texture view
		D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof(srv_desc));
		srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;
		bd->pd3dDevice->CreateShaderResourceView(pTexture, &srv_desc, &bd->pFontTextureView);
		pTexture->Release();
	}

	// Store our identifier
	io.Fonts->SetTexID((ImTextureID)bd->pFontTextureView);

	// Create texture sampler
	{
		D3D10_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D10_COMPARISON_ALWAYS;
		desc.MinLOD = 0.f;
		desc.MaxLOD = 0.f;
		bd->pd3dDevice->CreateSamplerState(&desc, &bd->pFontSampler);
	}
}
