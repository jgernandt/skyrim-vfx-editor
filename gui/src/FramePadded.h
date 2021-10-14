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
	//Add frame padding to the decorated object. Intended for use with text or other fixed-size components.
	class FramePadded final :
		public ComponentDecorator
	{
	public:
		FramePadded(ComponentPtr&& c) : ComponentDecorator(std::move(c)) {}

		virtual void frame() override;
		virtual Floats<2> getSizeHint() const override;

	};
}
