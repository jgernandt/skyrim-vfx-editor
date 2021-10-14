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

#pragma once
#include "gui_types.h"

//Down the road, everything defining the style (fonts, sizes, colours...) should be moved to here and
//possibly read from an external resource.
namespace node
{
	constexpr gui::ColRGBA TitleCol_Geom = { 0.9f, 0.7f, 0.5f, 1.0f };
	constexpr gui::ColRGBA TitleCol_GeomActive = { 0.9f, 0.7f, 0.5f, 1.0f };

	constexpr gui::ColRGBA TitleCol_Modifier = { 0.6f, 0.8f, 0.6f, 1.0f };
	constexpr gui::ColRGBA TitleCol_ModifierActive = { 0.6f, 0.8f, 0.6f, 1.0f };

	constexpr gui::ColRGBA TitleCol_Node = { 0.8f, 0.6f, 0.6f, 1.0f };
	constexpr gui::ColRGBA TitleCol_NodeActive = { 0.8f, 0.6f, 0.6f, 1.0f };

	constexpr gui::ColRGBA TitleCol_XData = { 0.9f, 0.9f, 0.9f, 1.0f };
	constexpr gui::ColRGBA TitleCol_XDataActive = { 0.9f, 0.9f, 0.9f, 1.0f };

	constexpr gui::ColRGBA TitleCol_Shader = { 0.6f, 0.6f, 0.8f, 1.0f };
	constexpr gui::ColRGBA TitleCol_ShaderActive = { 0.6f, 0.6f, 0.8f, 1.0f };
}