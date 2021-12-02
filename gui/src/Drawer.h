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
#include <cassert>
#include <stack>
#include "gui_types.h"
#include "CallWrapper.h"
#include "input.h"

struct ImDrawList;

namespace gui
{
	enum class Layer
	{
		BACKGROUND,
		FOREGROUND,
		WINDOW,
	};

	ImDrawList* getDrawList(gui::Layer l);

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

		//void pushClipArea(float x1, float y1, float x2, float y2);
		void pushClipArea(const Floats<2>& p1, const Floats<2>& p2);
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

	class FrameDrawer
	{
	public:
		virtual ~FrameDrawer() = default;

		//Drawing
		virtual void setTargetLayer(Layer) = 0;

		virtual void circle(const Floats<2>& centre, float radius, const ColRGBA& col, bool global = false) = 0;
		virtual void curve(const std::vector<gui::Floats<2>>& data, const ColRGBA& col, float width = 1.0f, bool global = false) = 0;
		virtual void line(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col, float width = 1.0f, bool global = false) = 0;
		virtual void rectangle(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col, bool global = false) = 0;
		virtual void rectangleGradient(const Floats<2>& p1, const Floats<2>& p2,
			const ColRGBA& tl, const ColRGBA& tr, const ColRGBA& bl, const ColRGBA& br, bool global = false) = 0;
		virtual void triangle(const Floats<2>& p1, const Floats<2>& p2, const Floats<2>& p3, const ColRGBA& col, bool global = false) = 0;

		//Push a clip region to use for future draw calls. Returns an object that restores the previous region on destruction.
		[[nodiscard]] virtual util::CallWrapper pushClipArea(const Floats<2>& p1, const Floats<2>& p2, bool intersect = true) = 0;

		//Push a transform to use for future draw/transform calls. Returns an object that restores the previous transform on destruction.
		[[nodiscard]] virtual util::CallWrapper pushTransform(const Floats<2>& translation, const Floats<2>& scale) = 0;

		virtual Floats<2> getCurrentTranslation() const = 0;
		virtual Floats<2> getCurrentScale() const = 0;

		//Apply our currently pushed transform to a point
		virtual Floats<2> toGlobal(const Floats<2>&) const = 0;
		virtual Floats<2> toLocal(const Floats<2>&) const = 0;

		//load font of given scale for next frame
		virtual void loadFontScale(float) = 0;
		virtual void pushUIScale(float) = 0;
		virtual void popUIScale() = 0;

		//Do we really want inputs here? Seems like we're using this class to generally hide imgui's global nature,
		//which isn't necessarily a good idea.
		virtual bool isMouseDown(Mouse::Button) const = 0;
		virtual Floats<2> getMouseMove() const = 0;
		virtual Floats<2> getMousePosition() const = 0;
		virtual float getWheelDelta() const = 0;

		//To report that a component has handled input (don't like it!)
		virtual bool isWheelHandled() const = 0;
		virtual void setWheelHandled() = 0;
		//More generally, if this is something we want to pursue:
		//get the component that is currently capturing the button (if any)
		//virtual IComponent* getCaptured(MouseButton) const = 0;
		//have a component capture/release the given button
		//virtual void capture(MouseButton, IComponent&) = 0;
		//virtual void release(MouseButton, IComponent&) = 0;
	};
}