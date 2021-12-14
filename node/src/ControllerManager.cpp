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
#include "ControllerManager.h"
#include "widget_types.h"
#include "style.h"

node::ControllerManager::ControllerManager(
	const ni_ptr<NiControllerManager>& manager, const ni_ptr<BSBehaviorGraphExtraData>& bged) :
	m_manager{ manager }, m_bged{ bged }
{
	setClosable(true);
	setTitle("Behaviour");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	auto&& pal = m_manager->objectPalette.assigned();
	assert(m_manager && m_bged && pal != nullptr);//note: this is assumed throughout this file
	pal->objects.addListener(*this);
	//need to initialise listeners for any preexisting objects in the palette
	int i = 0;
	for (auto&& av : pal->objects) {
		onInsert(i);
		assert(m_lsnrs.size() == i + 1);
		m_lsnrs[i++]->onAssign(av.object.assigned().get());
	}

	m_root = newField<Root>(Root::ID, *this);

	newChild<gui::Text>("Behaviour file");
	newChild<StringInput>(make_ni_ptr(m_bged, &BSBehaviorGraphExtraData::fileName));

	//until we have some other way to determine connector position for loading placement
	m_root->connector->setTranslation({ 0.0f, 38.0f });
}

node::ControllerManager::~ControllerManager()
{
	disconnect();
	//unregister any remaining listeners
	auto&& obj_list = m_manager->objectPalette.assigned()->objects;
	assert(obj_list.size() == m_lsnrs.size());
	int i = 0;
	for (auto&& av : obj_list) {
		av.object.removeListener(*m_lsnrs[i++]);
	}
	//and ourselves
	obj_list.removeListener(*this);
}

void node::ControllerManager::onInsert(int pos)
{
	assert((size_t)pos <= m_lsnrs.size());
	auto&& av = m_manager->objectPalette.assigned()->objects.at(pos);
	auto it = m_lsnrs.insert(m_lsnrs.begin() + pos, std::make_unique<NameSyncer>(av));
	//register listener
	av.object.addListener(**it);
}

void node::ControllerManager::onErase(int pos)
{
	assert((size_t)pos < m_lsnrs.size());
	//don't unregister; target has been destroyed
	m_lsnrs.erase(m_lsnrs.begin() + pos);
}

node::ControllerManager::NameSyncer::NameSyncer(AVObject& av) : m_av{ av }
{
}

node::ControllerManager::NameSyncer::~NameSyncer()
{
	if (auto prev = m_source.lock())
		prev->removeListener(*this);
}

void node::ControllerManager::NameSyncer::onAssign(NiAVObject* obj)
{
	//unregister from previous
	if (auto prev = m_source.lock()) {
		prev->removeListener(*this);
		prev.reset();
	}

	if (obj) {
		//register to new
		if (auto current = m_av.object.assigned(); obj == current.get()) {//should be certain
			m_source = make_ni_ptr(std::static_pointer_cast<NiObjectNET>(current), &NiObjectNET::name);
			current->name.addListener(*this);
			onSet(current->name.get());
		}
	}
}

void node::ControllerManager::NameSyncer::onSet(const std::string& name)
{
	m_av.name.set(name);
}

node::ControllerManager::Root::Root(const std::string& name, ControllerManager& node) :
	Field{ name }, m_node{ node }, m_rcvr{ *this }, m_sndr{ node.m_manager->objectPalette.assigned()->scene }
{
	//Don't bother adding the root to the palette, looks like it's not automatically needed
	//m_node.m_manager->objectPalette.assigned()->scene.addListener(*this);
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
}

node::ControllerManager::Root::~Root()
{
	//m_node.m_manager->objectPalette.assigned()->scene.removeListener(*this);
}

void node::ControllerManager::Root::onAssign(NiAVObject* obj)
{
	auto&& objPalette = m_node.m_manager->objectPalette.assigned();
	assert(obj == objPalette->scene.assigned().get());
	if (obj) {
		//Add an entry for obj, if it is not in the list already
		bool found = false;
		for (auto&& item : objPalette->objects) {
			if (item.object.assigned().get() == obj) {
				found = true;
				break;
			}
		}
		if (!found) {
			objPalette->objects.push_front();
			objPalette->objects.front().object.assign(objPalette->scene.assigned());
		}
	}
	else {
		//Are we sure the scene root will always be in the front?
		//No. We need to search for m_node.m_manager->target.assigned(). Which may have been unassigned already...
		//We could just leave it. It won't do anything now that we have been disconnected.
		//objPalette->objects.pop_front();
	}
}

node::ControllerManager::Root::Rcvr::Rcvr(Root& root) : m_root{ root }
{
	m_root.m_node.m_bged->fileName.addListener(*this);
}

node::ControllerManager::Root::Rcvr::~Rcvr()
{
	m_root.m_node.m_bged->fileName.removeListener(*this);
}

void node::ControllerManager::Root::Rcvr::onConnect(IControllableRoot& ifc)
{
	m_ifc = &ifc;
	ifc.addController(m_root.m_node.m_manager);
	if (!m_root.m_node.m_bged->fileName.get().empty())
		ifc.extraData().add(m_root.m_node.m_bged);
}

void node::ControllerManager::Root::Rcvr::onDisconnect(IControllableRoot& ifc)
{
	ifc.extraData().remove(m_root.m_node.m_bged.get());
	ifc.removeController(m_root.m_node.m_manager.get());
	m_ifc = nullptr;
}

void node::ControllerManager::Root::Rcvr::onSet(const std::string& file)
{
	if (m_ifc) {
		if (file.empty())
			m_ifc->extraData().remove(m_root.m_node.m_bged.get());
		else
			m_ifc->extraData().add(m_root.m_node.m_bged);
	}
}


node::ControllerSequence::ControllerSequence(const ni_ptr<NiControllerSequence>& obj)
{
	assert(obj);
}

node::ControllerSequence::~ControllerSequence()
{
	disconnect();
}
