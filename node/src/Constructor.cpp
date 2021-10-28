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
#include "Constructor.h"
#include "nodes.h"
#include "Positioner.h"

//We need to know about the nif backend, which is somewhat stupid, but the alternatives seem unreasonably complicated
#include "nif_backend.h"

void node::Constructor::makeRoot(const nif::ni_ptr<nif::native::NiObject>& root)
{
	//Exceptional cases: root is null or not a NiNode. What do we do?
	//Create one and leave a warning? Just throw?

	if (!root) {
		throw std::runtime_error("File has no root");
	}
	else if (!root->GetType().IsDerivedType(Niflib::NiNode::TYPE)) {
		throw std::runtime_error("Invalid root");
	}
	else {
		//Leave room for the root node (maybe vanity, but I'd prefer it to be first)
		m_nodes.resize(1);
		EP_process(static_cast<Niflib::NiAVObject*>(root.get()));
		for (auto&& ctlr : static_cast<Niflib::NiObjectNET*>(root.get())->GetControllers()) {
			if (Niflib::DynamicCast<Niflib::NiControllerManager>(ctlr)) {
				m_warnings.push_back("This file contains behaviour-controlled animations. This functionality is not yet supported, and any edit to this file may break it.");
				break;
			}
		}
	}
}

void node::Constructor::extractNodes(gui::ConnectionHandler& target)
{
	std::vector<std::pair<gui::Connector*, gui::Connector*>> couplings;

	//symmetric matrix indicating connections between nodes
	//(this should be some sparse format instead)
	Eigen::MatrixXf connectivity = Eigen::MatrixXf::Zero(m_nodes.size(), m_nodes.size());

	for (auto&& item : m_connections) {
		gui::Connector* c1 = nullptr;
		int i1;
		if (auto it = m_objectMap.find(item.object1); it != m_objectMap.end()) {
			if (it->second >= 0)
				if (Field* f = m_nodes[it->second]->getField(item.field1)) {
					c1 = f->connector;
					i1 = it->second;
				}
		}

		gui::Connector* c2 = nullptr;
		int i2;
		if (auto it = m_objectMap.find(item.object2); it != m_objectMap.end()) {
			if (it->second >= 0)
				if (Field* f = m_nodes[it->second]->getField(item.field2)) {
					c2 = f->connector;
					i2 = it->second;
				}
		}

		if (c1 && c2) {
			couplings.push_back({ c1, c2 });
			assert(i1 < static_cast<int>(m_nodes.size()) && i2 < static_cast<int>(m_nodes.size()));
			if (i1 != i2) {
				connectivity(i1, i2) = 1.0f;
				connectivity(i2, i1) = 1.0f;
			}
			//if somehow i1 == i2 we ignore it
		}
	}
	m_connections.clear();

	if (m_nodes.size() > 1) {
		Positioner solver;
		auto pos = solver.solve(connectivity);
		if (pos.size() == 2 * m_nodes.size()) {
			for (int i = 0; i < static_cast<int>(m_nodes.size()); i++)
				m_nodes[i]->setTranslation({ pos[i], pos[i + m_nodes.size()] });
		}
	}

	for (auto&& node : m_nodes) {
		target.addChild(std::move(node));
	}
	m_nodes.clear();

	for (auto&& pair : couplings) {
		pair.first->setConnectionState(pair.second, true);
		pair.second->setConnectionState(pair.first, true);
	}

}

void node::Constructor::EP_process(nif::native::NiAVObject* obj)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			std::unique_ptr<AVObject> node;

			if (obj->GetType().IsDerivedType(Niflib::NiNode::TYPE)) {
				node = process(static_cast<Niflib::NiNode*>(obj));
			}
			else if (obj->GetType().IsDerivedType(Niflib::NiParticleSystem::TYPE)) {
				node = process(static_cast<Niflib::NiParticleSystem*>(obj));
			}

			if (!node) {
				//We did not recognise this type. Replace it with a dummy object.
				node = std::make_unique<DummyAVObject>(std::make_unique<nif::NiAVObject>(obj));
			}

			if (dynamic_cast<Root*>(node.get())) {
				m_objectMap.insert({ obj, 0 });
				m_nodes[0] = std::move(node);
			}
			else {
				m_objectMap.insert({ obj, m_nodes.size() });
				m_nodes.push_back(std::move(node));
			}

			for (auto&& data : obj->GetExtraData()) {
				EP_process(data);

				Connection c;
				c.object1 = obj;
				c.field1 = AVObject::EXTRA_DATA;
				c.object2 = data;
				c.field2 = ExtraData::TARGET;
				m_connections.push_back(c);
			}
		}
		else {
			//This is a file error; the AVObject is a child of multiple Nodes
			std::string s = obj->GetName();
			s.append(" has multiple parents");
			m_warnings.push_back(std::move(s));
		}
	}
}

std::unique_ptr<node::NodeShared> node::Constructor::process(nif::native::NiNode* obj)
{
	assert(obj);

	//We can't handle destruction of skin instances right now, since we disconnect the bones (potentially destroying them)
	//before destroying the skin instance (incompatible with niflib). It makes no sense to show the bones as regular nodes
	//anyway, since it would be an error to remove (or even edit) them. 
	//Until we can provide a proper interface for skinned meshes, we disallow them.
	if (obj->IsSkeletonRoot())
		throw std::runtime_error("Skinned meshes are not supported. File will not be loaded.");

	std::unique_ptr<NodeShared> node;
	if (!obj->GetParent())
		node = std::make_unique<Root>(std::make_unique<nif::NiNode>(obj));
	else
		node = std::make_unique<Node>(std::make_unique<nif::NiNode>(obj));

	for (auto&& child : obj->GetChildren()) {
		EP_process(child);

		Connection c;
		c.object1 = obj;
		c.field1 = Node::CHILDREN;
		c.object2 = child;
		c.field2 = AVObject::PARENT;
		m_connections.push_back(c);
	}

	return std::move(node);
}

std::unique_ptr<node::ParticleSystem> node::Constructor::process(nif::native::NiParticleSystem* obj)
{
	using namespace Niflib;

	assert(obj);

	std::unique_ptr<ParticleSystem> node;
	std::unique_ptr<nif::NiParticleSystem> psys = std::make_unique<nif::NiParticleSystem>(obj);
	std::unique_ptr<nif::NiPSysUpdateCtlr> ctlr;
	std::unique_ptr<nif::NiPSysData> data;
	std::unique_ptr<nif::NiAlphaProperty> alpha;
	std::unique_ptr<nif::NiPSysAgeDeathModifier> adm;
	std::unique_ptr<nif::NiPSysPositionModifier> pm;
	std::unique_ptr<nif::NiPSysBoundUpdateModifier> bum;

	bool incomplete = false;
	bool irregular = false;
	bool discarded = false;

	if (Niflib::NiPSysData* d = Niflib::DynamicCast<Niflib::NiPSysData>(obj->GetData())) {
		if (auto result = m_objectMap.insert({ d, -1 }); !result.second) {
			//File error: This data is used by multiple objects
			std::string s = obj->GetName();
			s.append(" shares its data with another object");
			m_warnings.push_back(std::move(s));
		}
		data = std::make_unique<nif::NiPSysData>(d);
	}
	else
		incomplete = true;

	if (NiAlphaProperty* a = obj->GetAlphaProperty()) {
		if (auto result = m_objectMap.insert({ a, -1 }); !result.second) {
			//This is not an error, but will be a problem for us since we're not exposing this alpha property. We should clone it.
			// 
			//NiAlphaPropertyRef clone = StaticCast<NiAlphaProperty>(a->Clone(s_version, s_userVersion));
			//Clone is broken. Doesn't provide a header for indexed string i/o. We'll do it manually:
			NiAlphaPropertyRef clone = new NiAlphaProperty;
			clone->SetFlags(a->GetFlags());
			clone->SetTestThreshold(a->GetTestThreshold());
			//If, by any chance, there is extra data, we can just add it:
			for (auto&& ed : a->GetExtraData())
				clone->AddExtraData(ed);
			//Controllers, however, would have to be cloned as well. 
			//Controlled alpha properties are rare, so let's just leave that for another happy day.
			if (a->IsAnimated())
				m_warnings.push_back(obj->GetName() + " is sharing a controlled alpha property with another object");

			obj->SetAlphaProperty(clone);
			alpha = std::make_unique<nif::NiAlphaProperty>(clone);
		}
		else
			alpha = std::make_unique<nif::NiAlphaProperty>(a);
	}
	else
		incomplete = true;

	//Locate the update ctlr. If there is none, create one. If there are many, discard all but one.
	for (auto&& c : obj->GetControllers()) {
		if (Niflib::NiPSysUpdateCtlr* u = Niflib::DynamicCast<Niflib::NiPSysUpdateCtlr>(c)) {
			if (!ctlr) {
				if (auto result = m_objectMap.insert({ u, -1 }); !result.second) {
					//File error: Controller is used by multiple objects
					std::string s = obj->GetName();
					s.append(" shares its update controller with another object");
					m_warnings.push_back(std::move(s));
				}
				ctlr = std::make_unique<nif::NiPSysUpdateCtlr>(u);
			}
			else {
				//there was more than one update ctlr, which is a file error (?)
				obj->RemoveController(c);//our list is a copy, so this is fine here
				discarded = true;
			}
		}
	}
	if (!ctlr)
		incomplete = true;

	//Modifiers. We should sort the list in order. Find the fundamental mods. Maybe we move them, maybe let ParticleSystem do it.
	auto compare = [](const Niflib::NiPSysModifierRef& first, const Niflib::NiPSysModifierRef& second) -> bool
	{
		if (first && second)
			return first->GetOrder() < second->GetOrder();
		else
			return false;
	};
	auto&& mods = obj->GetModifiers();
	std::sort(mods.begin(), mods.end(), compare);
	for (size_t i = 0; i < mods.size(); i++) {
		if (mods[i])
			mods[i]->SetOrder(i);
		else {
			//Shouldn't happen, but if it did we'd have a problem
			mods.erase(mods.begin() + i);
			i--;
		}
	}

	auto&& ctlrs = obj->GetControllers();
	nif::native::NiPSysModifier* lastMod = nullptr;

	for (auto it = mods.begin(); it != mods.end(); ++it) {
		if (Niflib::NiPSysAgeDeathModifier* derived = Niflib::DynamicCast<Niflib::NiPSysAgeDeathModifier>(*it)) {
			if (!adm) {
				if (auto result = m_objectMap.insert({ *it, -1 }); !result.second) {
					//File error: Modifier is used by multiple objects
					std::string s = obj->GetName() + ":" + (*it)->GetName() + " is used by multiple objects";
					m_warnings.push_back(std::move(s));
				}
				adm = std::make_unique<nif::NiPSysAgeDeathModifier>(derived);
			}
			else {
				//there was more than one, which is useless (?). Discard it.
				it = mods.erase(it);
				--it;
				discarded = true;
			}
		}
		else if (Niflib::NiPSysPositionModifier* derived = Niflib::DynamicCast<Niflib::NiPSysPositionModifier>(*it)) {
			if (!pm) {
				if (auto result = m_objectMap.insert({ *it, -1 }); !result.second) {
					//File error: Modifier is used by multiple objects
					std::string s = obj->GetName() + ":" + (*it)->GetName() + " is used by multiple objects";
					m_warnings.push_back(std::move(s));
				}
				pm = std::make_unique<nif::NiPSysPositionModifier>(derived);
			}
			else {
				//there was more than one, which is useless (?). Discard it.
				it = mods.erase(it);
				--it;
				discarded = true;
			}
		}
		else if (Niflib::NiPSysBoundUpdateModifier* derived = Niflib::DynamicCast<Niflib::NiPSysBoundUpdateModifier>(*it)) {
			if (!bum) {
				if (auto result = m_objectMap.insert({ *it, -1 }); !result.second) {
					//File error: Modifier is used by multiple objects
					std::string s = obj->GetName() + ":" + (*it)->GetName() + " is used by multiple objects";
					m_warnings.push_back(std::move(s));
				}
				bum = std::make_unique<nif::NiPSysBoundUpdateModifier>(derived);
			}
			else {
				//there was more than one, which is useless (?). Discard it.
				it = mods.erase(it);
				--it;
				discarded = true;
			}
		}
		else {
			EP_process(*it, ctlrs);

			Connection c;
			c.object1 = lastMod ? static_cast<nif::native::NiObject*>(lastMod) : obj;
			c.field1 = lastMod ? Modifier::NEXT_MODIFIER : ParticleSystem::MODIFIERS;
			c.object2 = *it;
			c.field2 = Modifier::TARGET;
			m_connections.push_back(c);

			lastMod = *it;
		}
	}

	if (incomplete) {
		std::string s = obj->GetName();
		s.append(":\nOne or more missing components were added");
		m_warnings.push_back(s);
	}
	if (irregular) {
		std::string s = obj->GetName();
		s.append(":\nOne or more modifiers were reordered");
		m_warnings.push_back(s);
	}
	if (discarded) {
		std::string s = obj->GetName();
		s.append(":\nOne or more redundant modifiers were discarded");
		m_warnings.push_back(s);
	}

	node = std::make_unique<ParticleSystem>(std::move(psys), std::move(data), std::move(alpha), std::move(ctlr), std::move(adm), std::move(pm), std::move(bum));

	if (node->subtexCount().get() == nif::SubtextureCount{ 0, 0 })
		m_warnings.push_back(obj->GetName() + " has an irregular texture atlas layout. Edits to this field cannot be undone");

	if (BSEffectShaderProperty* shader = DynamicCast<BSEffectShaderProperty>(obj->GetShaderProperty())) {
		EP_process(shader);

		Connection c;
		c.object1 = obj;
		c.object2 = shader;
		c.field1 = ParticleSystem::SHADER;
		c.field2 = EffectShader::GEOMETRY;
		m_connections.push_back(c);
	}

	return std::move(node);
}

void node::Constructor::EP_process(nif::native::NiExtraData* obj)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			//construct new node
			std::unique_ptr<ExtraData> node;
			if (obj->GetType().IsDerivedType(Niflib::NiStringExtraData::TYPE))
				node = process(static_cast<Niflib::NiStringExtraData*>(obj));

			if (!node)
				node = std::make_unique<DummyExtraData>(std::make_unique<nif::NiExtraData>(obj));

			m_objectMap.insert({ obj, m_nodes.size() });
			m_nodes.push_back(std::move(node));
		}
		//else ignore
	}
}

std::unique_ptr<node::StringDataShared> node::Constructor::process(nif::native::NiStringExtraData* obj)
{
	assert(obj);
	std::unique_ptr<StringDataShared> node;

	if (obj->GetName() == "Prn")
		node = std::make_unique<WeaponTypeData>(std::make_unique<nif::NiStringExtraData>(obj));
	else
		node = std::make_unique<StringData>(std::make_unique<nif::NiStringExtraData>(obj));

	return std::move(node);
}

void node::Constructor::EP_process(nif::native::NiPSysModifier* obj, const CtlrList& ctlrs)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			std::unique_ptr<Modifier> node;

			if (obj->GetType().IsDerivedType(Niflib::NiPSysEmitter::TYPE))
				node = process(static_cast<Niflib::NiPSysEmitter*>(obj), ctlrs);
			else if (obj->GetType().IsSameType(Niflib::NiPSysGravityModifier::TYPE))
				node = process(static_cast<Niflib::NiPSysGravityModifier*>(obj), ctlrs);
			else if (obj->GetType().IsSameType(Niflib::NiPSysRotationModifier::TYPE))
				node = process(static_cast<Niflib::NiPSysRotationModifier*>(obj), ctlrs);
			else if (obj->GetType().IsSameType(Niflib::BSPSysScaleModifier::TYPE))
				node = std::make_unique<ScaleModifier>(
					std::make_unique<nif::BSPSysScaleModifier>(
						static_cast<Niflib::BSPSysScaleModifier*>(obj)));
			else if (obj->GetType().IsSameType(Niflib::BSPSysSimpleColorModifier::TYPE))
				node = std::make_unique<SimpleColourModifier>(
					std::make_unique<nif::BSPSysSimpleColorModifier>(
						static_cast<Niflib::BSPSysSimpleColorModifier*>(obj)));

			if (!node) {
				node = std::make_unique<DummyModifier>(std::make_unique<nif::NiPSysModifier>(obj));
				//Add controllers
				for (auto&& c : ctlrs) {
					if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
						ctlr && ctlr->GetModifierName() == obj->GetName())
					{
						node->addUnknownController(std::make_unique<nif::NiPSysModifierCtlr>(ctlr));
					}
				}
			}

			m_objectMap.insert({ obj, m_nodes.size() });
			m_nodes.push_back(std::move(node));
		}
		else {
			//File error: modifier used by multiple particle systems
			m_warnings.push_back(obj->GetTarget() ? obj->GetTarget()->GetName() : std::string() + ":" +
				obj->GetName() + " is referenced by multiple particle systems");
		}
	}
}

std::unique_ptr<node::Emitter> node::Constructor::process(nif::native::NiPSysEmitter* obj, const CtlrList& ctlrs)
{
	using namespace Niflib;

	assert(obj);

	std::unique_ptr<Emitter> node;
	std::unique_ptr<nif::NiPSysEmitterCtlr> emitterCtlr;
	std::unique_ptr<nif::NiFloatInterpolator> brIplr;
	std::unique_ptr<nif::NiBoolInterpolator> visIplr;

	for (auto&& c : ctlrs) {
		if (NiPSysEmitterCtlr* ctlr = DynamicCast<NiPSysEmitterCtlr>(c);
			ctlr && ctlr->GetModifierName() == obj->GetName())
		{
			emitterCtlr = std::make_unique<nif::NiPSysEmitterCtlr>(static_cast<NiPSysEmitterCtlr*>(ctlr));
			//Do we care to look for duplicates? Probably not, right?
			break;
		}
	}

	if (emitterCtlr) {
		if (NiInterpolator* iplr = emitterCtlr->getNative().GetInterpolator()) {
			if (NiFloatInterpolator* f_iplr = DynamicCast<NiFloatInterpolator>(iplr); 
				f_iplr && f_iplr->GetData() == nullptr) 
			{
				brIplr = std::make_unique<nif::NiFloatInterpolator>(f_iplr);
			}
			else {
				//This iplr should be processed as a separate node
				EP_process(iplr);

				Connection c;
				c.object1 = obj;
				c.object2 = iplr;
				c.field1 = Emitter::BIRTH_RATE;
				//c.field2 = Interpolator::TARGET, or something like that
				m_connections.push_back(c);
			}
		}
		if (NiInterpolator* iplr = emitterCtlr->getNative().GetVisibilityInterpolator()) {
			if (NiBoolInterpolator* b_iplr = DynamicCast<NiBoolInterpolator>(iplr);
				b_iplr && (b_iplr->GetData() == nullptr || b_iplr->GetData()->GetKeys().size() < 3))
			{
				visIplr = std::make_unique<nif::NiBoolInterpolator>(b_iplr);
			}
			else {
				//This iplr should be processed as a separate node
				EP_process(iplr);
			}
		}
	}

	if (obj->GetType().IsDerivedType(Niflib::NiPSysVolumeEmitter::TYPE)) {

		Niflib::NiPSysVolumeEmitter* ve = static_cast<Niflib::NiPSysVolumeEmitter*>(obj);
		if (Niflib::NiNode* emitterObj = ve->GetEmitterObject()) {
			Connection c;
			c.object1 = obj;
			c.object2 = emitterObj;
			c.field1 = VolumeEmitter::EMITTER_OBJECT;
			c.field2 = Node::OBJECT;
			m_connections.push_back(c);
		}

		if (obj->GetType().IsSameType(Niflib::NiPSysBoxEmitter::TYPE))
			node = std::make_unique<BoxEmitter>(
				std::make_unique<nif::NiPSysBoxEmitter>(static_cast<Niflib::NiPSysBoxEmitter*>(obj)),
				std::move(emitterCtlr),
				std::move(brIplr),
				std::move(visIplr));
		else if (obj->GetType().IsSameType(Niflib::NiPSysCylinderEmitter::TYPE))
			node = std::make_unique<CylinderEmitter>(
				std::make_unique<nif::NiPSysCylinderEmitter>(static_cast<Niflib::NiPSysCylinderEmitter*>(obj)),
				std::move(emitterCtlr),
				std::move(brIplr),
				std::move(visIplr));
		else if (obj->GetType().IsSameType(Niflib::NiPSysSphereEmitter::TYPE))
			node = std::make_unique<SphereEmitter>(
				std::make_unique<nif::NiPSysSphereEmitter>(static_cast<Niflib::NiPSysSphereEmitter*>(obj)),
				std::move(emitterCtlr),
				std::move(brIplr),
				std::move(visIplr));
	}

	if (node) {
		//Look for property controllers
		for (auto&& c : ctlrs) {
			if (c && !c->IsDerivedType(NiPSysEmitterCtlr::TYPE))
				if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
					ctlr && ctlr->GetModifierName() == obj->GetName())
				{
					//node->addLifeSpanController... e.g.
					node->addUnknownController(std::make_unique<nif::NiPSysModifierCtlr>(ctlr));
				}
		}
	}

	return std::move(node);
}

std::unique_ptr<node::GravityModifier> node::Constructor::process(nif::native::NiPSysGravityModifier* obj, const CtlrList& ctlrs)
{
	assert(obj);

	std::unique_ptr<GravityModifier> node;

	if (obj->GetForceType() == Niflib::FORCE_PLANAR)
		node = std::make_unique<PlanarForceField>(std::make_unique<nif::NiPSysGravityModifier>(obj));
	else if (obj->GetForceType() == Niflib::FORCE_SPHERICAL)
		node = std::make_unique<SphericalForceField>(std::make_unique<nif::NiPSysGravityModifier>(obj));

	if (node) {
		//Connect to our gravity object
		if (obj->GetGravityObject()) {
			Connection c;
			c.object1 = obj;
			c.object2 = obj->GetGravityObject();
			c.field1 = GravityModifier::GRAVITY_OBJECT;
			c.field2 = Node::OBJECT;
			m_connections.push_back(c);
		}

		//Look for property controllers
		for (auto&& c : ctlrs) {
			if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
				ctlr && ctlr->GetModifierName() == obj->GetName())
			{
				node->addUnknownController(std::make_unique<nif::NiPSysModifierCtlr>(ctlr));
			}
		}
	}

	return std::move(node);
}

std::unique_ptr<node::RotationModifier> node::Constructor::process(nif::native::NiPSysRotationModifier* obj, const CtlrList& ctlrs)
{
	assert(obj);

	auto node = std::make_unique<RotationModifier>(std::make_unique<nif::NiPSysRotationModifier>(obj));
	//Look for property controllers
	for (auto&& c : ctlrs) {
		if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
			ctlr && ctlr->GetModifierName() == obj->GetName())
		{
			node->addUnknownController(std::make_unique<nif::NiPSysModifierCtlr>(ctlr));
		}
	}

	return std::move(node);
}

void node::Constructor::EP_process(nif::native::BSEffectShaderProperty* obj)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			auto node = std::make_unique<EffectShader>(std::make_unique<nif::BSEffectShaderProperty>(obj));

			m_objectMap.insert({ obj, m_nodes.size() });
			m_nodes.push_back(std::move(node));
		}
	}
}
