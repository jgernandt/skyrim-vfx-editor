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

#include "pch.h"
#include "Composition.h"
#include "Visitor.h"

gui::Component::~Component()
{
	assert(!m_parent);
}

gui::Floats<2> gui::Component::getGlobalPosition() const
{
	Floats<2> p = m_parent ? m_parent->getGlobalPosition() : Floats<2>{ 0.0f, 0.0f };
	return { p[0] + m_position[0], p[1] + m_position[1] };
}

void gui::Component::accept(Visitor& v)
{
	v.visit(*this);
}

gui::IInvoker* gui::Component::getInvoker()
{
	return m_parent ? m_parent->getInvoker() : nullptr;
}

gui::IComponent* gui::Component::getRoot()
{
	return m_parent ? m_parent->getRoot() : this;
}

gui::Composite::~Composite()
{
	for (auto& c : m_children) {
		assert(c);
		c->setParent(nullptr);
	}
}

void gui::Composite::frame(FrameDrawer& fd)
{
	for (auto& c : m_children) {
		assert(c);
		c->frame(fd);
	}
}

void gui::Composite::accept(Visitor& v)
{
	v.visit(*this);
}

void gui::Composite::addChild(ComponentPtr&& c)
{
	if (c) {
		//ptr must not have a parent (we do not check for duplicate children, though)
		assert(!c->getParent());
		c->setParent(this);
		m_children.push_back(std::move(c));
	}
}

gui::ComponentPtr gui::Composite::removeChild(IComponent* c)
{
	ComponentPtr ret;

	auto pred = [&](const ComponentPtr& p) { return p.get() == c; };
	if (auto it = std::find_if(m_children.begin(), m_children.end(), pred); it != m_children.end()) {
		assert(c);
		c->setParent(nullptr);
		ret = std::move(*it);
		m_children.erase(it);
	}

	return ret;
}

void gui::Composite::clearChildren()
{
	for (auto&& child : m_children)
		child->setParent(nullptr);
	m_children.clear();
}

gui::ComponentDecorator::ComponentDecorator(ComponentPtr&& c) : m_component{ std::move(c) }
{
	assert(m_component && !m_component->getParent());
	m_component->setParent(this);
}

gui::ComponentDecorator::~ComponentDecorator()
{
	assert(m_component && !m_parent);
	m_component->setParent(nullptr);
}

void gui::ComponentDecorator::addChild(ComponentPtr&& c)
{
	assert(m_component);
	m_component->addChild(std::move(c));
}

gui::ComponentPtr gui::ComponentDecorator::removeChild(IComponent* c)
{
	assert(m_component);
	if (c == m_component.get() && m_parent)
		//this call should be coming from the component itself
		return m_parent->removeChild(this);
	else
		return m_component->removeChild(c);
}

void gui::ComponentDecorator::clearChildren()
{
	assert(m_component);
	m_component->clearChildren();
}

void gui::ComponentDecorator::frame(FrameDrawer& fd)
{
	assert(m_component);
	m_component->frame(fd);
}

gui::Floats<2> gui::ComponentDecorator::getPosition() const
{
	assert(m_component);
	return m_component->getPosition();
}

void gui::ComponentDecorator::setPosition(const Floats<2>& pos)
{
	assert(m_component);
	m_component->setPosition(pos);
}

gui::Floats<2> gui::ComponentDecorator::getGlobalPosition() const
{
	assert(m_component);
	return m_component->getGlobalPosition();
}

gui::Floats<2> gui::ComponentDecorator::getSize() const
{
	assert(m_component);
	return m_component->getSize();
}

void gui::ComponentDecorator::setSize(const Floats<2>& size)
{
	assert(m_component);
	m_component->setSize(size);
}

gui::Floats<2> gui::ComponentDecorator::getSizeHint() const
{
	assert(m_component);
	return m_component->getSizeHint();
}

void gui::ComponentDecorator::setSizeHint(const Floats<2>& hint)
{
	assert(m_component);
	m_component->setSizeHint(hint);
}

void gui::ComponentDecorator::accept(Visitor& v)
{
	assert(m_component);
	m_component->accept(v);
}

gui::IInvoker* gui::ComponentDecorator::getInvoker()
{
	return m_parent ? m_parent->getInvoker() : nullptr;
}

gui::IComponent* gui::ComponentDecorator::getRoot()
{
	return m_parent ? m_parent->getRoot() : this;
}
