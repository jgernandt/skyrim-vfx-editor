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

bool gui::Component::hasAncestor(IComponent* c) const
{
	return (m_parent && c) ? m_parent == c || m_parent->hasAncestor(c) : false;
}

gui::Floats<2> gui::Component::getGlobalPosition() const
{
	return toGlobalSpace({ 0.0f, 0.0f });
}

gui::Floats<2> gui::Component::toGlobalSpace(const Floats<2>& p) const
{
	Floats<2> pp = toParentSpace(p);
	return m_parent ? m_parent->toGlobalSpace(pp) : pp;
}

gui::Floats<2> gui::Component::toParentSpace(const Floats<2>& p) const
{
	return m_translation + p * m_scale;
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
	fd.pushTransform(m_translation, m_scale);
	for (auto& c : m_children) {
		assert(c);
		c->frame(fd);
	}
	fd.popTransform();
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
	for (auto&& child : m_children) {
		assert(child);
		child->setParent(nullptr);
	}
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

bool gui::ComponentDecorator::hasAncestor(IComponent* c) const
{
	return (m_parent && c) ? m_parent == c || m_parent->hasAncestor(c) : false;
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

gui::Floats<2> gui::ComponentDecorator::getTranslation() const
{
	assert(m_component);
	return m_component->getTranslation();
}

void gui::ComponentDecorator::setTranslation(const Floats<2>& t)
{
	assert(m_component);
	m_component->setTranslation(t);
}

void gui::ComponentDecorator::setTranslationX(float x)
{
	assert(m_component);
	m_component->setTranslationX(x);
}

void gui::ComponentDecorator::setTranslationY(float y)
{
	assert(m_component);
	m_component->setTranslationY(y);
}

gui::Floats<2> gui::ComponentDecorator::getScale() const
{
	assert(m_component);
	return m_component->getScale();
}

void gui::ComponentDecorator::setScale(const Floats<2>& s)
{
	assert(m_component);
	m_component->setScale(s);
}

void gui::ComponentDecorator::setScaleX(float x)
{
	assert(m_component);
	m_component->setScaleX(x);
}

void gui::ComponentDecorator::setScaleY(float y)
{
	assert(m_component);
	m_component->setScaleY(y);
}

gui::Floats<2> gui::ComponentDecorator::getGlobalPosition() const
{
	assert(m_component);
	return m_component->getGlobalPosition();
}

gui::Floats<2> gui::ComponentDecorator::toGlobalSpace(const Floats<2>& p) const
{
	assert(m_component);
	return m_component->toGlobalSpace(p);
}

gui::Floats<2> gui::ComponentDecorator::toParentSpace(const Floats<2>& p) const
{
	assert(m_component);
	return m_component->toParentSpace(p);
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
