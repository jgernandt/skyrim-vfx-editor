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
#include <stack>
#include "gui_types.h"

namespace gui
{
	enum class Layer
	{
		BACKGROUND,
		FOREGROUND,
		WINDOW,
	};

	//Doesn't really make sense to encapsulate this in an object when it's calling a global anyway.
	//Still, it hides the backend.
	class Drawer
	{
	public:
		Drawer() {}
		~Drawer();

		//call before and after any drawing
		void begin();
		void end();

		void setTargetLayer(Layer l) { assert(!m_drawing); m_layer = l; }

		void pushClipArea(float x1, float y1, float x2, float y2);
		void pushClipArea(const Floats<2>& xlims, const Floats<2>& ylims);
		void popClipArea();

		void pushTransform(float tx, float ty, float sx, float sy);
		void pushTransform(const Floats<2>& translation, const Floats<2>& scale);
		void popTransform();

		void line(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col);
		void rectangle(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col);
		void rectangleGradient(const Floats<2>& p1, const Floats<2>& p2, 
			const ColRGBA& tl, const ColRGBA& tr, const ColRGBA& bl, const ColRGBA& br);
		void triangle(const Floats<2>& p1, const Floats<2>& p2, const Floats<2>& p3, const ColRGBA& col);

	private:
		std::stack<Floats<4>> m_clipArea;
		std::stack<Floats<4>> m_transform;

		Layer m_layer{ Layer::WINDOW };

		bool m_drawing{ false };
	};
}