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
#include "widgets.h"

namespace app
{
	class AboutBox final : public gui::Modal
	{
	public:
		AboutBox();

        void openGPL()
        {
            if (m_gplDetail)
                m_gplDetail->open();
        }

        void openImGui()
        {
            if (m_imguiDetail)
                m_imguiDetail->open();
        }

        void openNiflib()
        {
            if (m_niflibDetail)
                m_niflibDetail->open();
        }

	private:
		gui::Modal* m_gplDetail{ nullptr };
		gui::Modal* m_imguiDetail{ nullptr };
		gui::Modal* m_niflibDetail{ nullptr };
	};
}