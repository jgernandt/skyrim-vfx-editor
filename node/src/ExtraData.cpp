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

node::ExtraData::TargetField::TargetField(const std::string& name, NodeBase& node, const ni_ptr<NiExtraData>& obj) :
	Field(name), m_rvr(obj)
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
}

node::ExtraData::ExtraData(const ni_ptr<NiExtraData>& obj)
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_XData);
	setColour(COL_TITLE_ACTIVE, TitleCol_XDataActive);

	m_targetField = newField<TargetField>(TARGET, *this, obj);
}


node::StringDataShared::StringDataShared(const ni_ptr<NiStringExtraData>& obj) :
	ExtraData(obj)
{}


node::StringData::StringData(nif::File& file) : StringData(file.create<nif::NiStringExtraData>()) {}

node::StringData::StringData(ni_ptr<nif::NiStringExtraData>&& obj) :
	StringDataShared(obj)
{
	setTitle("String data");
	setSize({ WIDTH, HEIGHT });

	newChild<gui::Text>(NAME);
	newChild<StringInput>(make_ni_ptr(std::static_pointer_cast<NiExtraData>(obj), &NiExtraData::name));

	newChild<gui::Text>(VALUE);
	newChild<StringInput>(make_ni_ptr(obj, &NiStringExtraData::value));

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::StringData::~StringData()
{
	disconnect();
}

node::WeaponTypeData::WeaponTypeData(nif::File& file) : WeaponTypeData(file.create<nif::NiStringExtraData>()) {}

node::WeaponTypeData::WeaponTypeData(ni_ptr<nif::NiStringExtraData>&& obj) :
	StringDataShared(obj)
{
	setTitle("Weapon type data");
	setSize({ WIDTH, HEIGHT });

	assert(obj);
	obj->name.set("Prn");

	using widget_type = gui::Selector<std::string, ni_ptr<Property<std::string>>>;
	newChild<widget_type>(make_ni_ptr(obj, &NiStringExtraData::value), TYPE,
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
	ExtraData(obj)
{
	assert(obj);
	setTitle("Extra data");
	setSize({ WIDTH, HEIGHT });
	newChild<gui::Separator>();
	newChild<gui::Label>(obj->name.get());

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::DummyExtraData::~DummyExtraData()
{
	disconnect();
}
