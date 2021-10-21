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
#include <stack>
#include <windows.h>
#include "IComponent.h"

struct ID3D10Device;

namespace gui
{
	namespace backend
	{
		//I'm not sure this is how we want to do it, but lump this up with FrameDrawer for now
		//(feels like this class is turning into some general abstraction for all of Dear ImGui)
		class ImGuiWinD3D10 : public FrameDrawer
		{
		public:
			ImGuiWinD3D10();
			~ImGuiWinD3D10();

			virtual void pushClipArea(const Floats<2>& xlims, const Floats<2>& ylims, bool intersect = true) override {}
			virtual void popClipArea() override {}

			virtual void pushTransform(const Floats<2>& translation, const Floats<2>& scale) override;
			virtual void popTransform() override;

			virtual Floats<2> toGlobal(const Floats<2>& local) const override;
			virtual Floats<2> toLocal(const Floats<2>& global) const override;

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

		private:
			std::stack<Floats<4>> m_clipArea;
			std::stack<Floats<4>> m_transform;
		};

	}
}