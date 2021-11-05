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
#include <array>
#include <filesystem>
#include <memory>
#include <stack>
#include <windows.h>

#include "Timer.h"
#include "IComponent.h"

struct ID3D10Device;
struct ImFont;

namespace gui
{
	namespace backend
	{
		//I'm not sure this is how we want to do it, but lump this up with FrameDrawer for now
		//(feels like this class is turning into some general abstraction for all of Dear ImGui)
		class ImGuiWinD3D10 final : public FrameDrawer
		{
		public:
			ImGuiWinD3D10();
			~ImGuiWinD3D10();

			virtual void pushClipArea(const Floats<2>& p1, const Floats<2>& p2, bool intersect = true) override;
			virtual void popClipArea() override;

			virtual void pushTransform(const Floats<2>& translation, const Floats<2>& scale) override;
			virtual void popTransform() override;

			virtual Floats<2> getCurrentTranslation() const override;
			virtual Floats<2> getCurrentScale() const override;

			virtual Floats<2> toGlobal(const Floats<2>& local) const override;
			virtual Floats<2> toLocal(const Floats<2>& global) const override;

			virtual void loadFontScale(float scale) override;
			virtual void pushUIScale(float scale) override;
			virtual void popUIScale() override;

			virtual bool isMouseDown(MouseButton btn) const override;
			virtual Floats<2> getMouseMove() const override;
			virtual Floats<2> getMousePosition() const override;
			virtual float getWheelDelta() const override;

			virtual bool isWheelHandled() const override;
			virtual void setWheelHandled() override;

			void initWin32Window(HWND hwnd);
			void initDX10Window(ID3D10Device* device);

			bool setDefaultFont();
			bool setDefaultFont(const std::filesystem::path& path);

			void beginFrame();
			void endFrame();

			//These two don't really seem to belong here; they should be properties of the composition
			bool handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			bool isCapturingKeyboard();

			void setStyleColours();

		private:
			std::stack<Floats<4>> m_clipArea;
			std::stack<Floats<4>> m_transform;
			std::stack<float> m_uiScale;
			Floats<2> m_lastMousePos{ 0.0f, 0.0f };

			std::vector<char> m_fontBuf;
			std::filesystem::path m_defaultFontPath;
			float m_secondFontScale{ 1.0f };
			bool m_reloadSecond{ false };

			bool m_wheelHandled{ false };

			//Debug
			Timer<long long, std::micro> m_timer;
			int m_frameCount{ 0 };
			long long m_lastTime{ 0 };
			float m_frameRate{ 0.0f };
		};
	}
}