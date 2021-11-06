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

#include "CallWrapper.h"

//We should adopt a global strategy for pixel rounding
constexpr float (*TO_PIXEL)(float) noexcept = &std::floor;

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
		virtual bool isMouseDown(MouseButton) const = 0;
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

	class IComponent
	{
	public:
		virtual ~IComponent() {}

		virtual IComponent* getParent() const = 0;
		//Not for public use! Parenting should be done via add/removeChild.
		virtual void setParent(IComponent*) = 0;
		virtual bool hasAncestor(IComponent*) const = 0;

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
		virtual void setTranslationX(float) = 0;
		virtual void setTranslationY(float) = 0;
		//Our local scale
		virtual Floats<2> getScale() const = 0;
		virtual void setScale(const Floats<2>&) = 0;
		virtual void setScaleX(float) = 0;
		virtual void setScaleY(float) = 0;
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
	};

	typedef std::unique_ptr<IComponent> ComponentPtr;
}