//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include <vector>
#include "IComponent.h"

namespace gui
{
	class Component : public IComponent
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

		virtual void frame() override {}

		virtual Floats<2> getPosition() const override { return m_position; }
		virtual void setPosition(const Floats<2>& pos) override { m_position = pos; }
		virtual Floats<2> getGlobalPosition() const override;

		virtual Floats<2> getSize() const override { return m_size; }
		virtual void setSize(const Floats<2>& size) override { m_size = size; }

		virtual Floats<2> getSizeHint() const override { return m_sizeHint; }
		virtual void setSizeHint(const Floats<2>& hint) override { m_sizeHint = hint; }

		virtual void accept(Visitor& v) override;

		virtual void addChild(ComponentPtr&&) override {}
		virtual ComponentPtr removeChild(IComponent*) override { return ComponentPtr(); }
		virtual void clearChildren() override {}

		virtual IComponent* getRoot() override;

	protected:
		virtual IInvoker* getInvoker() override;

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
		Floats<2> m_position{ 0.0f, 0.0f };
		Floats<2> m_size{ 0.0f, 0.0f };
		Floats<2> m_sizeHint{ 0.0f, 0.0f };

	private:
		IComponent* m_parent{ nullptr };
	};

	class Composite : public Component
	{
	public:
		typedef std::vector<ComponentPtr> ChildList;
	public:
		Composite() {}
		virtual ~Composite();

		virtual void frame() override;

		virtual void accept(Visitor& v) override;

		virtual void addChild(ComponentPtr&& c) override;
		virtual ComponentPtr removeChild(IComponent* c) override;
		virtual void clearChildren() override;

		template<typename T, typename ...Args> T* newChild(Args&& ... args);
		ChildList& getChildren() { return m_children; }
		const ChildList& getChildren() const { return m_children; }

	private:
		ChildList m_children;
	};

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

		virtual void addChild(ComponentPtr&& c) final override;
		virtual ComponentPtr removeChild(IComponent* c) final override;
		virtual void clearChildren() final override;

		virtual void frame() override;

		virtual Floats<2> getPosition() const override;
		virtual void setPosition(const Floats<2>& pos) override;
		virtual Floats<2> getGlobalPosition() const override;

		virtual Floats<2> getSize() const override;
		virtual void setSize(const Floats<2>& size) override;

		virtual Floats<2> getSizeHint() const override;
		virtual void setSizeHint(const Floats<2>& hint) override;

		virtual void accept(Visitor& v) override;

		virtual IInvoker* getInvoker() override;
		virtual IComponent* getRoot() override;

	protected:
		ComponentPtr m_component;

	private:
		IComponent* m_parent{ nullptr };
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
}