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
#include "Composition.h"

namespace gui
{
	class Separator final : public Component
	{
	public:
		Separator() = default;
		virtual void frame() override;
	};

	class VerticalSpacing final : public Component
	{
	public:
		VerticalSpacing(int n = 1) : m_N{n} {}
		virtual void frame() override;

	private:
		const int m_N;
	};
}