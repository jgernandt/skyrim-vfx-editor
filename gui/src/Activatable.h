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
#include <functional>

namespace gui
{
	//We might want an abstract action type instead of std::function
	//class IAction;

	class Activatable
	{
	public:
		typedef std::function<void()> action_type;

	public:
		Activatable(const action_type& action = action_type()) : m_action{ action } {}
		virtual ~Activatable() = default;

		virtual void onActivate() { if (m_action) m_action(); }

		void setAction(const action_type& action) { m_action = action; }

	private:
		action_type m_action;
	};
}