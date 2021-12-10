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
#include "GravityModifier.h"
#include "widget_types.h"

using namespace node;
using namespace nif;

class GravityObjectField final : public Field
{
	Receiver<void> m_rcvr;
	Sender<Ptr<NiNode>> m_sndr;

public:
	GravityObjectField(const std::string& name, NodeBase& node, ni_ptr<Ptr<NiNode>>&& obj) :
		Field(name), m_sndr(*obj) //old format
	{
		connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}
};

node::GravityModifier::StrengthField::StrengthField(const std::string& name, GravityModifier& node, ni_ptr<Property<float>>&& obj) :
	Field{ name }, m_node{ node }, m_sndr{ *this }
{
	connector = node.addConnector(std::string(), ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));

	auto sw = node.newChild<gui::Switch>(std::bind(&gui::Connector::isConnected, connector));
	auto input = sw->newChild<DragFloat>(obj, name);
	input->setSensitivity(0.1f);
	input->setNumberFormat("%.2f");

	sw->newChild<gui::Label>(name);
}

Ref<NiInterpolator>& node::GravityModifier::StrengthField::iplr()
{
	//If there is no controller, create one.
	//Add it to the device.
	//Means we need access to a nif::File and to the Modifier. No way around that.
	//I do like that we (the StrngthField) manage the controller, though.
	if (!m_ctlr) {
		m_ctlr = m_node.m_file.create<NiPSysGravityStrengthCtlr>();
		m_node.addController(m_ctlr);
		m_rcvr.setController(m_ctlr);
	}
	return m_ctlr->interpolator;
}

ni_ptr<NiTimeController> node::GravityModifier::StrengthField::ctlr()
{
	if (!m_ctlr) {
		m_ctlr = m_node.m_file.create<NiPSysGravityStrengthCtlr>();
		m_node.addController(m_ctlr);
		m_rcvr.setController(m_ctlr);
	}
	return m_ctlr;
}

void node::GravityModifier::StrengthField::onAssign(NiInterpolator* obj)
{
	//when null is assigned, release the controller (not critical, we could keep it)
	if (!obj) {
		m_rcvr.setController(nullptr);
		m_node.removeController(m_ctlr.get());
		m_ctlr.reset();
	}
}

void node::GravityModifier::StrengthField::setController(const ni_ptr<NiPSysGravityStrengthCtlr>& ctlr)
{
	//Should only be called during loading
	assert(!m_ctlr);
	m_ctlr = ctlr;
	m_node.addController(m_ctlr);
	m_rcvr.setController(m_ctlr);
}

class DecayField final : public Field
{
public:
	DecayField(const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& obj) :
		Field(name)
	{
		//(planar symmetry unless world aligned)
		widget = node.newChild<DragFloat>(obj, name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
		static_cast<DragFloat*>(widget)->setAlwaysClamp(true);//negative decay freaks the game out
	}
};

class TurbulenceField final : public Field
{
public:
	TurbulenceField(const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& obj) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(obj, name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

class TurbulenceScaleField final : public Field
{
public:
	TurbulenceScaleField(const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& obj) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(obj, name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

node::GravityModifier::GravityModifier(File& file, const ni_ptr<NiPSysGravityModifier>& obj) :
	Modifier(obj), m_file{ file }
{
	newChild<gui::Separator>();

	m_objectField = newField<GravityObjectField>(GRAVITY_OBJECT, *this, 
		make_ni_ptr(obj, &NiPSysGravityModifier::gravityObject));

	m_strengthField = newField<StrengthField>(STRENGTH, *this,
		make_ni_ptr(obj, &NiPSysGravityModifier::strength));

	m_decayField = newField<DecayField>(DECAY, *this,
		make_ni_ptr(obj, &NiPSysGravityModifier::decay));

	m_turbField = newField<TurbulenceField>(TURBULENCE, *this,
		make_ni_ptr(obj, &NiPSysGravityModifier::turbulence));

	m_turbScaleField = newField<TurbulenceScaleField>(TURBULENCE_SCALE, *this,
		make_ni_ptr(obj, &NiPSysGravityModifier::turbulenceScale));
}


class GravityAxisField final : public Field
{
public:
	GravityAxisField(const std::string& name, NodeBase& node, 
		ni_ptr<Property<nif::Floats<3>>>&& axis, ni_ptr<Property<bool>>&& aligned) :
		Field(name)
	{
		node.newChild<gui::Text>(PlanarForceField::GRAVITY_AXIS);
		std::array<std::string, 3> labels{ "X", "Y", "Z" };
		auto w = node.newChild<DragInputH<nif::Floats<3>, 3>>(axis, labels);
		w->setSensitivity(0.01f);
		node.newChild<Checkbox>(aligned, PlanarForceField::WORLD_ALIGNED);
	}
};


node::PlanarForceField::PlanarForceField(File& file, const ni_ptr<NiPSysGravityModifier>& obj) :
	GravityModifier(file, obj)
{
	setTitle("Planar force field");
	setSize({ WIDTH, HEIGHT });

	newChild<gui::Separator>();
	m_axisField = newField<GravityAxisField>(GRAVITY_AXIS, *this,
		make_ni_ptr(obj, &NiPSysGravityModifier::gravityAxis),
		make_ni_ptr(obj, &NiPSysGravityModifier::worldAligned));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(GRAVITY_OBJECT)->connector->setTranslation({ 0.0f, 90.0f });
}

node::PlanarForceField::~PlanarForceField()
{
	disconnect();
}



node::SphericalForceField::SphericalForceField(File& file, const ni_ptr<NiPSysGravityModifier>& obj) :
	GravityModifier(file, obj)
{
	setTitle("Spherical force field");
	setSize({ WIDTH, HEIGHT });

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(GRAVITY_OBJECT)->connector->setTranslation({ 0.0f, 90.0f });
}

node::SphericalForceField::~SphericalForceField()
{
	disconnect();
}
