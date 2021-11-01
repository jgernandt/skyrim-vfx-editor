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

node::ObjectNET::NameField::NameField(const std::string& name, ObjectNET& node) : Field(name)
{
	widget = node.newChild<StringInput>(node.object().name());
}

node::ObjectNET::ExtraDataField::ExtraDataField(const std::string& name, ObjectNET& node) : 
	Field(name), m_sdr(node.object().extraData())
{
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
}

node::ObjectNET::ObjectNET(std::unique_ptr<nif::NiObjectNET>&& obj) : NodeBase(std::move(obj))
{
}

nif::NiObjectNET& node::ObjectNET::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiObjectNET*>(getObjects()[0].get());
}


node::AVObject::ParentField::ParentField(const std::string& name, AVObject& node) :
	Field(name), m_rvr(node.object())
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sdr, m_rvr));
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

//This class uses itself as a property for its DragInputs, by forwarding to get/setData.
//get/setData performs a conversion of the format provided by the interface to whatever
//format is currently set to display.
//Data could return size 4, if we want to support a quat format.
class RotationInput final :
	public gui::Composite
{
public:
	using PropertyType = IProperty<nif::rotation_t>;

public:
	RotationInput(PropertyType& p, math::EulerOrder order, bool intrinsic) : m_property{ p }, m_order{ *this, order }, m_intrinsic{ *this, intrinsic }
	{ 
		rebuildUI(); 
	}
	~RotationInput() { clearChildren(); }//safe play, since our children reference properties on us

	math::Rotation::euler_type getData() const
	{
		//Treat this as a template, so that it can handle a change of types later.
		//Right now, all of this just forwards a math::Rotation from m_property.get().
		return nif::nif_type_conversion<math::Rotation>::from(util::property_traits<PropertyType>::get(m_property)).getEuler(m_order.value);
	}
	void setData(const math::Rotation::euler_type& euler)
	{
		//Likewise, this just instantiates a math::Rotation from the param and forwards it to m_property.set
		util::property_traits<PropertyType>::set(m_property,
			nif::nif_type_conversion<util::property_traits<PropertyType>::value_type>::from(math::Rotation(euler)));
	}

	void rebuildUI();

	template<typename T>
	struct Property
	{
		Property(RotationInput& p, T val) : parent{ p }, value{ val } {}
		T get() const { return value; }
		void set(T val) 
		{ 
			if (val != value) {
				value = val;
				parent.rebuildUI();
			}
		}

		RotationInput& parent;
		T value;
	};


private:

	PropertyType& m_property;
	Property<math::EulerOrder> m_order;
	Property<bool> m_intrinsic;
};

template<>
struct util::property_traits<RotationInput>
{
	using property_type = RotationInput;
	using value_type = math::Rotation::euler_type;
	using get_type = math::Rotation::euler_type;

	static get_type get(const property_type& p) { return p.getData(); }
	static void set(property_type& p, const value_type& val) { p.setData(val); }
};

template<typename T>
struct util::property_traits<RotationInput::Property<T>>
{
	using property_type = RotationInput::Property<T>;
	using value_type = T;
	using get_type = T;

	static get_type get(const property_type& p) { return p.get(); }
	static void set(property_type& p, const value_type& val) { p.set(val); }
};

void RotationInput::rebuildUI()
{
	clearChildren();
	using widget_type1 = gui::Selector<bool, Property<bool>>;
	newChild<widget_type1>(m_intrinsic, std::string(),
		widget_type1::ItemList{
			{ false, "Euler extrinsic" },
			{ true, "Euler intrinsic" } });

	if (m_intrinsic.value) {
		using widget_type2 = gui::Selector<math::EulerOrder, Property<math::EulerOrder>>;
		newChild<widget_type2>(m_order, std::string(),
			widget_type2::ItemList{
				{ math::EulerOrder::XYZ, "Z up, X forward" },
				{ math::EulerOrder::XZY, "Y up, X forward" },
				{ math::EulerOrder::YXZ, "Z up, Y forward" },
				{ math::EulerOrder::YZX, "X up, Y forward" },
				{ math::EulerOrder::ZXY, "Y up, Z forward" },
				{ math::EulerOrder::ZYX, "X up, Z forward" } });

		auto axis = intrOrder(m_order.value);

		std::array<std::string, 3> labels;
		labels[axis[0]] = "Yaw";
		labels[axis[1]] = "Pitch";
		labels[axis[2]] = "Roll";
		auto widget = std::make_unique<gui::DragInput<math::Rotation::euler_type, 3, RotationInput>>(*this, labels);

		widget->setSensitivity(0.5f);
		widget->setNumberFormat(u8"%.0f°");

		widget->setLowerLimit(-90.0f, axis[1]);
		widget->setUpperLimit(90.0f, axis[1]);
		widget->setAlwaysClamp(true, axis[1]);

		widget->setPermutation(axis);

		addChild(std::move(widget));
	}
	else {
		using widget_type2 = gui::Selector<math::EulerOrder, Property<math::EulerOrder>>;
		newChild<widget_type2>(m_order, std::string(),
			widget_type2::ItemList{
				{ math::EulerOrder::XYZ, "Order XYZ" },
				{ math::EulerOrder::XZY, "Order XZY" },
				{ math::EulerOrder::YXZ, "Order YXZ" },
				{ math::EulerOrder::YZX, "Order YZX" },
				{ math::EulerOrder::ZXY, "Order ZXY" },
				{ math::EulerOrder::ZYX, "Order ZYX" } });

		std::array<std::string, 3> labels{ "X", "Y", "Z" };
		auto widget = std::make_unique<gui::DragInput<math::Rotation::euler_type, 3, RotationInput>>(*this, labels);

		widget->setSensitivity(0.5f);
		widget->setNumberFormat(u8"%.0f°");

		auto axis = extrOrder(m_order.value);
		widget->setLowerLimit(-90.0f, axis[1]);
		widget->setUpperLimit(90.0f, axis[1]);
		widget->setAlwaysClamp(true, axis[1]);

		addChild(std::move(widget));
	}
}

node::AVObject::TransformField::TransformField(const std::string& name, AVObject& node) : Field(name)
{
	//Future project: add connector to a controller

	auto popup = std::make_unique<gui::Popup>();
	popup->setSize({ 150.0f, 0.0f });

	//Translation
	popup->addChild(std::make_unique<gui::Text>("Translation"));
	std::array<std::string, 3> labels{ "X", "Y", "Z" };
	auto t = std::make_unique<DragInput<nif::translation_t, 3>>(node.object().transform().translation(), labels);
	t->setSensitivity(0.1f);
	popup->addChild(std::move(t));

	//Rotation
	popup->addChild(std::make_unique<gui::Text>("Rotation"));
	popup->addChild(std::make_unique<RotationInput>(node.object().transform().rotation(), math::EulerOrder::XYZ, false));

	//Scale
	popup->addChild(std::make_unique<gui::Text>(" "));
	auto s = std::make_unique<DragFloat>(node.object().transform().scale(), "Scale");
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


node::AVObject::AVObject(std::unique_ptr<nif::NiAVObject>&& obj) :
	ObjectNET(std::move(obj))
{
}

nif::NiAVObject& node::AVObject::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiAVObject*>(getObjects()[0].get());
}

node::DummyAVObject::DummyAVObject(std::unique_ptr<nif::NiAVObject>&& obj) : 
	AVObject(std::move(obj))
{
	setColour(COL_TITLE, TitleCol_Geom);
	setColour(COL_TITLE_ACTIVE, TitleCol_GeomActive);
	setSize({ WIDTH, HEIGHT });

	setClosable(true);
	setTitle("AVObject");

	newField<NameField>(NAME, *this);
	newField<ParentField>(PARENT, *this);
	newField<TransformField>(TRANSFORM, *this);
}
