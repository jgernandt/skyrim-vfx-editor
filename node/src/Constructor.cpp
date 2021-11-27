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
#include "Positioner.h"

using namespace nif;

void node::Constructor::extractNodes(gui::ConnectionHandler& target, bool arrange)
{
	//Translate modifier connection requests into actual ConnectionInfo
	struct Compare
	{
		bool operator() (NiPSysModifier* lhs, NiPSysModifier* rhs)
		{
			assert(lhs && rhs);
			return lhs->order.get() < rhs->order.get();
		}
	};
	for (auto&& entry : m_modConnections) {
		//The particle system should always have a node
		assert(m_objectMap.find(entry.first) != m_objectMap.end() && m_objectMap.find(entry.first)->second >= 0);

		//Sort the list by mod order (should be already)
		std::sort(entry.second.begin(), entry.second.end(), Compare{});

		//for each modifier with a node, register a connection to the previous one
		NiObject* prev = entry.first;
		for (auto next = entry.second.begin(); next < entry.second.end(); ++next) {
			if (auto it = m_objectMap.find(*next); it != m_objectMap.end()) {
				if (it->second >= 0) {
					//This mod has a node
					ConnectionInfo info;
					info.object1 = prev;
					info.field1 = prev == entry.first ? ParticleSystem::MODIFIERS : Modifier::NEXT_MODIFIER;
					info.object2 = *next;
					info.field2 = Modifier::TARGET;
					addConnection(info);

					prev = *next;
				}
			}
		}
	}

	std::vector<std::pair<gui::Connector*, gui::Connector*>> couplings;
	std::vector<Positioner::LinkInfo> linkInfo;

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
				if (i1 > i2) {
					//the positioner benefits from consistency
					std::swap(i1, i2);
					std::swap(c1, c2);
				}

				linkInfo.push_back({});
				linkInfo.back().node1 = i1;
				linkInfo.back().node2 = i2;

				//Offset should be (node-space) translation(c1) - translation(c2).
				//Connectors don't know their position until we start drawing them,
				//but down the line we want to delegate to some layout manager to place gui components.
				//This solution would mean that the connectors *do* know their position at this point.
				//So let's make this work somehow:
				linkInfo.back().offset = c2->getTranslation() - c1->getTranslation();
				//(later, we may not be able to assume that the local translation is in node space)

				//The stiffness may have to be determined pairwise. 
				//It may also have to depend on the number of links to the same connector, or to the same node.
				//Unless we make the graph data accessible somehow, this will be hard to work with. Something to consider.
				//Right now I think the only ones we want softer are the upwards references.
				if (item.field1 == Node::OBJECT || item.field2 == Node::OBJECT)
					linkInfo.back().stiffness = 0.1f;
				else
					linkInfo.back().stiffness = 1.0f;
			}
			//if somehow i1 == i2 we ignore it
		}
	}
	m_connections.clear();

	if (m_nodes.size() > 1) {
		if (arrange)
			target.addChild(std::make_unique<Positioner>(std::move(m_nodes), std::move(linkInfo)));
		else {
			for (auto&& node : m_nodes)
				target.addChild(std::move(node));
			m_nodes.clear();
		}
	}
	else if (m_nodes.size() == 1)
		target.addChild(std::move(m_nodes.front()));

	for (auto&& pair : couplings) {
		pair.first->setConnectionState(pair.second, true);
		pair.second->setConnectionState(pair.first, true);
	}
}

void node::Constructor::addConnection(const node::ConnectionInfo& info)
{
	m_connections.push_back(info);
}

void node::Constructor::addModConnections(NiParticleSystem* target, std::vector<NiPSysModifier*>&& mods)
{
	if (target)
		m_modConnections[target] = std::move(mods);
}

void node::Constructor::addNode(NiObject* obj, std::unique_ptr<NodeBase>&& node)
{
	assert(obj && node);
	m_objectMap.insert({ obj, m_nodes.size() });
	m_nodes.push_back(std::move(node));
}

ni_ptr<NiObject> node::Constructor::getObject() const
{
	return !m_objectStack.empty() ? m_objectStack.back() : ni_ptr<NiObject>();
}


/*
void node::Constructor::makeRoot()
{
	Niflib::NiNode* root = nullptr;
	if (auto rootPtr = m_file.getRoot())
		root = &rootPtr->getNative();

	if (!root) {
		throw std::runtime_error("File has no root");
	}
	else {
		//Leave room for the root node (maybe vanity, but I'd prefer it to be first)
		m_nodes.resize(1);
		EP_process(static_cast<Niflib::NiAVObject*>(root));
		for (auto&& ctlr : static_cast<Niflib::NiObjectNET*>(root)->GetControllers()) {
			if (Niflib::DynamicCast<Niflib::NiControllerManager>(ctlr)) {
				m_warnings.push_back("This file contains behaviour-controlled animations. This functionality is not yet supported, and any edit to this file may break it.");
				break;
			}
		}
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
				node = std::make_unique<DummyAVObject>(m_file.get<nif::NiAVObject>(obj));
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
		node = std::make_unique<Root>(m_file.get<nif::NiNode>(obj));
	else
		node = std::make_unique<Node>(m_file.get<nif::NiNode>(obj));

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
	std::shared_ptr<nif::NiParticleSystem> psys = m_file.get<nif::NiParticleSystem>(obj);
	std::shared_ptr<nif::NiPSysUpdateCtlr> ctlr;
	std::shared_ptr<nif::NiPSysData> data;
	std::shared_ptr<nif::NiAlphaProperty> alpha;
	std::shared_ptr<nif::NiPSysAgeDeathModifier> adm;
	std::shared_ptr<nif::NiPSysPositionModifier> pm;
	std::shared_ptr<nif::NiPSysBoundUpdateModifier> bum;

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
		data = m_file.get<nif::NiPSysData>(d);
	}
	else
		incomplete = true;

	if (NiAlphaProperty* a = obj->GetAlphaProperty()) {
		if (auto result = m_objectMap.insert({ a, -1 }); !result.second) {
			//This is not an error, but will be a problem for us since we're not exposing this alpha property. We should clone it.
			// 
			//NiAlphaPropertyRef clone = StaticCast<NiAlphaProperty>(a->Clone(s_version, s_userVersion));
			//Clone is broken. Doesn't provide a header for indexed string i/o. We'll do it manually:
			alpha = m_file.create<nif::NiAlphaProperty>();
			alpha->getNative().SetFlags(a->GetFlags());
			alpha->getNative().SetTestThreshold(a->GetTestThreshold());
			//If, by any chance, there is extra data, we can just add it:
			for (auto&& ed : a->GetExtraData())
				alpha->getNative().AddExtraData(ed);
			//Controllers, however, would have to be cloned as well. 
			//Controlled alpha properties are rare, so let's just leave that for another happy day.
			if (a->IsAnimated())
				m_warnings.push_back(obj->GetName() + " is sharing a controlled alpha property with another object");

			obj->SetAlphaProperty(&alpha->getNative());
		}
		else
			alpha = m_file.get<nif::NiAlphaProperty>(a);
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
				ctlr = m_file.get<nif::NiPSysUpdateCtlr>(u);
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
				adm = m_file.get<nif::NiPSysAgeDeathModifier>(derived);
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
				pm = m_file.get<nif::NiPSysPositionModifier>(derived);
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
				bum = m_file.get<nif::NiPSysBoundUpdateModifier>(derived);
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

	node = std::make_unique<ParticleSystem>(m_file, std::move(psys), std::move(data), std::move(alpha), std::move(ctlr), std::move(adm), std::move(pm), std::move(bum));

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
				node = std::make_unique<DummyExtraData>(m_file.get<nif::NiExtraData>(obj));

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
		node = std::make_unique<WeaponTypeData>(m_file.get<nif::NiStringExtraData>(obj));
	else
		node = std::make_unique<StringData>(m_file.get<nif::NiStringExtraData>(obj));

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
					m_file.get<nif::BSPSysScaleModifier>(
						static_cast<Niflib::BSPSysScaleModifier*>(obj)));
			else if (obj->GetType().IsSameType(Niflib::BSPSysSimpleColorModifier::TYPE))
				node = std::make_unique<SimpleColourModifier>(
					m_file.get<nif::BSPSysSimpleColorModifier>(
						static_cast<Niflib::BSPSysSimpleColorModifier*>(obj)));

			if (!node) {
				node = std::make_unique<DummyModifier>(m_file.get<nif::NiPSysModifier>(obj));
				//Add controllers
				for (auto&& c : ctlrs) {
					if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
						ctlr && ctlr->GetModifierName() == obj->GetName())
					{
						node->addUnknownController(m_file.get<nif::NiPSysModifierCtlr>(ctlr));
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
	std::shared_ptr<nif::NiPSysEmitterCtlr> emitterCtlr;
	std::shared_ptr<nif::NiFloatInterpolator> brIplr;
	std::shared_ptr<nif::NiBoolInterpolator> visIplr;

	for (auto&& c : ctlrs) {
		if (NiPSysEmitterCtlr* ctlr = DynamicCast<NiPSysEmitterCtlr>(c);
			ctlr && ctlr->GetModifierName() == obj->GetName())
		{
			emitterCtlr = m_file.get<nif::NiPSysEmitterCtlr>(static_cast<NiPSysEmitterCtlr*>(ctlr));
			//Do we care to look for duplicates? Probably not, right?
			break;
		}
	}

	if (emitterCtlr) {
		if (NiInterpolator* iplr = emitterCtlr->getNative().GetInterpolator()) {
			if (NiFloatInterpolator* f_iplr = DynamicCast<NiFloatInterpolator>(iplr); 
				f_iplr && f_iplr->GetData() == nullptr) 
			{
				brIplr = m_file.get<nif::NiFloatInterpolator>(f_iplr);
			}
			else {
				//This iplr should be processed as a separate node
				EP_process(iplr, *emitterCtlr);

				Connection c;
				c.object1 = obj;
				c.object2 = iplr;
				c.field1 = Emitter::BIRTH_RATE;
				c.field2 = FloatController::TARGET;
				m_connections.push_back(c);
			}
		}
		if (NiInterpolator* iplr = emitterCtlr->getNative().GetVisibilityInterpolator()) {
			if (NiBoolInterpolator* b_iplr = DynamicCast<NiBoolInterpolator>(iplr);
				b_iplr && (b_iplr->GetData() == nullptr || b_iplr->GetData()->GetKeys().size() < 3))
			{
				visIplr = m_file.get<nif::NiBoolInterpolator>(b_iplr);
			}
			else {
				//This iplr should be processed as a separate node
				EP_process(iplr, *emitterCtlr);
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
			node = std::make_unique<BoxEmitter>(m_file,
				m_file.get<nif::NiPSysBoxEmitter>(static_cast<Niflib::NiPSysBoxEmitter*>(obj)),
				std::move(emitterCtlr),
				std::move(brIplr),
				std::move(visIplr));
		else if (obj->GetType().IsSameType(Niflib::NiPSysCylinderEmitter::TYPE))
			node = std::make_unique<CylinderEmitter>(m_file,
				m_file.get<nif::NiPSysCylinderEmitter>(static_cast<Niflib::NiPSysCylinderEmitter*>(obj)),
				std::move(emitterCtlr),
				std::move(brIplr),
				std::move(visIplr));
		else if (obj->GetType().IsSameType(Niflib::NiPSysSphereEmitter::TYPE))
			node = std::make_unique<SphereEmitter>(m_file,
				m_file.get<nif::NiPSysSphereEmitter>(static_cast<Niflib::NiPSysSphereEmitter*>(obj)),
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
					node->addUnknownController(m_file.get<nif::NiPSysModifierCtlr>(ctlr));
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
		node = std::make_unique<PlanarForceField>(m_file.get<nif::NiPSysGravityModifier>(obj));
	else if (obj->GetForceType() == Niflib::FORCE_SPHERICAL)
		node = std::make_unique<SphericalForceField>(m_file.get<nif::NiPSysGravityModifier>(obj));

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
				node->addUnknownController(m_file.get<nif::NiPSysModifierCtlr>(ctlr));
			}
		}
	}

	return std::move(node);
}

std::unique_ptr<node::RotationModifier> node::Constructor::process(nif::native::NiPSysRotationModifier* obj, const CtlrList& ctlrs)
{
	assert(obj);

	auto node = std::make_unique<RotationModifier>(m_file.get<nif::NiPSysRotationModifier>(obj));
	//Look for property controllers
	for (auto&& c : ctlrs) {
		if (Niflib::NiPSysModifierCtlr* ctlr = Niflib::DynamicCast<Niflib::NiPSysModifierCtlr>(c);
			ctlr && ctlr->GetModifierName() == obj->GetName())
		{
			node->addUnknownController(m_file.get<nif::NiPSysModifierCtlr>(ctlr));
		}
	}

	return std::move(node);
}

void node::Constructor::EP_process(nif::native::BSEffectShaderProperty* obj)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			auto node = std::make_unique<EffectShader>(m_file.get<nif::BSEffectShaderProperty>(obj));

			m_objectMap.insert({ obj, m_nodes.size() });
			m_nodes.push_back(std::move(node));
		}
	}
}

void node::Constructor::EP_process(nif::native::NiInterpolator* obj, const nif::NiTimeController& ctlr)
{
	if (obj) {
		if (auto it = m_objectMap.find(obj); it == m_objectMap.end()) {
			std::unique_ptr<NodeBase> node;

			if (obj->GetType().IsDerivedType(Niflib::NiBoolInterpolator::TYPE)) {

			}
			else if (obj->GetType().IsSameType(Niflib::NiFloatInterpolator::TYPE)) {
				Niflib::NiFloatInterpolator* fiplr = Niflib::StaticCast<Niflib::NiFloatInterpolator>(obj);
				std::shared_ptr<nif::NiFloatData> data;
				if (Niflib::NiFloatData* d = fiplr->GetData())
					data = m_file.get<nif::NiFloatData>(d);
				auto ctlr_node = std::make_unique<FloatController>(
					m_file, m_file.get<nif::NiFloatInterpolator>(fiplr), std::move(data), &ctlr);
				node = std::move(ctlr_node);
			}
			else if (obj->GetType().IsSameType(Niflib::NiBlendFloatInterpolator::TYPE)) {

			}

			if (node) {
				m_objectMap.insert({ obj, m_nodes.size() });
				m_nodes.push_back(std::move(node));
			}
		}
		else {
			//interpolator used by multiple controllers?
			//I'm not sure if this is an error, but we're not supporting it right now. We should warn.
			auto target = ctlr.getNative().GetTarget();
			if (target)
				m_warnings.push_back(target->GetName() + " is sharing an interpolator with another object. This cannot be displayed properly.");
		}
	}
}
*/
