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
#include "RotationModifier.h"
#include "widget_types.h"

constexpr float VAR_FRACTION = -0.5f;

using namespace nif;
using namespace node;

class AngleField final : public node::Field
{
public:
	AngleField(const std::string& name, NodeBase& node, 
		ni_ptr<Property<math::degf>>&& angle, ni_ptr<Property<math::degf>>&& angleVar) : 
		Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(angle, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(angleVar, u8"±");
		main->setSensitivity(0.5f);
		main->setLowerLimit(-180.0f);
		main->setUpperLimit(180.0f);
		main->setNumberFormat(u8"%.0f°");
		var->setSensitivity(0.5f);
		var->setLowerLimit(0.0f);
		var->setUpperLimit(180.0f);
		var->setNumberFormat(u8"%.0f°");
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

class SpeedField final : public node::Field
{
public:
	SpeedField(const std::string& name, NodeBase& node,
		ni_ptr<Property<math::degf>>&& speed, ni_ptr<Property<math::degf>>&& speedVar) :
		Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(speed, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(speedVar, u8"±");
		main->setSensitivity(0.5f);
		main->setNumberFormat(u8"%.0f°");
		var->setSensitivity(0.5f);
		var->setLowerLimit(0.0f);
		var->setNumberFormat(u8"%.0f°");
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

node::RotationModifier::RotationModifier(ni_ptr<NiPSysRotationModifier>&& obj) :
	Modifier(obj)
{
	setSize({ WIDTH, HEIGHT });
	setTitle("Rotation modifier");

	m_device.addRequirement(ModRequirement::ROTATION);

	newChild<gui::Separator>();

	m_angleField = newField<AngleField>(ANGLE, *this,
		make_ni_ptr(obj, &NiPSysRotationModifier::angle),
		make_ni_ptr(obj, &NiPSysRotationModifier::angleVar));

	m_speedField = newField<SpeedField>(SPEED, *this,
		make_ni_ptr(obj, &NiPSysRotationModifier::speed),
		make_ni_ptr(obj, &NiPSysRotationModifier::speedVar));

	newChild<Checkbox>(make_ni_ptr(obj, &NiPSysRotationModifier::randomSign), "Random direction");

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
}

node::RotationModifier::~RotationModifier()
{
	disconnect();
}
