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

//Needs better memory management!
//Sequences are passed around by reference, not updated to our new system.
//Also a lot of stuff being sent around needlessly. There's a better way to do this.
class node::ParticleSystem::ModifiersManager final : public IModifiable
{
public:
	ModifiersManager(
		OSequence<nif::NiPSysModifier>& mods,
		OSequence<nif::NiTimeController>& ctlrs,
		const ni_ptr<nif::NiPSysData>& data,
		ni_ptr<nif::NiPSysAgeDeathModifier>&& adm,
		ni_ptr<nif::NiPSysPositionModifier>&& pm,
		ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum,
		ni_ptr<nif::NiPSysUpdateCtlr>&& puc) :
		m_mods(mods),
		m_ctlrs(ctlrs),
		m_reqs(data, std::move(adm), std::move(pm), std::move(bum), std::move(puc), m_mods, m_ctlrs)
	{
		//This should always be active and, the way we made it, must be first listener to the sequence
		m_reqs.add(Modifier::Requirement::UPDATE);

		//This seems to be required even if there is no movement. You get weird behaviour without it.
		m_reqs.add(Modifier::Requirement::MOVEMENT);

		//After some consideration, let's keep this permanent too. The usefulness of 0 life span particles lasting forever seems limited.
		m_reqs.add(Modifier::Requirement::LIFETIME);
	}

	~ModifiersManager()
	{
		m_reqs.remove(Modifier::Requirement::UPDATE);
		m_reqs.remove(Modifier::Requirement::MOVEMENT);
		m_reqs.remove(Modifier::Requirement::LIFETIME);
	}

	virtual ReservableSequence<nif::NiPSysModifier>& modifiers() override { return m_mods; }
	virtual ReservableSequence<nif::NiTimeController>& controllers() override { return m_ctlrs; }
	virtual IObservable<ISet<Modifier::Requirement>>& requirements() override { return m_reqs; }

private:
	struct Requirements : IObservable<ISet<Modifier::Requirement>>
	{
		Requirements(
			const ni_ptr<nif::NiPSysData>& data,
			ni_ptr<nif::NiPSysAgeDeathModifier>&& adm,
			ni_ptr<nif::NiPSysPositionModifier>&& pm,
			ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum,
			ni_ptr<nif::NiPSysUpdateCtlr>&& puc,
			ReservableSequence<nif::NiPSysModifier>& mods,
			ReservableSequence<nif::NiTimeController>& ctlrs);

		virtual void add(const Modifier::Requirement&) override;
		virtual void remove(const Modifier::Requirement&) override;
		virtual bool has(const Modifier::Requirement&) const override;
		virtual size_t size() const override { return 0; }//doesn't really make sense on us

		virtual void addListener(nif::SetListener<Modifier::Requirement>&) override { assert(false); }
		virtual void removeListener(nif::SetListener<Modifier::Requirement>&) override { assert(false); }

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

		virtual void addListener(AssignableListener<nif::BSEffectShaderProperty>&) override {}
		virtual void removeListener(AssignableListener<nif::BSEffectShaderProperty>&) override {}

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

		class Listener : public PropertyListener<nif::SubtextureCount>
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
	ModifiersField(const std::string& name, ParticleSystem& node, std::unique_ptr<IModifiable>&& ifc) : 
		Field(name), m_ifc{ std::move(ifc) }, m_sndr(*m_ifc)
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}

private:
	std::unique_ptr<IModifiable> m_ifc;
	Receiver<void> m_rcvr;
	Sender<IModifiable> m_sndr;
};

node::ParticleSystem::ParticleSystem(nif::File& file) :
	ParticleSystem(file,
		file.create<nif::NiParticleSystem>(), 
		ni_ptr<nif::NiPSysData>(),
		ni_ptr<nif::NiAlphaProperty>(),
		ni_ptr<nif::NiPSysUpdateCtlr>(),
		ni_ptr<nif::NiPSysAgeDeathModifier>(),
		ni_ptr<nif::NiPSysPositionModifier>(),
		ni_ptr<nif::NiPSysBoundUpdateModifier>())
{
}

node::ParticleSystem::ParticleSystem(nif::File& file,
	ni_ptr<nif::NiParticleSystem>&& obj,
	ni_ptr<nif::NiPSysData>&& dat,
	ni_ptr<nif::NiAlphaProperty>&& alpha,
	ni_ptr<nif::NiPSysUpdateCtlr>&& ctlr,
	ni_ptr<nif::NiPSysAgeDeathModifier>&& adm,
	ni_ptr<nif::NiPSysPositionModifier>&& pm,
	ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum) :
	AVObject(std::move(obj)),
	m_data{ std::move(dat) },
	m_alpha{ std::move(alpha) }
{
	if (!m_data) {
		m_data = file.create<nif::NiPSysData>();
		object().data().assign(m_data.get());
		m_data->maxCount().set(100);
	}

	if (!m_alpha) {
		m_alpha = file.create<nif::NiAlphaProperty>();
		m_alpha->flags().set(13);
		object().alphaProperty().assign(m_alpha.get());
	}

	//Controllers and modifiers are all handled by our ModifiersField.
	if (ctlr) {
		//Move to last
		if (size_t pos = object().controllers().find(*ctlr); pos != -1)
			object().controllers().erase(pos);
		object().controllers().insert(-1, *ctlr);
	}
	else {
		ctlr = file.create<nif::NiPSysUpdateCtlr>();
		ctlr->flags().set(72);
		ctlr->frequency().set(1.0f);
		ctlr->phase().set(0.0f);
		ctlr->startTime().set(0.0f);
		ctlr->stopTime().set(1.0f);
	}

	if (adm) {
		//Move to first
		if (size_t pos = object().modifiers().find(*adm); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(0, *adm);
	}
	else {
		adm = file.create<nif::NiPSysAgeDeathModifier>();
	}

	if (pm) {
		//Move to last
		if (size_t pos = object().modifiers().find(*pm); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(-1, *pm);
	}
	else {
		pm = file.create<nif::NiPSysPositionModifier>();
	}

	if (bum) {
		//Move to last
		if (size_t pos = object().modifiers().find(*bum); pos != -1)
			object().modifiers().erase(pos);
		object().modifiers().insert(-1, *bum);
	}
	else {
		bum = file.create<nif::NiPSysBoundUpdateModifier>();
	}

	m_subtexCount.set(nif::nif_type_conversion<nif::SubtextureCount>::from(data().subtexOffsets().get()));
	m_subtexLsnr = std::make_unique<SetterListener<nif::SubtextureCount, std::vector<nif::SubtextureOffset>>>(data().subtexOffsets());
	m_subtexCount.addListener(*m_subtexLsnr);
	m_subtexLsnr->onSet(m_subtexCount.get());

	setClosable(true);
	setColour(COL_TITLE, TitleCol_Geom);
	setColour(COL_TITLE_ACTIVE, TitleCol_GeomActive);
	setSize({ WIDTH, HEIGHT });
	setTitle("Particle system");

	m_name = newField<NameField>(NAME, *this);
	m_parent = newField<ParentField>(PARENT, *this);
	m_transform = newField<TransformField>(TRANSFORM, *this);

	newChild<gui::Separator>();

	m_shaderField = newField<ShaderField>(SHADER, *this);

	newChild<gui::Separator>();

	m_worldSpaceField = newField<WorldSpaceField>(WORLD_SPACE, *this);
	m_maxCountField = newField<MaxCountField>(MAX_COUNT, *this);

	newChild<gui::Separator>();

	newChild<gui::Text>("Texture atlas layout");
	std::array<std::string, 2> labels{ "H", "V" };
	auto w = newChild<DragInputH<nif::SubtextureCount, 2>>(m_subtexCount, labels);
	w->setSensitivity(0.05f);
	w->setLowerLimit(1);
	w->setUpperLimit(100);//crazy numbers could be problematic, since we don't multithread
	w->setAlwaysClamp();

	newChild<gui::Separator>();

	auto modifiable = std::make_unique<ModifiersManager>(object().modifiers(), object().controllers(), 
		m_data, std::move(adm), std::move(pm), std::move(bum), std::move(ctlr));
	m_modifiersField = newField<ModifiersField>(MODIFIERS, *this, std::move(modifiable));

	//until we have some other way to determine connector position for loading placement
	getField(PARENT)->connector->setTranslation({ 0.0f, 62.0f });
	getField(SHADER)->connector->setTranslation({ WIDTH, 114.0f });
	getField(MODIFIERS)->connector->setTranslation({ WIDTH, 264.0f });
}

node::ParticleSystem::~ParticleSystem()
{
	assert(m_subtexLsnr);
	m_subtexCount.removeListener(*m_subtexLsnr);

	disconnect();
}

nif::NiParticleSystem& node::ParticleSystem::object()
{
	assert(m_obj);
	return *static_cast<nif::NiParticleSystem*>(m_obj.get());
}

nif::NiPSysData& node::ParticleSystem::data()
{
	assert(m_data);
	return *m_data;
}

nif::NiAlphaProperty& node::ParticleSystem::alphaProperty()
{
	assert(m_alpha);
	return *m_alpha;
}

node::ParticleSystem::ModifiersManager::Requirements::Requirements(
	const ni_ptr<nif::NiPSysData>& data,
	ni_ptr<nif::NiPSysAgeDeathModifier>&& adm,
	ni_ptr<nif::NiPSysPositionModifier>&& pm,
	ni_ptr<nif::NiPSysBoundUpdateModifier>&& bum,
	ni_ptr<nif::NiPSysUpdateCtlr>&& puc,
	ReservableSequence<nif::NiPSysModifier>& mods,
	ReservableSequence<nif::NiTimeController>& ctlrs)
{
	assert(data);
	m_mngrs.insert(
		{ Modifier::Requirement::COLOUR, 
		std::make_unique<ColourRequirement>(nif::make_field_ptr(data, &data->hasColour())) });

	assert(adm);
	m_mngrs.insert(
		{ Modifier::Requirement::LIFETIME, std::make_unique<LifetimeRequirement>(std::move(adm), mods) });

	assert(pm);
	m_mngrs.insert(
		{ Modifier::Requirement::MOVEMENT, std::make_unique<MovementRequirement>(std::move(pm), mods) });

	m_mngrs.insert({ Modifier::Requirement::ROTATION, 
		std::make_unique<RotationsRequirement>(
			nif::make_field_ptr(data, &data->hasRotationAngles()), nif::make_field_ptr(data, &data->hasRotationSpeeds())) });

	assert(bum && puc);
	m_mngrs.insert({ Modifier::Requirement::UPDATE, 
		std::make_unique<UpdateRequirement>(std::move(bum), std::move(puc), mods, ctlrs) });
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


