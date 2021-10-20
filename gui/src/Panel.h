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
	//A free-standing container for other widgets. Unlike Window, has no decorations and cannot be moved.
	class Panel : public Composite
	{
	public:
		Panel() {}
		virtual ~Panel() {}
		virtual void frame(FrameDrawer& fd) override;
		//virtual void setSizeHint(const Floats<2>& size) override;

		//No one can be referencing this popup, as it may be arbitrarily deleted by us.
		void setContextMenu(std::unique_ptr<Popup>&& menu);//should be a general Widget mechanic?

	private:
		UniqueLabel<1> m_label;
		Popup* m_contextMenu{ nullptr };
	};
}