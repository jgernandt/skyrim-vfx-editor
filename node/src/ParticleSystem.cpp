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
#include "ParticleSystem.h"
#include "style.h"
#include "widget_types.h"

#include "Modifier.h"
#include "ModifierRequirements.h"

//All operations on Particle system modifiers go through us.
	//This is not so much a manager anymore, it is a simple implementation of an interface.
	//There's really no need to expose this outside of ParticleSystem.
class node::ParticleSystem::ModifiersManager final : public IModifiable
{
public:
	ModifiersManager(ParticleSystem& node) :
		m_mods(node.object().modifiers()), m_ctlrs(node.object().controllers()), m_reqs(node, m_mods, m_ctlrs)
	{}

	virtual ReservableSequence<nif::NiPSysModifier>& modifiers() override { return m_mods; }
	virtual ReservableSequence<nif::NiTimeController>& controllers() override { return m_ctlrs; }
	virtual ISet<Modifier::Requirement>& requirements() override { return m_reqs; }

private:
	struct Requirements : ISet<Modifier::Requirement>
	{
		Requirements(ParticleSystem& node,
			ReservableSequence<nif::NiPSysModifier>& mods,
			ReservableSequence<nif::NiTimeController>& ctlrs);

		virtual void add(const Modifier::Requirement&) override;
		virtual void remove(const Modifier::Requirement&) override;
		virtual bool has(const Modifier::Requirement&) const override;
		virtual size_t size() const override { return 0; }//doesn't really make sense on us

		virtual void addListener(ISetListener<Modifier::Requirement>&) override {}
		virtual void removeListener(ISetListener<Modifier::Requirement>&) override {}

	private:
		std::map<Modifier::Requirement, std::unique_ptr<ModifierRequirement>> m_mngrs;
	};

	ReservableSequence<nif::NiPSysModifier> m_mods;
	ReservableSequence<nif::NiTimeController> m_ctlrs;
	Requirements m_reqs;
};

class node::ParticleSystem::MaxCountField final : public Field
{
public:
	MaxCountField(const std::string& name, ParticleSystem& node) : Field(name)
	{
		auto w = node.newChild<DragInput<unsigned short, 1>>(node.data().maxCount(), name);
		w->setSensitivity(0.5f);
		w->setLowerLimit(0);
		w->setAlwaysClamp();

		widget = w;
	}
};

class node::ParticleSystem::WorldSpaceField final : public Field
{
public:
	WorldSpaceField(const std::string& name, ParticleSystem& node) : Field(name)
	{
		widget = node.newChild<Checkbox>(node.object().worldSpace(), name);
	}
};

class node::ParticleSystem::ShaderField : public Field
{
public:
	ShaderField(const std::string& name, ParticleSystem& node) :
		Field(name), m_sender(node.object().shaderProperty())
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sender, m_rcvr));

		//Put alpha blend mode here for now. We might want more options for that later, though.
		using widget_type = gui::Selector<unsigned short, IProperty<unsigned short>>;
		auto item = node.newChild<gui::Item>();
		item->newChild<gui::Text>("Blend mode");
		item->newChild<widget_type>(node.alphaProperty().flags(), std::string(),
			widget_type::ItemList{ { 13, "Add" }, { 237, "Mix" } });
	}

private:
	Receiver<void> m_rcvr;
	Sender<IAssignable<nif::BSEffectShaderProperty>> m_sender;
};

/*If we want to receive subtexture offsets from the shader node
class node::ParticleSystem::ShaderField : public Field
{
public:
	ShaderField(const std::string& name, ParticleSystem& node) :
		Field(name), m_ifc(node.object().shaderProperty()), m_receiver(node.data().subtexOffsets()), m_sender(m_ifc)
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sender, m_receiver));
	}

private:
	//Forwards external assignments to our target and manages our default shader
	struct ShaderAssigner : IAssignable<nif::BSEffectShaderProperty>
	{
		ShaderAssigner(IAssignable<nif::BSEffectShaderProperty>& target) : m_shader{ target } {}
		~ShaderAssigner()
		{
			if (m_shader.isAssigned(&m_default))
				m_shader.assign(nullptr);
		}

		virtual void assign(nif::BSEffectShaderProperty* obj) override { m_shader.assign(obj ? obj : &m_default); }
		virtual bool isAssigned(nif::BSEffectShaderProperty* obj) const override { return m_shader.isAssigned(obj); }

		virtual void addListener(IAssignableListener<nif::BSEffectShaderProperty>&) override {}
		virtual void removeListener(IAssignableListener<nif::BSEffectShaderProperty>&) override {}

		IAssignable<nif::BSEffectShaderProperty>& m_shader;//will survive us
		nif::BSEffectShaderProperty m_default;

	};

	//Connects external subtexture properties with our target, and clears them on dc
	struct SubtexReceiver : Receiver<IProperty<nif::SubtextureCount>>
	{
		SubtexReceiver(IProperty<std::vector<nif::SubtextureOffset>>& prop) : m_target{ prop }, m_lsnr{ prop } {}

		virtual void onConnect(IProperty<nif::SubtextureCount>& ifc) override { ifc.addListener(m_lsnr); }
		virtual void onDisconnect(IProperty<nif::SubtextureCount>& ifc) override
		{
			ifc.removeListener(m_lsnr);
			m_target.set(std::vector<nif::SubtextureOffset>());
		}

		class Listener : public IPropertyListener<nif::SubtextureCount>
		{
		public:
			Listener(IProperty<std::vector<nif::SubtextureOffset>>& prop) : m_ifc{ prop } {}
			virtual ~Listener() = default;

			virtual void onSet(const nif::SubtextureCount& t) override
			{
				//if the count is irregular, we don't want to touch it
				if (t != nif::SubtextureCount{ 0, 0 })
					m_ifc.set(nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(t));
			}

		private:
			IProperty<std::vector<nif::SubtextureOffset>>& m_ifc;
		};

		IProperty<std::vector<nif::SubtextureOffset>>& m_target;
		Listener m_lsnr;
	};

	ShaderAssigner m_ifc;
	SubtexReceiver m_receiver;
	Sender<IAssignable<nif::BSEffectShaderProperty>> m_sender;
};*/

class node::ParticleSystem::ModifiersField final : public Field
{
public:
	ModifiersField(const std::string& name, ParticleSystem& node) : 
		Field(name), m_mngr(node), m_sndr(m_mngr)
	{
		//This should always be active and, the way we made it, must be first listener to the sequence
		m_mngr.requirements().add(Modifier::Requirement::UPDATE);

		//This seems to be required even if there is no movement. You get weird behaviour without it.
		m_mngr.requirements().add(Modifier::Requirement::MOVEMENT);

		//After some consideration, let's keep this permanent too. The usefulness of 0 life span particles lasting forever seems limited.
		m_mngr.requirements().add(Modifier::Requirement::LIFETIME);

		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}

private:
	ModifiersManager m_mngr;
	Receiver<void> m_rcvr;
	Sender<IModifiable> m_sndr;
};

node::ParticleSystem::ParticleSystem() : 
	ParticleSystem(
		std::make_unique<nif::NiParticleSystem>(), 
		std::unique_ptr<nif::NiPSysData>(),
		std::unique_ptr<nif::NiAlphaProperty>(),
		std::unique_ptr<nif::NiPSysUpdateCtlr>(),
		std::unique_ptr<nif::NiPSysAgeDeathModifier>(),
		std::unique_ptr<nif::NiPSysPositionModifier>(),
		std::unique_ptr<nif::NiPSysBoundUpdateModifier>())
{
}

node::ParticleSystem::ParticleSystem(
	std::unique_ptr<nif::NiParticleSystem>&& obj,
	std::unique_ptr<nif::NiPSysData>&& dat,
	std::unique_ptr<nif::NiAlphaProperty>&& alpha,
	std::unique_ptr<nif::NiPSysUpdateCtlr>&& ctlr,
	std::unique_ptr<nif::NiPSysAgeDeathModifier>&& adm,
	std::unique_ptr<nif::NiPSysPositionModifier>&& pm,
	std::unique_ptr<nif::NiPSysBoundUpdateModifier>&& bum) :
	AVObject(std::move(obj))
{
	if (!dat) {
		dat = std::make_unique<nif::NiPSysData>();
		object().data().assign(dat.get());
		dat->maxCount().set(100);
	}
	addObject(std::move(dat));
	if (!alpha) {
		alpha = std::make_unique<nif::NiAlphaProperty>();
		alpha->flags().set(13);
		object().alphaProperty().assign(alpha.get());
	}
	addObject(std::move(alpha));

	//Controllers and modifiers are all handled by our ModifiersField. 
	//We just own the objects, so they are guaranteed to survive anyone that uses them.
	if (ctlr) {
		//Move to last
		if (size_t pos = object().controllers().find(*ctlr); pos != -1)
			object().controllers().erase(pos);
		object().controllers().insert(-1, *ctlr);
	}
	else {
		ctlr = std::make_unique<nif::NiPSysUpdateCtlr>();
		ctlr->flags().set(72);
		ctlr->frequency().set(1.0f);
		ctlr->phase().set(0.0f);
		ctlr->startTime().set(0.0f);
		ctlr->stopTime().set(1.0f);
	}
	addObject(std::move(ctlr));

	if (adm) {
		//Move to first
		if (size_t pos = object().modifiers().find(*adm); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(0, *adm);
	}
	else {
		adm = std::make_unique<nif::NiPSysAgeDeathModifier>();
	}
	addObject(std::move(adm));

	if (pm) {
		//Move to last
		if (size_t pos = object().modifiers().find(*pm); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(-1, *pm);
	}
	else {
		pm = std::make_unique<nif::NiPSysPositionModifier>();
	}
	addObject(std::move(pm));

	if (bum) {
		//Move to last
		if (size_t pos = object().modifiers().find(*bum); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(-1, *bum);
	}
	else {
		bum = std::make_unique<nif::NiPSysBoundUpdateModifier>();
	}
	addObject(std::move(bum));

	m_subtexCount.set(nif::nif_type_conversion<nif::SubtextureCount>::from(data().subtexOffsets().get()));
	m_subtexLsnr = std::make_unique<SetterListener<nif::SubtextureCount, std::vector<nif::SubtextureOffset>>>(data().subtexOffsets());
	m_subtexCount.addListener(*m_subtexLsnr);

	setClosable(true);
	setColour(COL_TITLE, TitleCol_Geom);
	setColour(COL_TITLE_ACTIVE, TitleCol_GeomActive);
	setSize({ 160.0, 0.0f });
	setTitle("Particle system");

	newField<NameField>(NAME, *this);
	newField<ParentField>(PARENT, *this);
	newField<TransformField>(TRANSFORM, *this);

	newChild<gui::Separator>();

	newField<ShaderField>(SHADER, *this);

	newChild<gui::Separator>();

	newField<WorldSpaceField>(WORLD_SPACE, *this);
	newField<MaxCountField>(MAX_COUNT, *this);

	newChild<gui::Separator>();

	newChild<gui::Text>("Texture atlas layout");
	std::array<std::string, 2> labels{ "H", "V" };
	auto w = newChild<DragInputH<nif::SubtextureCount, 2>>(m_subtexCount, labels);
	w->setSensitivity(0.05f);
	w->setLowerLimit(1);
	w->setUpperLimit(100);//crazy numbers could be problematic, since we don't multithread
	w->setAlwaysClamp();

	newChild<gui::Separator>();

	newField<ModifiersField>(MODIFIERS, *this);
}

node::ParticleSystem::~ParticleSystem()
{
	assert(m_subtexLsnr);
	m_subtexCount.removeListener(*m_subtexLsnr);
}

nif::NiParticleSystem& node::ParticleSystem::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiParticleSystem*>(getObjects()[0].get());
}

nif::NiPSysData& node::ParticleSystem::data()
{
	assert(getObjects().size() > 1 && getObjects()[1]);
	return *static_cast<nif::NiPSysData*>(getObjects()[1].get());
}

nif::NiAlphaProperty& node::ParticleSystem::alphaProperty()
{
	assert(getObjects().size() > 2 && getObjects()[2]);
	return *static_cast<nif::NiAlphaProperty*>(getObjects()[2].get());
}

nif::NiPSysUpdateCtlr& node::ParticleSystem::updateCtlr()
{
	assert(getObjects().size() > 3 && getObjects()[3]);
	return *static_cast<nif::NiPSysUpdateCtlr*>(getObjects()[3].get());
}

nif::NiPSysAgeDeathModifier& node::ParticleSystem::ageDeathMod()
{
	assert(getObjects().size() > 4 && getObjects()[4]);
	return *static_cast<nif::NiPSysAgeDeathModifier*>(getObjects()[4].get());
}

nif::NiPSysPositionModifier& node::ParticleSystem::positionMod()
{
	assert(getObjects().size() > 5 && getObjects()[5]);
	return *static_cast<nif::NiPSysPositionModifier*>(getObjects()[5].get());
}

nif::NiPSysBoundUpdateModifier& node::ParticleSystem::boundUpdateMod()
{
	assert(getObjects().size() > 6 && getObjects()[6]);
	return *static_cast<nif::NiPSysBoundUpdateModifier*>(getObjects()[6].get());
}

node::ParticleSystem::ModifiersManager::Requirements::Requirements(
	ParticleSystem& node,
	ReservableSequence<nif::NiPSysModifier>& mods,
	ReservableSequence<nif::NiTimeController>& ctlrs)
{
	m_mngrs.insert({ Modifier::Requirement::COLOUR, std::make_unique<ColourRequirement>(node.data().hasColour()) });
	m_mngrs.insert({ Modifier::Requirement::LIFETIME, std::make_unique<LifetimeRequirement>(node.ageDeathMod(), mods) });
	m_mngrs.insert({ Modifier::Requirement::MOVEMENT, std::make_unique<MovementRequirement>(node.positionMod(), mods) });
	m_mngrs.insert({ Modifier::Requirement::ROTATION, 
		std::make_unique<RotationsRequirement>(node.data().hasRotationAngles(), node.data().hasRotationSpeeds()) });
	m_mngrs.insert({ Modifier::Requirement::UPDATE, 
		std::make_unique<UpdateRequirement>(node.boundUpdateMod(), node.updateCtlr(), mods, ctlrs) });
}

void node::ParticleSystem::ModifiersManager::Requirements::add(const Modifier::Requirement& req)
{
	if (auto it = m_mngrs.find(req); it != m_mngrs.end())
		it->second->incr();
}

void node::ParticleSystem::ModifiersManager::Requirements::remove(const Modifier::Requirement& req)
{
	if (auto it = m_mngrs.find(req); it != m_mngrs.end())
		it->second->decr();
}

bool node::ParticleSystem::ModifiersManager::Requirements::has(const Modifier::Requirement& req) const
{
	if (auto it = m_mngrs.find(req); it != m_mngrs.end())
		return it->second->active();
	else
		return false;
}


