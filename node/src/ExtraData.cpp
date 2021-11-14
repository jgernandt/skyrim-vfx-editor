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

node::ExtraData::ExtraData(ni_ptr<nif::NiExtraData>&& obj) : m_obj{ std::move(obj) }
{
	assert(m_obj);

	setClosable(true);
	setColour(COL_TITLE, TitleCol_XData);
	setColour(COL_TITLE_ACTIVE, TitleCol_XDataActive);

	m_targetField = newField<TargetField>(TARGET, *this);
}

nif::NiExtraData& node::ExtraData::object()
{
	return *m_obj;
}


node::StringDataShared::StringDataShared(ni_ptr<nif::NiStringExtraData>&& obj) :
	ExtraData(std::move(obj))
{
}

nif::NiStringExtraData& node::StringDataShared::object()
{
	assert(m_obj);
	return *static_cast<nif::NiStringExtraData*>(m_obj.get());
}


node::StringData::StringData(nif::File& file) : StringData(file.create<nif::NiStringExtraData>()) {}

node::StringData::StringData(ni_ptr<nif::NiStringExtraData>&& obj) :
	StringDataShared(std::move(obj))
{
	setTitle("String data");
	setSize({ WIDTH, HEIGHT });

	newChild<gui::Text>(NAME);
	newChild<StringInput>(object().name());

	newChild<gui::Text>(VALUE);
	newChild<StringInput>(object().value());

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::StringData::~StringData()
{
	disconnect();
}

node::WeaponTypeData::WeaponTypeData(nif::File& file) : WeaponTypeData(file.create<nif::NiStringExtraData>()) {}

node::WeaponTypeData::WeaponTypeData(ni_ptr<nif::NiStringExtraData>&& obj) :
	StringDataShared(std::move(obj))
{
	setTitle("Weapon type data");
	setSize({ WIDTH, HEIGHT });

	object().name().set("Prn");

	using widget_type = gui::Selector<std::string, IProperty<std::string>>;
	newChild<widget_type>(object().value(), TYPE,
		widget_type::ItemList{
			{ "WeaponAxe", "Axe" },
			{ "WeaponDagger", "Dagger" },
			{ "WeaponMace", "Mace" },
			{ "WeaponSword", "Sword" },
			{ "WeaponBack", "Two-hander" },
			{ "WeaponBow", "Ranged" },
			{ "SHIELD", "Shield" } });

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::WeaponTypeData::~WeaponTypeData()
{
	disconnect();
}

node::DummyExtraData::DummyExtraData(ni_ptr<nif::NiExtraData>&& obj) :
	ExtraData(std::move(obj))
{
	setTitle("Extra data");
	setSize({ WIDTH, HEIGHT });
	newChild<gui::Separator>();
	newChild<gui::Label>(object().name().get());

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::DummyExtraData::~DummyExtraData()
{
	disconnect();
}
