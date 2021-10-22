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
#include "gui_types.h"

std::array<float, 2> gui::GuiConverter<std::array<float, 2>>::convert(const ImVec2& f)
{
	return { f.x, f.y };
}

std::array<float, 4> gui::GuiConverter<std::array<float, 4>>::convert(const ImVec4& f)
{
	return { f.x, f.y, f.z, f.w };
}

Eigen::Array<float, 2, 1> gui::GuiConverter<Eigen::Array<float, 2, 1>>::convert(const ImVec2& f)
{
	return { f.x, f.y };
}

Eigen::Array<float, 4, 1> gui::GuiConverter<Eigen::Array<float, 4, 1>>::convert(const ImVec4& f)
{
	return { f.x, f.y, f.z, f.w };
}

Eigen::Vector<float, 2> gui::GuiConverter<Eigen::Vector<float, 2>>::convert(const ImVec2& f)
{
	return { f.x, f.y };
}

Eigen::Vector<float, 4> gui::GuiConverter<Eigen::Vector<float, 4>>::convert(const ImVec4& f)
{
	return { f.x, f.y, f.z, f.w };
}

ImVec2 gui::GuiConverter<ImVec2>::convert(const std::array<float, 2>& f)
{
	return ImVec2(f[0], f[1]);
}

ImVec2 gui::GuiConverter<ImVec2>::convert(const Eigen::Array<float, 2, 1>& f)
{
	return ImVec2(f[0], f[1]);
}

ImVec2 gui::GuiConverter<ImVec2>::convert(const Eigen::Vector<float, 2>& f)
{
	return ImVec2(f[0], f[1]);
}

ImVec4 gui::GuiConverter<ImVec4>::convert(const std::array<float, 4>& f)
{
	return ImVec4(f[0], f[1], f[2], f[3]);
}

ImVec4 gui::GuiConverter<ImVec4>::convert(const Eigen::Array<float, 4, 1>& f)
{
	return ImVec4(f[0], f[1], f[2], f[3]);
}

ImVec4 gui::GuiConverter<ImVec4>::convert(const Eigen::Vector<float, 4>& f)
{
	return ImVec4(f[0], f[1], f[2], f[3]);
}