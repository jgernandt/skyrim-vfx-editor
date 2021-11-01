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

#include "pch.h"
#include "Switch.h"

gui::Switch::Switch(const std::function<int()>& cdn) : m_condition{ cdn }
{
}

void gui::Switch::frame(FrameDrawer& fd)
{
    if (int n = m_condition ? m_condition() : -1; n < static_cast<int>(getChildren().size()) && n >= 0) {
        assert(getChildren()[n]);
        getChildren()[n]->frame(fd);
    }
}

gui::Floats<2> gui::Switch::getSizeHint() const
{
    if (int n = m_condition ? m_condition() : -1; n < static_cast<int>(getChildren().size()) && n >= 0) {
        assert(getChildren()[n]);
        return getChildren()[n]->getSizeHint();
    }
    else
        return { 0.0f, 0.0f };
}
