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
#include "AVObject.h"
#include "widget_types.h"
#include "style.h"

using namespace nif;

node::ObjectNET::NameField::NameField(const std::string& name, NodeBase& node, ni_ptr<Property<std::string>>&& name2) : 
	Field(name)
{
	widget = node.newChild<StringInput>(name2);
}

node::ObjectNET::ExtraDataField::ExtraDataField(const std::string& name, NodeBase& node, ni_ptr<Set<NiExtraData>>&& extraData) :
	Field(name), 
	m_sdr(*extraData)//old format
{
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
}

node::ObjectNET::ObjectNET()
{
}


node::AVObject::ParentField::ParentField(const std::string& name, NodeBase& node, const ni_ptr<NiAVObject>& object) :
	Field(name), m_rvr(object)
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sdr, m_rvr));
}


node::AVObject::TransformField::TransformField(const std::string& name, AVObject& node, ni_ptr<Transform>&& transform) :
	Field(name)
{
	//Future project: add connector to a controller

	auto popup = std::make_unique<gui::Popup>();
	popup->setSize({ 150.0f, 0.0f });

	//Translation
	popup->addChild(std::make_unique<gui::Text>("Translation"));
	std::array<std::string, 3> labels{ "X", "Y", "Z" };
	auto t = std::make_unique<DragInput<nif::translation_t, 3>>(make_ni_ptr(transform, &Transform::translation), labels);
	t->setSensitivity(0.1f);
	popup->addChild(std::move(t));
	
	//Rotation
	popup->addChild(std::make_unique<gui::Text>("Rotation"));
	node.m_rotAdapter.updateUI(popup->newChild<gui::Composite>());
	
	//Scale
	popup->addChild(std::make_unique<gui::Text>(" "));
	auto s = std::make_unique<DragFloat>(make_ni_ptr(transform, &Transform::scale), "Scale");
	s->setSensitivity(0.01f);
	s->setLowerLimit(0.0f);
	s->setUpperLimit(std::numeric_limits<float>::max());
	s->setAlwaysClamp();
	s->setNumberFormat("%.2f");
	popup->addChild(std::move(s));

	widget = node.newChild<gui::Button>(name, std::bind(&gui::Popup::open, popup.get()));
	node.addChild(std::move(popup));

	//What do we set widget to?
}


node::AVObject::AVObject(const ni_ptr<NiAVObject>& obj) : m_rotAdapter(obj)
{
}


//RotationAdapter//////////////////

template<>
struct util::property_traits<node::AVObject::RotationAdapter*>
{
	using property_type = node::AVObject::RotationAdapter*;
	using value_type = math::Rotation::euler_type;
	using get_type = math::Rotation::euler_type;

	static get_type get(node::AVObject::RotationAdapter* p) { return p->m_current; }
	static void set(node::AVObject::RotationAdapter* p, const value_type& val) { p->setEuler(val); }
};
template<>
struct util::property_traits<node::AVObject::DummyClass1*>
{
	using property_type = node::AVObject::DummyClass1*;
	using value_type = math::EulerOrder;
	using get_type = math::EulerOrder;

	static get_type get(node::AVObject::DummyClass1* p) 
	{ 
		return static_cast<node::AVObject::RotationAdapter*>(p)->m_current.order; 
	}
	static void set(node::AVObject::DummyClass1* p, math::EulerOrder val)
	{ 
		static_cast<node::AVObject::RotationAdapter*>(p)->setEulerOrder(val);
	}
};
template<>
struct util::property_traits<node::AVObject::DummyClass2*>
{
	using property_type = node::AVObject::DummyClass2*;
	using value_type = bool;
	using get_type = bool;

	static get_type get(node::AVObject::DummyClass2* p)
	{
		return static_cast<node::AVObject::RotationAdapter*>(p)->m_intrinsic;
	}
	static void set(node::AVObject::DummyClass2* p, bool val)
	{
		static_cast<node::AVObject::RotationAdapter*>(p)->setIntrinsic(val);
	}
};

node::AVObject::RotationAdapter::RotationAdapter(const ni_ptr<NiAVObject>& obj)
{
	assert(obj);
	m_backend = make_ni_ptr(obj, &obj->transform.rotation);
	m_backend->addListener(*this);
	onSet(m_backend->get());
}

node::AVObject::RotationAdapter::~RotationAdapter()
{
	m_backend->removeListener(*this);
}

void node::AVObject::RotationAdapter::onSet(const math::Rotation& rot)
{
	m_current = rot.getEuler(m_current.order);
}

void node::AVObject::RotationAdapter::setEuler(const math::Rotation::euler_type& euler)
{
	m_backend->set(math::Rotation(euler));
}

void node::AVObject::RotationAdapter::setEulerOrder(math::EulerOrder order)
{
	if (order != m_current.order) {
		m_current = m_backend->get().getEuler(order);
		updateUI(m_guiRoot);
	}
}

void node::AVObject::RotationAdapter::setIntrinsic(bool intrinsic)
{
	if (intrinsic != m_intrinsic) {
		m_intrinsic = intrinsic;
		updateUI(m_guiRoot);
	}
}

constexpr std::array<size_t, 3> extrOrder(math::EulerOrder order)
{
	switch (order) {
	case math::EulerOrder::XYZ:
		return { 0, 1, 2 };
	case math::EulerOrder::YZX:
		return { 1, 2, 0 };
	case math::EulerOrder::ZXY:
		return { 2, 0, 1 };
	case math::EulerOrder::XZY:
		return { 0, 2, 1 };
	case math::EulerOrder::YXZ:
		return { 1, 0, 2 };
	case math::EulerOrder::ZYX:
		return { 2, 1, 0 };
	default:
		return { 0, 0, 0 };
	}
}

constexpr std::array<size_t, 3> intrOrder(math::EulerOrder order)
{
	switch (order) {
	case math::EulerOrder::XYZ:
		return { 2, 1, 0 };
	case math::EulerOrder::YZX:
		return { 0, 2, 1 };
	case math::EulerOrder::ZXY:
		return { 1, 0, 2 };
	case math::EulerOrder::XZY:
		return { 1, 2, 0 };
	case math::EulerOrder::YXZ:
		return { 2, 0, 1 };
	case math::EulerOrder::ZYX:
		return { 0, 1, 2 };
	default:
		return { 0, 0, 0 };
	}
}
void node::AVObject::RotationAdapter::updateUI(gui::Composite* root)
{
	if (m_guiRoot)
		m_guiRoot->clearChildren();

	if (root)
		root->clearChildren();

	m_guiRoot = root;

	if (m_guiRoot) {
		using widget_type1 = gui::Selector<bool, DummyClass2*>;
		m_guiRoot->newChild<widget_type1>(this, std::string(),
			widget_type1::ItemList{
				{ false, "Euler extrinsic" },
				{ true, "Euler intrinsic" } });

		if (m_intrinsic) {
			using widget_type2 = gui::Selector<math::EulerOrder, DummyClass1*>;
			m_guiRoot->newChild<widget_type2>(this, std::string(),
				widget_type2::ItemList{
					{ math::EulerOrder::XYZ, "Z up, X forward" },
					{ math::EulerOrder::XZY, "Y up, X forward" },
					{ math::EulerOrder::YXZ, "Z up, Y forward" },
					{ math::EulerOrder::YZX, "X up, Y forward" },
					{ math::EulerOrder::ZXY, "Y up, Z forward" },
					{ math::EulerOrder::ZYX, "X up, Z forward" } });

			auto axis = intrOrder(m_current.order);

			std::array<std::string, 3> labels;
			labels[axis[0]] = "Yaw";
			labels[axis[1]] = "Pitch";
			labels[axis[2]] = "Roll";
			auto widget = std::make_unique<gui::DragInput<math::Rotation::euler_type, 3, RotationAdapter*>>(this, labels);

			widget->setSensitivity(0.5f);
			widget->setNumberFormat(u8"%.0f°");

			widget->setLowerLimit(-90.0f, axis[1]);
			widget->setUpperLimit(90.0f, axis[1]);
			widget->setAlwaysClamp(true, axis[1]);

			widget->setPermutation(axis);

			m_guiRoot->addChild(std::move(widget));
		}
		else {
			using widget_type2 = gui::Selector<math::EulerOrder, DummyClass1*>;
			m_guiRoot->newChild<widget_type2>(this, std::string(),//needs a property
				widget_type2::ItemList{
					{ math::EulerOrder::XYZ, "Order XYZ" },
					{ math::EulerOrder::XZY, "Order XZY" },
					{ math::EulerOrder::YXZ, "Order YXZ" },
					{ math::EulerOrder::YZX, "Order YZX" },
					{ math::EulerOrder::ZXY, "Order ZXY" },
					{ math::EulerOrder::ZYX, "Order ZYX" } });

			std::array<std::string, 3> labels{ "X", "Y", "Z" };
			auto widget = std::make_unique<gui::DragInput<math::Rotation::euler_type, 3, RotationAdapter*>>(this, labels);

			widget->setSensitivity(0.5f);
			widget->setNumberFormat(u8"%.0f°");

			auto axis = extrOrder(m_current.order);
			widget->setLowerLimit(-90.0f, axis[1]);
			widget->setUpperLimit(90.0f, axis[1]);
			widget->setAlwaysClamp(true, axis[1]);

			m_guiRoot->addChild(std::move(widget));
		}
	}
}


node::DummyAVObject::DummyAVObject(ni_ptr<NiAVObject>&& obj) :
	AVObject(obj)
{
	setColour(COL_TITLE, TitleCol_Geom);
	setColour(COL_TITLE_ACTIVE, TitleCol_GeomActive);
	setSize({ WIDTH, HEIGHT });

	setClosable(true);
	setTitle("AVObject");

	m_name = newField<NameField>(NAME, *this, make_ni_ptr<Property<std::string>, NiObjectNET>(obj, &NiObjectNET::name));
	m_parent = newField<ParentField>(PARENT, *this, obj);
	m_transform = newField<TransformField>(TRANSFORM, *this, make_ni_ptr(obj, &NiAVObject::transform));
}

node::DummyAVObject::~DummyAVObject()
{
	disconnect();
}
