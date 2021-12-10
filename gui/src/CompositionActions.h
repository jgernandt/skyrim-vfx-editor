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
#include "ICommand.h"
#include "IComponent.h"

namespace gui
{
	class AddChild final : public ICommand
	{
	public:
		AddChild(ComponentPtr&& child, IComponent* parent, bool reversible) :
			m_parent{ parent }, m_child{ child.get() }, m_component{ std::move(child) }, m_reversible{ reversible }
		{}

		virtual void execute() override
		{
			if (m_parent)
				m_parent->addChild(std::move(m_component));
		}
		virtual void reverse() override
		{
			assert(m_reversible);
			if (m_parent)
				m_component = m_parent->removeChild(m_child);
		}
		virtual bool reversible() const override { return m_reversible; }
	private:
		IComponent* m_parent;
		IComponent* m_child;
		ComponentPtr m_component;
		bool m_reversible;
	};

	class RemoveChild final : public ICommand
	{
	public:
		RemoveChild(IComponent* child, IComponent* parent, bool reversible) :
			m_parent{ parent }, m_child{ child }, m_reversible{ reversible } {}
		virtual void execute() override
		{
			if (m_parent)
				m_stored = m_parent->removeChild(m_child);
		}
		virtual void reverse() override
		{
			assert(m_reversible);
			if (m_parent)
				m_parent->addChild(std::move(m_stored));
		}
		virtual bool reversible() const override  { return m_reversible; }
	private:
		IComponent* m_parent;
		IComponent* m_child;
		ComponentPtr m_stored;//for redoing
		bool m_reversible;
	};

	class MoveChild final : public ICommand
	{
	public:
		MoveChild(IComponent* child, IComponent* from, IComponent* to, bool reversible) :
			m_child{ child }, m_from{ from }, m_to{ to }, m_reversible{ reversible } {}
		virtual void execute() override
		{
			if (m_from && m_to)
				m_to->addChild(m_from->removeChild(m_child));
		}
		virtual void reverse() override
		{
			assert(m_reversible);
			if (m_from && m_to)
				m_from->addChild(m_to->removeChild(m_child));
		}
		virtual bool reversible() const override { return m_reversible; }
	private:
		IComponent* m_child;
		IComponent* m_from;
		IComponent* m_to;
		bool m_reversible;
	};

	class InsertChild final : public ICommand
	{
	public:
		InsertChild(ComponentPtr&& child, IComponent* parent, int pos) :
			m_parent{ parent }, m_child{ std::move(child) }, m_pos{ pos }
		{}

		virtual void execute() override
		{
			if (m_parent)
				m_parent->insertChild(m_pos, std::move(m_child));
		}
		virtual void reverse() override {}
		virtual bool reversible() const override { return false; }
	private:
		IComponent* m_parent;
		ComponentPtr m_child;
		int m_pos;
	};
}