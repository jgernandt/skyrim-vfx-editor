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
#include "ExtraData.h"
#include "style.h"
#include "widget_types.h"

node::ExtraData::TargetField::TargetField(const std::string& name, ExtraData& node) :
	Field(name), m_rvr(node.object())
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
}

node::ExtraData::NameField::NameField(const std::string& name, ExtraData& node) : Field(name)
{
	node.newChild<gui::Text>(name);
	node.newChild<StringInput>(node.object().name());
}

node::ExtraData::ExtraData(std::unique_ptr<nif::NiExtraData>&& obj) : NodeBase(std::move(obj))
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_XData);
	setColour(COL_TITLE_ACTIVE, TitleCol_XDataActive);

	newField<TargetField>(TARGET, *this);
}

nif::NiExtraData& node::ExtraData::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiExtraData*>(getObjects()[0].get());
}


node::StringDataShared::StringDataShared(std::unique_ptr<nif::NiStringExtraData>&& obj) :
	ExtraData(std::move(obj))
{
}

nif::NiStringExtraData& node::StringDataShared::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiStringExtraData*>(getObjects()[0].get());
}


node::StringData::StringData() : StringData(std::make_unique<nif::NiStringExtraData>()) {}

node::StringData::StringData(std::unique_ptr<nif::NiStringExtraData>&& obj) : 
	StringDataShared(std::move(obj))
{
	setTitle("String data");
	setSize({ WIDTH, HEIGHT });

	newField<NameField>(NAME, *this);

	struct ValueField : Field
	{
		ValueField(const std::string& name, StringData& node) : Field(name)
		{
			node.newChild<gui::Text>(name);
			node.newChild<StringInput>(node.object().value());
		}
	};
	newField<ValueField>(VALUE, *this);

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::WeaponTypeData::WeaponTypeData() : WeaponTypeData(std::make_unique<nif::NiStringExtraData>()) {}

node::WeaponTypeData::WeaponTypeData(std::unique_ptr<nif::NiStringExtraData>&& obj) : 
	StringDataShared(std::move(obj))
{
	setTitle("Weapon type data");
	setSize({ WIDTH, HEIGHT });

	object().name().set("Prn");

	struct TypeField : Field
	{
		TypeField(const std::string& name, WeaponTypeData& node) : Field(name)
		{
			using widget_type = gui::Selector<std::string, IProperty<std::string>>;
			widget = node.newChild<widget_type>(node.object().value(), name,
				widget_type::ItemList{
					{ "WeaponAxe", "Axe" },
					{ "WeaponDagger", "Dagger" },
					{ "WeaponMace", "Mace" },
					{ "WeaponSword", "Sword" },
					{ "WeaponBack", "Two-hander" },
					{ "WeaponBow", "Ranged" },
					{ "SHIELD", "Shield" } });
		}
	};

	newField<TypeField>(TYPE, *this);

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::DummyExtraData::DummyExtraData(std::unique_ptr<nif::NiExtraData>&& obj) :
	ExtraData(std::move(obj))
{
	setTitle("Extra data");
	setSize({ WIDTH, HEIGHT });
	newChild<gui::Separator>();
	newChild<gui::FramePadded>(std::make_unique<gui::Text>(object().name().get()));

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}
