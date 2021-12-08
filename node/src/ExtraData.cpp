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

using namespace nif;
using namespace node;

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


node::StringData::StringData(const ni_ptr<nif::NiStringExtraData>& obj) :
	StringDataShared(obj)
{
	setTitle("String data");
	setSize({ WIDTH, HEIGHT });

	newChild<gui::Separator>();

	newChild<gui::Text>("Name");
	newChild<StringInput>(make_ni_ptr(std::static_pointer_cast<NiExtraData>(obj), &NiExtraData::name));

	newChild<gui::Text>("String");
	newChild<StringInput>(make_ni_ptr(obj, &NiStringExtraData::value));

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::StringData::~StringData()
{
	disconnect();
}

node::WeaponTypeData::WeaponTypeData(const ni_ptr<nif::NiStringExtraData>& obj) :
	StringDataShared(obj)
{
	setTitle("Weapon type data");
	setSize({ WIDTH, HEIGHT });

	assert(obj);
	obj->name.set("Prn");

	newChild<gui::Separator>();
	newChild<gui::Text>("Type");

	using widget_type = gui::Selector<std::string, ni_ptr<Property<std::string>>>;
	newChild<widget_type>(make_ni_ptr(obj, &NiStringExtraData::value), "",
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


struct AttachTProperty
{
	ni_ptr<Vector<Property<std::string>>> target;
};

template<>
struct util::property_traits<AttachTProperty>
{
	using property_type = AttachTProperty;
	using value_type = std::string;
	using get_type = std::string;

	static std::string get(const AttachTProperty& p) 
	{ 
		assert(p.target && p.target->size() == 1);
		auto&& s = p.target->at(0).get();
		assert(s.size() > 9 && s[9] == '&');
		if (s.size() > 10)
			return s.substr(10);
		else
			return std::string();
	}
	static void set(AttachTProperty& p, const std::string& val) 
	{
		assert(p.target && p.target->size() == 1);
		p.target->at(0).set(std::string("NamedNode&") + val);
	}
};

node::AttachPointData::AttachPointData(const ni_ptr<NiStringsExtraData>& obj) :
	ExtraData(obj)
{
	setTitle("Attach point");
	setSize({ WIDTH, HEIGHT });

	assert(obj);
	obj->name.set("AttachT");

	obj->strings.resize(1);
	if (obj->strings.at(0).get().find("NamedNode&") == -1)
		obj->strings.at(0).set("NamedNode&MagicEffectsNode");

	newChild<gui::Separator>();
	newChild<gui::Text>("Bone");
	newChild<gui::TextInput<AttachTProperty>>(AttachTProperty{ make_ni_ptr(obj, &NiStringsExtraData::strings) });

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ 0.0f, 38.0f });
}

node::AttachPointData::~AttachPointData()
{
	disconnect();
}

node::AttachPointData::PreWriteProcessor::PreWriteProcessor(File& file) :
	m_file{ file }
{
}

void node::AttachPointData::PreWriteProcessor::traverse(NiNode& obj)
{
	m_current = &obj;

	for (auto&& data : obj.extraData) {
		assert(data);
		data->receive(*this);
	}

	for (auto&& child : obj.children) {
		assert(child);
		child->receive(*this);
	}

	if (&obj == m_file.getRoot().get()) {
		if (m_needMulti != (m_multiTech != nullptr)) {
			if (m_needMulti) {
				//create new
				auto data = m_file.create<NiStringsExtraData>();
				data->name.set("AttachT");
				data->strings.resize(1);
				data->strings.at(0).set("MultiTechnique");
				obj.extraData.add(data);
			}
			else {
				//remove existing
				obj.extraData.remove(m_multiTech);
			}
		}
	}
}

void node::AttachPointData::PreWriteProcessor::traverse(BSFadeNode& obj)
{
	PreWriteProcessor::traverse(static_cast<NiNode&>(obj));
}

void node::AttachPointData::PreWriteProcessor::traverse(NiStringsExtraData& obj)
{
	if (obj.name.get() == "AttachT") {
		if (m_current == m_file.getRoot().get()) {
			if (obj.strings.size() > 0 && obj.strings.at(0).get() == "MultiTechnique")
				m_multiTech = &obj;
		}
		else {
			if (obj.strings.size() > 0 && obj.strings.at(0).get().find("NamedNode&") != -1)
				m_needMulti = true;
		}
	}
}


node::DummyExtraData::DummyExtraData(const ni_ptr<nif::NiExtraData>& obj) :
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
