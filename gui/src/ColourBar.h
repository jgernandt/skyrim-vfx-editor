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
#include <functional>
#include "Widget.h"
#include "Drawer.h"

namespace gui
{
	class ColourGradient
	{
	public:
		virtual ~ColourGradient() = default;
		virtual void draw(Drawer&) const = 0;
	};

	class ColourBar : public Component
	{
	public:

		ColourBar(std::unique_ptr<ColourGradient>&& gradient) : m_gradient{ std::move(gradient) } {}
		virtual ~ColourBar() = default;

		virtual void frame() override;

	private:
		std::unique_ptr<ColourGradient> m_gradient;

	};
}