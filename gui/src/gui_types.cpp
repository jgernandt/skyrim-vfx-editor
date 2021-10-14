//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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

ImVec2 gui::GuiConverter<ImVec2>::convert(const std::array<float, 2>& f)
{
	return ImVec2(f[0], f[1]);
}

ImVec4 gui::GuiConverter<ImVec4>::convert(const std::array<float, 4>& f)
{
	return ImVec4(f[0], f[1], f[2], f[3]);
}


/*std::array<float, 2> util::convert_to<std::array<float, 2>>::from(const ImVec2& f)
{
	return { f.x, f.y };
}

std::array<float, 4> util::convert_to<std::array<float, 4>>::from(const ImVec4& f)
{
	return { f.x, f.y, f.z, f.w };
}

ImVec2 util::convert_to<ImVec2>::from(const std::array<float, 2>& f)
{
	return ImVec2(f[0], f[1]);
}

ImVec4 util::convert_to<ImVec4>::from(const std::array<float, 4>& f)
{
	return ImVec4(f[0], f[1], f[2], f[3]);
}*/