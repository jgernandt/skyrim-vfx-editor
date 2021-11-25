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
#include "Modifier.h"
#include "ParticleSystem.h"

#include "style.h"
#include "widget_types.h"

using namespace nif;


class node::ParticleSystem::ModifiersField final :
	public Field, public IModifiable, public SequenceListener<NiPSysModifier>
{
	Receiver<void> m_rcvr;
	Sender<IModifiable> m_sndr;

	const ni_ptr<NiParticleSystem> m_psys;
	const ni_ptr<NiPSysData> m_data;

	const std::weak_ptr<NiPSysAgeDeathModifier> m_adm;
	const std::weak_ptr<NiPSysPositionModifier> m_pm;
	const std::weak_ptr<NiPSysBoundUpdateModifier> m_bum;

	Modifier::NameUpdater m_admName;
	Modifier::NameUpdater m_bumName;
	Modifier::NameUpdater m_pmName;

	int m_colReqs{ 0 };
	int m_rotReqs{ 0 };

public:
	ModifiersField(
		const std::string& name, 
		NodeBase& node,
		const ni_ptr<NiParticleSystem>& psys,
		const ni_ptr<NiPSysData>& data,
		const ni_ptr<NiPSysAgeDeathModifier>& adm,
		const ni_ptr<NiPSysPositionModifier>& pm,
		const ni_ptr<NiPSysBoundUpdateModifier>& bum,
		const ni_ptr<NiPSysUpdateCtlr>& puc)
		:
		Field{ name },
		m_sndr{ *this },
		m_psys{ psys },
		m_data{ data },
		m_adm{ adm },
		m_pm{ pm },
		m_bum{ bum },
		m_admName{ make_ni_ptr<Property<std::string>, NiPSysModifier>(adm, &NiPSysModifier::name) },
		m_bumName{ make_ni_ptr<Property<std::string>, NiPSysModifier>(bum, &NiPSysModifier::name) },
		m_pmName{ make_ni_ptr<Property<std::string>, NiPSysModifier>(pm, &NiPSysModifier::name) }
	{
		assert(m_psys && m_data && adm && bum && pm);

		//Simplest way to assure the right position is to always erase first
		if (int i = m_psys->modifiers.find(adm.get()); i >= 0)
			m_psys->modifiers.erase(i);
		adm->order.set(-1);

		if (int i = m_psys->modifiers.find(bum.get()); i >= 0)
			m_psys->modifiers.erase(i);
		bum->order.set(-1);

		if (int i = m_psys->modifiers.find(pm.get()); i >= 0)
			m_psys->modifiers.erase(i);
		pm->order.set(-1);

		if (int i = m_psys->controllers.find(puc.get()); i >= 0)
			m_psys->controllers.erase(i);

		//We are updating the order of modifiers...
		m_psys->modifiers.addListener(*this);

		//...and the names of static mods
		adm->order.addListener(m_admName);
		bum->order.addListener(m_bumName);
		pm->order.addListener(m_pmName);

		m_psys->modifiers.insert(0, adm);
		adm->target.assign(m_psys);

		m_psys->modifiers.insert(m_psys->modifiers.size(), pm);
		pm->target.assign(m_psys);

		m_psys->modifiers.insert(m_psys->modifiers.size(), bum);
		bum->target.assign(m_psys);

		m_psys->controllers.insert(m_psys->controllers.size(), puc);
		puc->target.assign(m_psys);

		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}

	~ModifiersField()
	{
		if (auto adm = m_adm.lock())
			adm->order.removeListener(m_admName);
		if (auto pm = m_pm.lock())
			pm->order.removeListener(m_pmName);
		if (auto bum = m_bum.lock())
			bum->order.removeListener(m_bumName);
		m_psys->modifiers.removeListener(*this);
	}

	virtual void addModifier(const ni_ptr<NiPSysModifier>& mod) override 
	{
		//Insert before position mod (end - 2)
		if (mod) {
			m_psys->modifiers.insert(m_psys->modifiers.size() - 2, mod);
			mod->target.assign(m_psys);
		}
	}
	virtual void removeModifier(NiPSysModifier* mod) override 
	{
		if (mod) {
			if (int pos = m_psys->modifiers.find(mod); pos >= 0) {
				m_psys->modifiers.erase(pos);
				mod->order.set(-1);
				mod->target.assign(nullptr);
			}
		}
	}

	virtual void addController(const ni_ptr<NiTimeController>& ctlr) override 
	{
		//Insert before update ctlr (end - 1)
		if (ctlr) {
			m_psys->controllers.insert(m_psys->controllers.size() - 1, ctlr);
			ctlr->target.assign(m_psys);
		}
	}
	virtual void removeController(NiTimeController* ctlr) override
	{
		if (ctlr) {
			if (int pos = m_psys->controllers.find(ctlr); pos >= 0) {
				m_psys->controllers.erase(pos);
				ctlr->target.assign(nullptr);
			}
		}
	}

	virtual void addRequirement(ModRequirement req) override 
	{
		switch (req) {
		case ModRequirement::COLOUR:
			if (++m_colReqs == 1) {
				m_data->hasColour.set(true);
			}
			break;
		case ModRequirement::ROTATION:
			if (++m_rotReqs == 1) {
				m_data->hasRotationAngles.set(true);
				m_data->hasRotationSpeeds.set(true);
			}
			break;
		}
	}
	virtual void removeRequirement(ModRequirement req) override 
	{
		switch (req) {
		case ModRequirement::COLOUR:
			if (--m_colReqs == 0) {
				m_data->hasColour.set(false);
			}
			break;
		case ModRequirement::ROTATION:
			if (--m_rotReqs == 0) {
				m_data->hasRotationAngles.set(false);
				m_data->hasRotationSpeeds.set(false);
			}
			break;
		}
	}

	virtual void onInsert(int pos) override 
	{
		//update order of all mods >= pos
		for (; pos < m_psys->modifiers.size(); pos++)
			m_psys->modifiers.at(pos)->order.set(pos);
	}
	virtual void onErase(int pos) override { ModifiersField::onInsert(pos); }
};

class node::ParticleSystem::MaxCountField final : public Field
{
public:
	MaxCountField(const std::string& name, NodeBase& node, ni_ptr<Property<unsigned short>>&& maxCount) : 
		Field(name)
	{
		auto w = node.newChild<DragInput<unsigned short, 1>>(maxCount, name);
		w->setSensitivity(0.5f);
		w->setLowerLimit(0);
		w->setAlwaysClamp();

		widget = w;
	}
};

class node::ParticleSystem::ShaderField : public Field
{
public:
	ShaderField(const std::string& name, NodeBase& node, ni_ptr<Assignable<BSShaderProperty>>&& shaderProperty) :
		Field(name), 
		m_sender(*shaderProperty)//old format, should use ptr
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sender, m_rcvr));
	}

private:
	Receiver<void> m_rcvr;
	Sender<Assignable<BSShaderProperty>> m_sender;
};


node::ParticleSystem::ParticleSystem(
	const ni_ptr<NiParticleSystem>& psys,
	const ni_ptr<NiPSysData>& data,
	const ni_ptr<NiAlphaProperty>& alpha,
	const ni_ptr<NiPSysAgeDeathModifier>& adm,
	const ni_ptr<NiPSysBoundUpdateModifier>& bum,
	const ni_ptr<NiPSysPositionModifier>& pm,
	const ni_ptr<NiPSysUpdateCtlr>& ctlr) :
	AVObject{ psys },
	m_subtexLsnr{ make_ni_ptr(data, &NiPSysData::subtexOffsets) },
	m_subtexCount{ std::make_shared<Property<SubtextureCount>>() }
{
	assert(psys);

	//Controllers and modifiers are all handled by our ModifiersField.

	m_subtexCount->set(node_conversion<SubtextureCount>::from(data->subtexOffsets.get()));
	m_subtexCount->addListener(m_subtexLsnr);

	setClosable(true);
	setColour(COL_TITLE, TitleCol_Geom);
	setColour(COL_TITLE_ACTIVE, TitleCol_GeomActive);
	setSize({ WIDTH, HEIGHT });
	setTitle("Particle system");

	m_name = newField<NameField>(NAME, *this, make_ni_ptr(std::static_pointer_cast<NiObjectNET>(psys), &NiObjectNET::name));
	m_parent = newField<ParentField>(PARENT, *this, psys);
	m_transform = newField<TransformField>(TRANSFORM, *this, make_ni_ptr(std::static_pointer_cast<NiAVObject>(psys), &NiAVObject::transform));

	newChild<gui::Separator>();

	m_shaderField = newField<ShaderField>(SHADER, *this, make_ni_ptr(psys, &NiParticleSystem::shaderProperty));

	//Put alpha blend mode here for now. We might want more options for that later, though.
	using widget_type = gui::Selector<BlendFunction, ni_ptr<Property<BlendFunction>>>;
	auto item = newChild<gui::Item>();
	item->newChild<gui::Text>("Blend mode");
	item->newChild<widget_type>(make_ni_ptr(alpha, &NiAlphaProperty::dstFcn), std::string(),
		widget_type::ItemList{ { BlendFunction::ONE, "Add" }, { BlendFunction::ONE_MINUS_SRC_ALPHA, "Mix" } });

	newChild<gui::Separator>();

	newChild<Checkbox>(make_ni_ptr(psys, &NiParticleSystem::worldSpace), WORLD_SPACE);
	m_maxCountField = newField<MaxCountField>(MAX_COUNT, *this, make_ni_ptr(data, &NiPSysData::maxCount));

	newChild<gui::Separator>();

	newChild<gui::Text>("Texture atlas layout");
	std::array<std::string, 2> labels{ "H", "V" };
	auto w = newChild<DragInputH<SubtextureCount, 2>>(m_subtexCount, labels);
	w->setSensitivity(0.05f);
	w->setLowerLimit(1);
	w->setUpperLimit(100);//crazy numbers could be problematic, since we don't multithread
	w->setAlwaysClamp();

	newChild<gui::Separator>();

	m_modifiersField = newField<ModifiersField>(
		MODIFIERS,
		*this,
		psys,
		data,
		adm,
		pm,
		bum,
		ctlr);

	//until we have some other way to determine connector position for loading placement
	getField(PARENT)->connector->setTranslation({ 0.0f, 62.0f });
	getField(SHADER)->connector->setTranslation({ WIDTH, 114.0f });
	getField(MODIFIERS)->connector->setTranslation({ WIDTH, 264.0f });
}

node::ParticleSystem::~ParticleSystem()
{
	m_subtexCount->removeListener(m_subtexLsnr);
	disconnect();
}
