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
#include "Node.h"
#include "style.h"
#include "widget_types.h"

class node::NodeShared::ChildField : public Field
{
public:
	ChildField(const std::string& name, NodeShared& node) :
		Field(name), m_sdr(node.object().children())
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
	}

private:
	Receiver<void> m_rvr;
	Sender<ISet<nif::NiAVObject>> m_sdr;
};

node::NodeShared::NodeShared(std::unique_ptr<nif::NiNode>&& obj) : AVObject(std::move(obj))
{
	setColour(COL_TITLE, TitleCol_Node);
	setColour(COL_TITLE_ACTIVE, TitleCol_NodeActive);
	setSize({ 150.0f, 0.0f });
}

nif::NiNode& node::NodeShared::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiNode*>(getObjects()[0].get());
}

node::Node::Node() : Node(std::make_unique<nif::NiNode>()) {}

node::Node::Node(std::unique_ptr<nif::NiNode>&& obj) : NodeShared(std::move(obj))
{
	setClosable(true);
	setTitle("Node");

	newField<NameField>(NAME, *this);
	newField<ExtraDataField>(EXTRA_DATA, *this);
	newField<ChildField>(CHILDREN, *this);
	newField<ObjectField<nif::NiNode>>(OBJECT, *this, object());

	newField<ParentField>(PARENT, *this);
	newField<TransformField>(TRANSFORM, *this);

}

node::Root::Root() : Root(std::make_unique<nif::BSFadeNode>()) {}

node::Root::Root(std::unique_ptr<nif::NiNode>&& obj) : NodeShared(std::move(obj))
{
	setTitle("Root");

	struct NameField : Field
	{
		NameField(const std::string& name, Root& node) : Field(name)
		{
			widget = node.newChild<gui::FramePadded>(
				std::make_unique<gui::TextProperty<IProperty<std::string>>>(node.object().name()));
		}
	};

	newField<NameField>(NAME, *this);
	newField<ExtraDataField>(EXTRA_DATA, *this);
	newField<ChildField>(CHILDREN, *this);
	newField<ObjectField<nif::NiNode>>(OBJECT, *this, object());
}
