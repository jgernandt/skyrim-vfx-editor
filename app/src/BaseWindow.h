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

#pragma once
#include "framework.h"
#include "gui_types.h"

namespace app
{
	class BaseWindow
	{
	public:
		BaseWindow(HINSTANCE hInstance, int nCmdShow);
		~BaseWindow();

	protected:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static ATOM RegisterWinClass(HINSTANCE hInstance);
		static const std::wstring s_className;

	protected:
		virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd{ NULL };
	};

	class D3D10Window :
		public BaseWindow
	{
	public:
		D3D10Window(HINSTANCE hInstance, int nCmdShow);
		~D3D10Window();

		void clear(const gui::ColRGBA& col);
		void present();

	protected:
		virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	private:
		bool initD3D();
		bool createRenderTarget();
		void resizeRenderTarget(UINT w, UINT h);

	protected:
		CComPtr<ID3D10Device> m_d3dDevice;
		CComPtr<IDXGISwapChain> m_swapChain;
		CComPtr<ID3D10RenderTargetView> m_renderTargetView;
	};
}

