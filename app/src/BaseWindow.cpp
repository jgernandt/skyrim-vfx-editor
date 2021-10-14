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
#include "BaseWindow.h"

const std::wstring app::BaseWindow::s_className{ L"VFXEDITOR" };

app::BaseWindow::BaseWindow(HINSTANCE hInstance, int nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
        throw std::runtime_error("Failed to initialise COM");

    RegisterWinClass(hInstance);
    CreateWindowW(s_className.c_str(), L"New window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, reinterpret_cast<void*>(this));

    //Create message sets m_hwnd

    if (!m_hwnd)
        throw std::runtime_error("Failed to create window");

    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

app::BaseWindow::~BaseWindow()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);

    //Unregister class? Only really matters for dlls?

    CoUninitialize();
}

LRESULT app::BaseWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BaseWindow* window = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        assert(createStruct);
        window = reinterpret_cast<BaseWindow*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

        window->m_hwnd = hwnd;//need this to handle create messages
    }
    else
        window = reinterpret_cast<BaseWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    //std::cout << "HWND: " << hwnd << "\tmsg: " << uMsg << "\tptr: " << window << "\n";

    return window ? window->wndProc(uMsg, wParam, lParam) : DefWindowProc(hwnd, uMsg, wParam, lParam);
}

ATOM app::BaseWindow::RegisterWinClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &BaseWindow::WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = s_className.c_str();
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

LRESULT app::BaseWindow::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}


app::D3D10Window::D3D10Window(HINSTANCE hInstance, int nCmdShow) : BaseWindow(hInstance, nCmdShow)
{
    if (!initD3D())
        throw std::runtime_error("Failed to setup Direct3D");
}

app::D3D10Window::~D3D10Window()
{
}

void app::D3D10Window::clear(const gui::ColRGBA& col)
{
    static_assert(util::colour_traits<gui::ColRGBA>::is_array);
    if (m_d3dDevice) {
        m_d3dDevice->OMSetRenderTargets(1, &m_renderTargetView.p, NULL);
        m_d3dDevice->ClearRenderTargetView(m_renderTargetView, &util::colour_traits<gui::ColRGBA>::R(col));
    }
}

void app::D3D10Window::present()
{
    if (m_swapChain)
        m_swapChain->Present(1, 0);
}

LRESULT app::D3D10Window::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
            resizeRenderTarget((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
        return 0;
    }
    return BaseWindow::wndProc(uMsg, wParam, lParam);
}

bool app::D3D10Window::initD3D()
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
    if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &m_swapChain, &m_d3dDevice)))
        return false;
    else 
        return createRenderTarget();
}

bool app::D3D10Window::createRenderTarget()
{
    if (m_swapChain && m_d3dDevice) {
        CComPtr<ID3D10Texture2D> backBuffer;
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        m_d3dDevice->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
        return true;
    }
    else
        return false;
}

void app::D3D10Window::resizeRenderTarget(UINT w, UINT h)
{
    if (m_d3dDevice) {
        m_renderTargetView.Release();
        m_swapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
        createRenderTarget();
    }
}