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
#include "Widget.h"
#include "UniqueLabel.h"

namespace gui
{
	//Generalisation of the SimpleHandles we used for our Graph.
	//Should catch user interactions and call overrideable functions.
	//Appearance should be provided by decorators or derived types.
	class Handle : public Component
	{
	public:
		Handle() = default;
		virtual ~Handle() = default;

		virtual void frame(FrameDrawer& fd) override;

		virtual void onClick(Mouse::Button) = 0;
		virtual void onMove(const Floats<2>& delta) = 0;
		virtual void onRelease(Mouse::Button) = 0;

		bool isActive() const { return m_active; }
		bool isHovered() const { return m_hovered; }

	private:
		UniqueLabel<1> m_label;
		bool m_active{ false };
		bool m_hovered{ false };
	};
}