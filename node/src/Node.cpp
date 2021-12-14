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
#include "Node.h"
#include "style.h"
#include "widget_types.h"

using namespace nif;
using namespace node;

class node::NodeShared::ChildField : public Field
{
	Receiver<void> m_rvr;
	Sender<Set<NiAVObject>> m_sdr;

public:
	ChildField(const std::string& name, NodeBase& node, ni_ptr<Set<NiAVObject>>&& children) :
		Field(name), m_sdr(*children)//old format
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
	}
};

node::NodeShared::NodeShared(const ni_ptr<NiNode>& obj) : AVObject(obj)
{
	setColour(COL_TITLE, TitleCol_Node);
	setColour(COL_TITLE_ACTIVE, TitleCol_NodeActive);
}

node::Node::Node(const ni_ptr<nif::NiNode>& obj) : NodeShared(obj)
{
	setClosable(true);
	setTitle("Node");
	setSize({ WIDTH, HEIGHT });

	m_name = newField<NameField>(NAME, *this, make_ni_ptr(std::static_pointer_cast<NiObjectNET>(obj), &NiObjectNET::name));
	m_extraData = newField<ExtraDataField>(EXTRA_DATA, *this, make_ni_ptr(std::static_pointer_cast<NiObjectNET>(obj), &NiObjectNET::extraData));
	m_children = newField<ChildField>(CHILDREN, *this, make_ni_ptr(obj, &NiNode::children));
	m_references = newField<ObjectField<NiNode>>(OBJECT, *this, obj);

	m_parent = newField<ParentField>(PARENT, *this, obj);
	m_transform = newField<TransformField>(TRANSFORM, *this, make_ni_ptr(std::static_pointer_cast<NiAVObject>(obj), &NiAVObject::transform));

	//until we have some other way to determine connector position for loading placement
	m_extraData->connector->setTranslation({ WIDTH, 62.0f });
	m_children->connector->setTranslation({ WIDTH, 86.0f });
	m_references->connector->setTranslation({ WIDTH, 110.0f });
	m_parent->connector->setTranslation({ 0.0f, 134.0f });
}

node::Node::~Node()
{
	disconnect();
}


node::Root::Root(const ni_ptr<nif::NiNode>& obj) : NodeShared(obj)
{
	setTitle("Root");
	setSize({ WIDTH, HEIGHT });

	//newField<NameField>(NAME, *this);
	m_extraData = newField<ExtraDataField>(EXTRA_DATA, *this, make_ni_ptr(std::static_pointer_cast<NiObjectNET>(obj), &NiObjectNET::extraData));
	m_children = newField<ChildField>(CHILDREN, *this, make_ni_ptr(obj, &NiNode::children));
	m_references = newField<ObjectField<NiNode>>(OBJECT, *this, obj);
	m_behaviour = newField<Behaviour>(Behaviour::ID, *this, obj);

	//until we have some other way to determine connector position for loading placement
	m_extraData->connector->setTranslation({ WIDTH, 38.0f });
	m_children->connector->setTranslation({ WIDTH, 62.0f });
	m_references->connector->setTranslation({ WIDTH, 86.0f });
	m_behaviour->connector->setTranslation({ WIDTH, 110.0f });
}

node::Root::~Root()
{
	disconnect();
}

node::Root::Behaviour::Behaviour(const std::string& name, NodeBase& node, const ni_ptr<NiAVObject>& obj) :
	Field{ name }, m_ifc{ obj }, m_rcvr{ obj }, m_sndr{ m_ifc }
{
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
}

node::Root::Behaviour::ControllableRoot::ControllableRoot(const ni_ptr<NiAVObject>& obj) :
	m_obj{ obj }
{
	assert(obj);
}

void node::Root::Behaviour::ControllableRoot::addController(const ni_ptr<NiTimeController>& ctlr)
{
	if (ctlr && ctlr->target.assigned() != m_obj) {
		assert(!ctlr->target.assigned());//or it could end up with double owners
		m_obj->controllers.insert(0, ctlr);
		ctlr->target.assign(m_obj);
	}
}

void node::Root::Behaviour::ControllableRoot::removeController(NiTimeController* ctlr)
{
	if (ctlr) {
		if (int pos = m_obj->controllers.find(ctlr); pos != -1) {
			m_obj->controllers.erase(pos);
			ctlr->target.assign(nullptr);
		}
	}
}

Set<NiExtraData>& node::Root::Behaviour::ControllableRoot::extraData()
{
	return m_obj->extraData;
}


node::BillboardNode::BillboardNode(const ni_ptr<NiBillboardNode>& obj) :
	Node(obj)
{
	setTitle("Billboard node");
	setSize({ WIDTH, HEIGHT });

	newChild<gui::Separator>();

	newChild<gui::Text>("Billboard mode");

	using selector_type = gui::Selector<BillboardMode, ni_ptr<Property<BillboardMode>>>;
	auto selector = newChild<selector_type>(make_ni_ptr(obj, &NiBillboardNode::mode), std::string(),
		selector_type::ItemList{
			{ ALWAYS_FACE_CAMERA, "Direction" },
			{ RIGID_FACE_CAMERA, "Direction (rigid)" },
			{ ALWAYS_FACE_CENTER, "Position" },
			{ RIGID_FACE_CENTER, "Position (rigid)" },
			{ ROTATE_ABOUT_UP, "Position (Y only)" },
			{ BS_ROTATE_ABOUT_UP, "Global Z" } });
}

node::BillboardNode::~BillboardNode()
{
	disconnect();
}
