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

#pragma once
#include <filesystem>
#include <memory>
#include <windows.h>

struct ID3D10Device;

namespace gui
{
	namespace backend
	{
		class ImGuiWinD3D10
		{
		public:
			ImGuiWinD3D10();
			~ImGuiWinD3D10();

			void initWin32Window(HWND hwnd);
			void initDX10Window(ID3D10Device* device);

			bool loadFont();
			bool loadFont(const std::filesystem::path& path);

			void beginFrame();
			void endFrame();

			//These two don't really seem to belong here; they should be properties of the composition
			bool handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			bool isCapturingKeyboard();

			void setStyleColours();
		};

	}
}