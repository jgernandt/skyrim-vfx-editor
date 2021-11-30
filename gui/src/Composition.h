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
#include <vector>
#include "Observable.h"
#include "IComponent.h"

namespace gui
{
	class Component;
}

template<>
struct Event<gui::Component>
{
	enum {
		ADD_CHILD,
		REMOVE_CHILD,

	} type{ ADD_CHILD };

	gui::Component* source{ nullptr };
	gui::IComponent* component{ nullptr };
};

template<>
class IListener<gui::Component>
{
public:
	virtual ~IListener() = default;

	void receive(const Event<gui::Component>& e, Observable<gui::Component>&)
	{
		switch (e.type) {
		case Event<gui::Component>::ADD_CHILD:
			onAddChild(e.component, e.source);
			break;
		case Event<gui::Component>::REMOVE_CHILD:
			onRemoveChild(e.component, e.source);
			break;
		}
	}

	virtual void onAddChild(gui::IComponent* c, gui::Component* source) {}
	//c may have been destroyed when sending this. Stupid?
	virtual void onRemoveChild(gui::IComponent* c, gui::Component* source) {}
};

namespace gui
{
	using ComponentListener = IListener<gui::Component>;

	class Component : public IComponent, public Observable<Component>
	{
	public:
		Component() {}
		Component(const Component&) = delete;
		Component(Component&&) = delete;

		virtual ~Component();

		Component& operator=(const Component&) = delete;
		Component& operator=(Component&&) = delete;

		virtual IComponent* getParent() const override { return m_parent; }
		virtual void setParent(IComponent* c) override { m_parent = c; }
		virtual bool hasAncestor(IComponent* c) const override;

		virtual void addChild(ComponentPtr&&) override {}
		virtual void insertChild(int pos, std::unique_ptr<IComponent>&&) override {}
		virtual void eraseChild(int pos) override {}
		virtual ComponentPtr removeChild(IComponent*) override { return ComponentPtr(); }
		virtual void clearChildren() override {}

		virtual void frame(FrameDrawer& fd) override;

		virtual Floats<2> getTranslation() const override { return m_translation; }
		virtual void setTranslation(const Floats<2>& t) override { m_translation = t; }
		virtual void setTranslationX(float x) override { m_translation[0] = x; }
		virtual void setTranslationY(float y) override { m_translation[1] = y; }
		virtual void translate(const Floats<2>& t) override { m_translation += t; }

		virtual Floats<2> getScale() const override { return m_scale; }
		virtual void setScale(const Floats<2>& s) override { m_scale = s; }
		virtual void setScaleX(float x) override { m_scale[0] = x; }
		virtual void setScaleY(float y) override { m_scale[1] = y; }
		virtual void scale(const Floats<2>& s) override { m_scale *= s; }

		virtual Floats<2> getGlobalPosition() const override;
		virtual Floats<2> toGlobalSpace(const Floats<2>& p) const override;
		virtual Floats<2> toParentSpace(const Floats<2>& p) const override;
		virtual Floats<2> fromGlobalSpace(const Floats<2>& p) const override;
		virtual Floats<2> fromParentSpace(const Floats<2>& p) const override;

		virtual Floats<2> getSize() const override { return m_size; }
		virtual void setSize(const Floats<2>& size) override { m_size = size; }

		virtual Floats<2> getSizeHint() const override { return m_sizeHint; }
		virtual void setSizeHint(const Floats<2>& hint) override { m_sizeHint = hint; }

		virtual void accept(Visitor& v) override;

		virtual IComponent* getRoot() override;

		virtual void setMouseHandler(MouseHandler* h) override { m_mouseHandler = h; }

		virtual void setFocussed(bool) override {}
		virtual void setSelected(bool) override {}

	protected:
		virtual IInvoker* getInvoker() override;

		void handleMouse(FrameDrawer& fd);
		void receiveMouseInput(FrameDrawer& fd, unsigned int imgui_id);
		bool isHovered(FrameDrawer& fd);

		//Send the given command to our invoker, if we have one. Else, invoke it ourselves.
		template<typename CmdType, typename... Params>
		void asyncInvoke(Params&&... params) 
		{
			if (IInvoker* inv = getInvoker())
				inv->queue(std::make_unique<CmdType>(std::forward<Params>(params)...));
			else {
				CmdType cmd(std::forward<Params>(params)...);
				cmd.execute();
			}

		}

	protected:
		Floats<2> m_translation{ 0.0f, 0.0f };
		Floats<2> m_scale{ 1.0f, 1.0f };
		Floats<2> m_size{ 0.0f, 0.0f };
		Floats<2> m_sizeHint{ 0.0f, 0.0f };

	private:
		IComponent* m_parent{ nullptr };
		MouseHandler* m_mouseHandler{ nullptr };
		bool m_mouseFocus{ false };
	};

	class Composite : public Component
	{
	public:
		typedef std::vector<ComponentPtr> ChildList;
	public:
		Composite() {}
		virtual ~Composite();

		virtual void frame(FrameDrawer& fd) override;

		virtual void accept(Visitor& v) override;

		virtual void addChild(ComponentPtr&& c) override;
		virtual void insertChild(int pos, std::unique_ptr<IComponent>&& c) override;
		virtual void eraseChild(int pos) override;
		virtual ComponentPtr removeChild(IComponent* c) override;
		virtual void clearChildren() override;

		template<typename T, typename ...Args> T* newChild(Args&& ... args);
		ChildList& getChildren() { return m_children; }
		const ChildList& getChildren() const { return m_children; }

	private:
		ChildList m_children;
	};

	template<typename T, typename ...Args>
	inline T* Composite::newChild(Args&& ...args)
	{
		T* ret = new T(std::forward<Args>(args) ...);

		//we dont want this to be overridden, as that might invalidate the returned pointer
		Composite::addChild(std::unique_ptr<IComponent>(ret));
		//If addChild failed somehow, the returned pointer would be invalid.
		//How could it fail? It's not null and we don't already own it.
		//The only way is if the push_back allocation fails, but that will throw.
		//Should be no risk, then.
		return ret;
	}

	/* Discontinue updating this. We were barely using it, and it's extra work until the 
	* interface has settled in.
	class ComponentDecorator : public IComponent
	{
	public:
		ComponentDecorator(ComponentPtr&& c);
		ComponentDecorator(const ComponentDecorator&) = delete;
		ComponentDecorator(ComponentDecorator&&) = delete;

		virtual ~ComponentDecorator();

		ComponentDecorator& operator=(const ComponentDecorator&) = delete;
		ComponentDecorator& operator=(ComponentDecorator&&) = delete;

		virtual IComponent* getParent() const override { return m_parent; }
		virtual void setParent(IComponent* c) override { m_parent = c; }
		virtual bool hasAncestor(IComponent* c) const override;

		virtual void addChild(ComponentPtr&& c) final override;
		virtual ComponentPtr removeChild(IComponent* c) final override;
		virtual void clearChildren() final override;

		virtual void frame(FrameDrawer& fd) override;

		virtual Floats<2> getTranslation() const override;
		virtual void setTranslation(const Floats<2>& t) override;
		virtual void setTranslationX(float x) override;
		virtual void setTranslationY(float y) override;
		virtual void translate(const Floats<2>& t) override;

		virtual Floats<2> getScale() const override;
		virtual void setScale(const Floats<2>& s) override;
		virtual void setScaleX(float x) override;
		virtual void setScaleY(float y) override;
		virtual void scale(const Floats<2>& s) override;

		virtual Floats<2> getGlobalPosition() const override;
		virtual Floats<2> toGlobalSpace(const Floats<2>& p) const override;
		virtual Floats<2> toParentSpace(const Floats<2>& p) const override;
		virtual Floats<2> fromGlobalSpace(const Floats<2>& p) const override;
		virtual Floats<2> fromParentSpace(const Floats<2>& p) const override;

		virtual Floats<2> getSize() const override;
		virtual void setSize(const Floats<2>& size) override;

		virtual Floats<2> getSizeHint() const override;
		virtual void setSizeHint(const Floats<2>& hint) override;

		virtual void accept(Visitor& v) override;

		virtual IInvoker* getInvoker() override;
		virtual IComponent* getRoot() override;

		virtual void setMouseHandler(MouseHandler* h) override;

	protected:
		ComponentPtr m_component;

	private:
		IComponent* m_parent{ nullptr };
	};
	*/
}