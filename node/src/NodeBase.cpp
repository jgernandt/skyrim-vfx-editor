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

node::NodeBase::NodeBase() :
	Window(std::string()), m_leftCtlr(*this), m_rightCtlr(*this)
{
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
}

void node::NodeBase::onClose()
{
	asyncInvoke<DestroyAction>(*this);
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
				addChild(std::make_unique<gui::Label>(label));
		}
		else {
			//downwards to the right
			result->setController(&m_rightCtlr);
			if (!label.empty()) {
				auto item = std::make_unique<gui::Item>(std::make_unique<gui::RightAlign>());
				item->newChild<gui::Label>(label);
				addChild(std::move(item));
			}
		}
	}

	return result;
}

node::Field* node::NodeBase::getField(const std::string& name)
{
	if (auto it = m_fields.find(name); it != m_fields.end())
		return it->second;
	else
		return nullptr;
}

void node::NodeBase::disconnect()
{
	//Disconnect all our fields
	for (auto&& field : m_fields) {
		assert(field.second);
		if (field.second->connector)
			field.second->connector->disconnect();
	}
}

gui::Floats<2> node::NodeBase::LeftController::place(const gui::Floats<2>& hint)
{
	return { 0.0f, hint[1] };
}

gui::Floats<2> node::NodeBase::RightController::place(const gui::Floats<2>& hint)
{
	return { m_window.getSize()[0], hint[1] };
}
