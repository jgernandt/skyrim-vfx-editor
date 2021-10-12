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
#include "ICommand.h"
#include "IComponent.h"

namespace gui
{
	class AddChild final :
		public ICommand
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

	class RemoveChild final :
		public ICommand
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
}