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
#include "gui_types.h"

namespace gui
{
	class Visitor;

	//We need some object to track certain state as we traverse the composition. Things like clip regions, current transforms.
	//Come to think of it, this object might actually be the same thing as what we have so far called a GUIEngine. It's purpose
	//is to translate our composition into vertex buffers.
	//There will also be considerable overlap with the Drawer we already have. Should be combinable somehow.
	//In any case, sending (an interface to) such an object to frame seems like the way to go.
	class FrameDrawer 
	{
	public:
		virtual ~FrameDrawer() = default;

		virtual void pushClipArea(const Floats<2>& xlims, const Floats<2>& ylims, bool intersect = true) = 0;
		virtual void popClipArea() = 0;

		virtual void pushTransform(const Floats<2>& translation, const Floats<2>& scale) = 0;
		virtual void popTransform() = 0;

		//Apply our currently pushed transform to a point
		virtual Floats<2> toGlobal(const Floats<2>&) const = 0;
		virtual Floats<2> toLocal(const Floats<2>&) const = 0;
	};

	class IComponent
	{
	public:
		virtual ~IComponent() {}

		virtual IComponent* getParent() const = 0;
		//Not for public use! Parenting should be done via add/removeChild.
		virtual void setParent(IComponent*) = 0;

		//Pass ownership of a component to us, and add it as a child
		virtual void addChild(std::unique_ptr<IComponent>&&) = 0;
		//Remove a child from us, and return ownership of it to the caller
		virtual std::unique_ptr<IComponent> removeChild(IComponent*) = 0;
		//Remove all our children and release their resources
		virtual void clearChildren() = 0;

		//Process input and draw a frame. This is somewhat ImGui-specific. It could be replaced by some general event handling system.
		virtual void frame(FrameDrawer&) = 0;

		//Local translation from our parent component
		virtual Floats<2> getTranslation() const = 0;
		virtual void setTranslation(const Floats<2>&) = 0;
		//Our local scale
		virtual Floats<2> getScale() const = 0;
		virtual void setScale(const Floats<2>&) = 0;
		//Our position in the coordinates of the native window
		virtual Floats<2> getGlobalPosition() const = 0;

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
	};

	typedef std::unique_ptr<IComponent> ComponentPtr;
}