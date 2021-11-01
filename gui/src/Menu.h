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
#include "Activatable.h"
#include "Widget.h"

//This menu solution isn't great (permanently instantiating all menus), 
//but let's worry about that later.

namespace gui
{
	class MenuItem :
		public Activatable, public Component
	{
	public:
		MenuItem(const std::string& label, const action_type& action = action_type()) :
			Activatable(action), m_label{ label } {}
		virtual void frame(FrameDrawer& fd) override;

	private:
		std::string m_label;
	};

	class Menu : public Composite
	{
	public:
		Menu(const std::string& label) : m_label{ label } {}
		virtual void frame(FrameDrawer& fd) override;

	private:
		std::string m_label;
	};

	class MainMenu : public Menu
	{
	public:
		MainMenu(const std::string& label) : Menu{ label } {}
		virtual void frame(FrameDrawer& fd) override;

	};
}