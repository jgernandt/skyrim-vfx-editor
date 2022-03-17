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
	//Execute post-process functions
	for (auto&& fcn : m_postProcess)
		if (fcn)
			fcn();

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
	node->setAnimationManager(m_animationManager);
	m_objectMap.insert({ obj, m_nodes.size() });
	m_nodes.push_back(std::move(node));
}

ni_ptr<NiObject> node::Constructor::getObject() const
{
	return !m_objectStack.empty() ? m_objectStack.back() : ni_ptr<NiObject>();
}

void node::Constructor::mapController(NiInterpolator* iplr, NiTimeController* ctlr)
{
	m_controllerMap.insert({ iplr, ctlr });
}

NiTimeController* node::Constructor::getController(NiInterpolator* iplr) const
{
	if (auto it = m_controllerMap.find(iplr); it != m_controllerMap.end())
		return it->second;
	else
		return nullptr;
}
