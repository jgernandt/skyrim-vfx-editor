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
#include <memory>
#include "IInvoker.h"
#include "input.h"
#include "gui_types.h"
#include "Drawer.h"

//We should adopt a global strategy for pixel rounding
constexpr float (*TO_PIXEL)(float) noexcept = &std::floor;

#ifdef _DEBUG
extern int g_currentComponents;
#endif

namespace gui
{
	class Visitor;

	class IComponent
	{
	public:
#ifdef _DEBUG
		IComponent() { g_currentComponents++; }
		virtual ~IComponent() { g_currentComponents--; }
#else
		virtual ~IComponent() = default;
#endif

		virtual IComponent* getParent() const = 0;
		//Not for public use! Parenting should be done via add/removeChild.
		virtual void setParent(IComponent*) = 0;
		virtual bool hasAncestor(IComponent*) const = 0;

		//Pass ownership of a component to us, and add it as a child
		virtual void addChild(std::unique_ptr<IComponent>&&) = 0;
		//Add/remove child in given position (use iterator instead?)
		virtual void insertChild(int pos, std::unique_ptr<IComponent>&&) = 0;
		virtual void eraseChild(int pos) = 0;
		//Move a child in focus order (effectively erase-insert)
		virtual void moveChild(int pos, int to) = 0;
		//Remove a child from us, and return ownership of it to the caller
		virtual std::unique_ptr<IComponent> removeChild(IComponent*) = 0;
		//Remove all our children and release their resources
		virtual void clearChildren() = 0;

		//Process input and draw a frame. This is somewhat ImGui-specific. It could be replaced by some general event handling system.
		virtual void frame(FrameDrawer&) = 0;

		//Local translation from our parent component
		virtual Floats<2> getTranslation() const = 0;
		virtual void setTranslation(const Floats<2>&) = 0;
		virtual void setTranslationX(float) = 0;
		virtual void setTranslationY(float) = 0;
		virtual void translate(const Floats<2>&) = 0;
		//Our local scale
		virtual Floats<2> getScale() const = 0;
		virtual void setScale(const Floats<2>&) = 0;
		virtual void setScaleX(float) = 0;
		virtual void setScaleY(float) = 0;
		virtual void scale(const Floats<2>&) = 0;
		//Our position in the coordinates of the native window (short for toGlobalSpace({0.0f, 0.0f}))
		virtual Floats<2> getGlobalPosition() const = 0;
		//Transform a point in our space to the space of the native window
		virtual Floats<2> toGlobalSpace(const Floats<2>&) const = 0;
		//Transform a point in our space to the space of our parent
		virtual Floats<2> toParentSpace(const Floats<2>&) const = 0;
		//Transform a point in the space of the native window to our space
		virtual Floats<2> fromGlobalSpace(const Floats<2>&) const = 0;
		//Transform a point in the space of our parent to our space
		virtual Floats<2> fromParentSpace(const Floats<2>&) const = 0;

		//Actual size of component. Decided at runtime.
		virtual Floats<2> getSize() const = 0;
		virtual void setSize(const Floats<2>&) = 0;//maybe this should not be public, if it is a calculated value

		//Hint at the desired size of this component. Used by layout operators.
		virtual Floats<2> getSizeHint() const = 0;
		virtual void setSizeHint(const Floats<2>&) = 0;
		//(This size system isn't fully implemented yet. I'm not convinced by it.)

		virtual void accept(Visitor&) = 0;

		virtual IInvoker* getInvoker() = 0;
		virtual IComponent* getRoot() = 0;

		virtual void setMouseHandler(MouseHandler*) = 0;
	};

	typedef std::unique_ptr<IComponent> ComponentPtr;
}