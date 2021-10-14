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
#include "RotationModifier.h"
#include "widget_types.h"

constexpr float VAR_FRACTION = -0.5f;
constexpr float WND_SIZE = 160.0f;

class node::RotationModifier::AngleField final : public node::Field
{
public:
	AngleField(const std::string& name, node::RotationModifier& node) : Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().angle(), name);
		auto var = item->newChild<DragFloat>(node.object().angleVar(), u8"±");
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

class node::RotationModifier::SpeedField final : public node::Field
{
public:
	SpeedField(const std::string& name, node::RotationModifier& node) : Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().speed(), u8"Speed");
		auto var = item->newChild<DragFloat>(node.object().speedVar(), u8"±");
		main->setSensitivity(0.5f);
		main->setNumberFormat(u8"%.0f°");
		var->setSensitivity(0.5f);
		var->setLowerLimit(0.0f);
		var->setNumberFormat(u8"%.0f°");
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

node::RotationModifier::RotationModifier() :
	RotationModifier(std::make_unique<nif::NiPSysRotationModifier>())
{
	object().active().set(true);
}

node::RotationModifier::RotationModifier(std::unique_ptr<nif::NiPSysRotationModifier>&& obj) :
	Modifier(std::move(obj))
{
	setSize({ WND_SIZE, 0.0f });
	setTitle("Rotation modifier");

	addTargetField(std::make_shared<ReqDevice<Requirement::ROTATION>>(*this));

	newChild<gui::Separator>();

	newField<AngleField>(ANGLE, *this);
	newField<SpeedField>(SPEED, *this);
	newChild<Checkbox>(object().randomSign(), "Random direction");
}

nif::NiPSysRotationModifier& node::RotationModifier::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysRotationModifier*>(getObjects()[0].get());
}
