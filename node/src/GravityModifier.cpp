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

class StrengthField final : public Field
{
public:
	StrengthField(const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& obj) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(obj, name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.1f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

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

node::GravityModifier::GravityModifier(const ni_ptr<NiPSysGravityModifier>& obj) :
	Modifier(obj)
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


node::PlanarForceField::PlanarForceField(const ni_ptr<NiPSysGravityModifier>& obj) :
	GravityModifier(obj)
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



node::SphericalForceField::SphericalForceField(const ni_ptr<NiPSysGravityModifier>& obj) :
	GravityModifier(obj)
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
