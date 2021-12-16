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
#include "AnimationManager.h"
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

	m_root = newField<Root>(Root::ID, *this);

	newChild<gui::Text>("Behaviour file");
	newChild<StringInput>(make_ni_ptr(m_bged, &BSBehaviorGraphExtraData::fileName));

	//until we have some other way to determine connector position for loading placement
	m_root->connector->setTranslation({ 0.0f, 38.0f });
}

node::ControllerManager::~ControllerManager()
{
	disconnect();
}

void node::ControllerManager::setAnimationManager(AnimationManager& am)
{
	assert(!m_animationManager);

	m_animationManager = &am;
	m_manager->objectPalette.assigned()->objects.clear();

	am.objects().addListener(*this);
	for (int i = 0; i < am.objects().size(); i++)
		onInsert(i);
}

void node::ControllerManager::onInsert(int pos)
{
	auto&& pal = m_manager->objectPalette.assigned();
	assert(pal->objects.size() >= (size_t)pos);
	pal->objects.insert(pos);
	pal->objects.at(pos).assign(m_animationManager->objects().at(pos));
}

void node::ControllerManager::onErase(int pos)
{
	auto&& pal = m_manager->objectPalette.assigned();
	assert(pal->objects.size() > (size_t)pos);
	pal->objects.erase(pos);
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
