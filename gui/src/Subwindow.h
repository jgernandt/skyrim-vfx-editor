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
#include "Window.h"
#include "UniqueLabel.h"

namespace gui
{
	class Subwindow : public Composite
	{
	public:
		Subwindow(const std::string& name = std::string()) : m_label(name) {}
		virtual void frame(FrameDrawer& fd) override;
		void setStyle(Window::Style style, bool on = true);

	private:
		UniqueLabel<1> m_label;
		unsigned int m_style{ 24 };//no scroll
	};
}