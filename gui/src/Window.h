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
#include "Composition.h"
#include "UniqueLabel.h"

namespace gui
{
	class Window :
		public Composite
	{
	public:
		enum Colour : size_t
		{
			COL_TITLE,
			COL_TITLE_ACTIVE,
			COL_POPUP,//Doesn't make sense to set from here? Even if it's a child popup?
			/*COL_BACKGROUND,
			COL_BORDER,
			COL_TEXT,
			COL_BUTTON,
			COL_BUTTON_HOVER,
			COL_BUTTON_ACTIVE,
			COL_FRAME,
			COL_FRAME_HOVER,
			COL_FRAME_ACTIVE,*/
			COL_SIZE,
		};

	public:
		Window(const std::string& title = std::string());
		virtual ~Window() {}

		virtual void accept(Visitor& v) override;
		virtual void frame(FrameDrawer& fd) override;
		virtual void setTranslation(const Floats<2>& t) override;

		virtual void onClose();

		void setClosable(bool set = true) { m_closable = set; }
		void setTitle(const std::string& s) { m_title.reset(s); }
		void setColour(Colour i, const ColRGBA& col) { m_colours[i] = col; }

	private:
		UniqueLabel<1> m_title;
		ColRGBA m_colours[COL_SIZE]{ ColRGBA() };
		unsigned int m_style{ 0 };
		bool m_closable{ false };

		Floats<2> m_lastGlobalPos{ 0.0f, 0.0f };
	};
}