//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include "NodeBase.h"
#include "ICommand.h"
#include "Visitor.h"
#include "CallWrapper.h"
#include "style.h"
#include "widgets.h"

//We should be able to leave the safe disconnection to the node itself.
//Let's fix that sometime.
class DestroyAction final :
	public gui::ICommand
{
public:
	DestroyAction(gui::Component& node) : m_node{ &node } {}

	virtual void execute() override
	{
		assert(m_node);
		//if, somehow, this node is a root, we cannot touch it (should not be possible, though)
		m_parent = m_node->getParent();
		if (m_parent) {
			gui::Disconnector d;
			m_node->accept(d);
			d.execute();
			m_stateChanges = d.takeStateChanges();

			//Ideally, we should make sure we are allowed to remove the component before
			//we disconnect it, but right now some devices require the node to still be in the graph.
			m_component = m_parent->removeChild(m_node);
			//We'll know we goofed if we have a null component. Should we try to fix it, or throw?
			assert(m_component);
		}
	}

	virtual void reverse() override
	{
		if (m_component && m_parent) {
			m_parent->addChild(std::move(m_component));
			for (auto& item : m_stateChanges) {
				assert(item.first.first);
				item.first.first->setConnectionState(item.first.second, !item.second);
			}
		}
	}

	virtual bool reversible() const override
	{
		//return m_component; //??
		return true;
	}

private:
	gui::IComponent* m_node;
	gui::IComponent* m_parent{ nullptr };
	gui::ComponentPtr m_component;//keep it alive here
	gui::Connector::StateMap m_stateChanges;
};

node::NodeBase::NodeBase(std::unique_ptr<nif::NiObject>&& obj) :
	Window(std::string()), m_leftCtlr(*this), m_rightCtlr(*this)
{
	assert(obj);
	m_objects.push_back(std::move(obj));

	setColour(Window::COL_POPUP, { 0.75f, 0.75f, 0.75f, 0.85f });
	/*setColour(Window::COL_BACKGROUND, NodeCol_Background);
	setColour(Window::COL_BORDER, NodeCol_Border);
	setColour(Window::COL_TEXT, NodeCol_Text);
	setColour(Window::COL_FRAME, NodeCol_FrameBgd);
	setColour(Window::COL_FRAME_HOVER, NodeCol_FrameBgdHover);
	setColour(Window::COL_FRAME_ACTIVE, NodeCol_FrameBgdActive);*/
}

node::NodeBase::~NodeBase()
{
	//Disconnect all our fields
	for (auto&& field : m_fields) {
		assert(field.second);
		if (field.second->connector)
			field.second->connector->disconnect();
	}

	//The safest approach would be to destroy our children first, since some widgets may be referencing the objects. 
	//However, they shouldn't be touching anything on destruction.
	clearChildren();
}

void node::NodeBase::accept(gui::Visitor& v)
{
	v.visit(*this);
}

void node::NodeBase::onClose()
{
	asyncInvoke<DestroyAction>(*this);
}

nif::NiObject& node::NodeBase::object()
{
	assert(!m_objects.empty() && m_objects[0]);
	return *m_objects[0];
}

gui::Connector* node::NodeBase::addConnector(const std::string& label, ConnectorType type, std::unique_ptr<gui::Connector>&& c)
{
	gui::Connector* result = c.get();

	if (result) {
		addChild(std::move(c));

		if (type == ConnectorType::UP) {
			//upwards label to the left
			result->setController(&m_leftCtlr);
			if (!label.empty())
				addChild(std::make_unique<gui::FramePadded>(std::make_unique<gui::Text>(label)));
		}
		else {
			//downwards to the right
			result->setController(&m_rightCtlr);
			if (!label.empty()) {
				auto item = std::make_unique<gui::Item>(std::make_unique<gui::RightAlign>());
				item->newChild<gui::FramePadded>(std::make_unique<gui::Text>(label));
				addChild(std::move(item));
			}
		}
	}

	return result;
}

node::Field* node::NodeBase::getField(const std::string& name)
{
	if (auto it = m_fields.find(name); it != m_fields.end())
		return it->second.get();
	else
		return nullptr;
}

void node::NodeBase::removeController(nif::NiTimeController* obj)
{
	if (auto it = std::find_if(m_controllers.begin(), m_controllers.end(),
		[obj](const std::unique_ptr<nif::NiTimeController>& ptr) { return ptr.get() == obj; });
		it != m_controllers.end())
	{
		m_controllers.erase(it);
	}
}

gui::Floats<2> node::NodeBase::LeftController::place(const gui::Floats<2>& hint)
{
	return { m_window.getPosition()[0], hint[1] };
}

gui::Floats<2> node::NodeBase::RightController::place(const gui::Floats<2>& hint)
{
	return { m_window.getPosition()[0] + m_window.getSize()[0], hint[1] };
}
