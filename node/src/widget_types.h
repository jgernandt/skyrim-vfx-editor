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
#include "gui_types.h"
#include "nif_types.h"
#include "node_traits.h"
#include "widgets.h"

namespace node
{
	using Checkbox = gui::Checkbox<bool, 1, nif::Property<bool>>;

	template<typename T, size_t N>
	using DragInput = gui::DragInput<T, N, nif::Property<T>, util::DefaultConverter, gui::DefaultLayout>;
	template<typename T, size_t N>
	using DragInputH = gui::DragInput<T, N, nif::Property<T>, util::DefaultConverter, gui::HorizontalLayout>;

	using DragFloat = DragInput<float, 1>;
	using DragInt = DragInput<int, 1>;

	using ColourInput = gui::ColourInput<nif::ColRGBA, nif::Property<nif::ColRGBA>>;

	using StringInput = gui::TextInput<nif::Property<std::string>>;
}