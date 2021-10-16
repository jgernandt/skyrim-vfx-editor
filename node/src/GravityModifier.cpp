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

class node::GravityModifier::GravityObjectField final : public Field
{
public:
	GravityObjectField(const std::string& name, GravityModifier& node) :
		Field(name), m_sndr(node.object().gravityObject())
	{
		connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}

private:
	Receiver<void> m_rcvr;
	Sender<IAssignable<nif::NiNode>> m_sndr;
};

/*class node::GravityModifier::ForceTypeField final : public Field
{
public:
	ForceTypeField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		using widget_type = gui::Selector<nif::ForceType, IProperty<nif::ForceType>>;
		widget = node.newChild<widget_type>(node.object().forceType(), name,
			widget_type::ItemList{
				{ nif::FORCE_PLANAR, "Planar" },
				{ nif::FORCE_SPHERICAL, "Spherical" } });
	}

};*/

class node::GravityModifier::StrengthField final : public Field
{
public:
	StrengthField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(node.object().strength(), name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.1f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

class node::GravityModifier::DecayField final : public Field
{
public:
	DecayField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		//(planar symmetry unless world aligned)
		widget = node.newChild<DragFloat>(node.object().decay(), name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
		static_cast<DragFloat*>(widget)->setAlwaysClamp(true);//negative decay freaks the game out
	}
};

class node::GravityModifier::TurbulenceField final : public Field
{
public:
	TurbulenceField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(node.object().turbulence(), name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

class node::GravityModifier::TurbulenceScaleField final : public Field
{
public:
	TurbulenceScaleField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		widget = node.newChild<DragFloat>(node.object().turbulenceScale(), name);
		static_cast<DragFloat*>(widget)->setSensitivity(0.005f);
		static_cast<DragFloat*>(widget)->setLowerLimit(0.0f);
		static_cast<DragFloat*>(widget)->setNumberFormat("%.2f");
	}
};

node::GravityModifier::GravityModifier(std::unique_ptr<nif::NiPSysGravityModifier>&& obj) :
	Modifier(std::move(obj))
{
	setSize({ 150.0f, 0.0f });

	addTargetField(std::make_shared<ReqDevice<Requirement::MOVEMENT>>(*this));

	newChild<gui::Separator>();

	newField<GravityObjectField>(GRAVITY_OBJECT, *this);
	newField<StrengthField>(STRENGTH, *this);
	newField<DecayField>(DECAY, *this);
	newField<TurbulenceField>(TURBULENCE, *this);
	newField<TurbulenceScaleField>(TURBULENCE_SCALE, *this);
}

nif::NiPSysGravityModifier& node::GravityModifier::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysGravityModifier*>(getObjects()[0].get());
}

class node::PlanarForceField::GravityAxisField final : public Field
{
public:
	GravityAxisField(const std::string& name, GravityModifier& node) :
		Field(name)
	{
		node.newChild<gui::Text>(GRAVITY_AXIS);
		std::array<std::string, 3> labels{ "X", "Y", "Z" };
		auto w = node.newChild<DragInputH<nif::Floats<3>, 3>>(node.object().gravityAxis(), labels);
		w->setSensitivity(0.01f);
		node.newChild<Checkbox>(node.object().worldAligned(), WORLD_ALIGNED);
	}
};

node::PlanarForceField::PlanarForceField() :
	PlanarForceField(std::make_unique<nif::NiPSysGravityModifier>())
{
	object().active().set(true);
	object().forceType().set(nif::FORCE_PLANAR);
	object().gravityAxis().set({ 0.0f, 0.0f, 1.0f });
}

node::PlanarForceField::PlanarForceField(std::unique_ptr<nif::NiPSysGravityModifier>&& obj) :
	GravityModifier(std::move(obj))
{
	setTitle("Planar force field");

	newChild<gui::Separator>();
	newField<GravityAxisField>(GRAVITY_AXIS, *this);
}

node::SphericalForceField::SphericalForceField() :
	SphericalForceField(std::make_unique<nif::NiPSysGravityModifier>())
{
	object().active().set(true);
	object().forceType().set(nif::FORCE_SPHERICAL);
	object().gravityAxis().set({ 0.0f, 0.0f, 1.0f });
}

node::SphericalForceField::SphericalForceField(std::unique_ptr<nif::NiPSysGravityModifier>&& obj) :
	GravityModifier(std::move(obj))
{
	setTitle("Spherical force field");
}
