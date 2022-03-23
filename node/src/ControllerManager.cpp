//Copyright 2021, 2022 Jonas Gernandt
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
#include "ActionEditor.h"
#include "AnimationManager.h"
#include "CompositionActions.h"
#include "ControllerManager.h"
#include "nodes_internal.h"//InterpolatorFactory
#include "style.h"
#include "widget_types.h"

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

	m_actions = newField<Actions>(Actions::ID, *this);
	m_root = newField<Root>(Root::ID, *this);

	newChild<gui::Text>("Behaviour file");
	newChild<StringInput>(make_ni_ptr(m_bged, &BSBehaviorGraphExtraData::fileName));

	//until we have some other way to determine connector position for loading placement
	m_actions->connector->setTranslation({ WIDTH, 38.0f });
	m_root->connector->setTranslation({ 0.0f, 62.0f });
}

node::ControllerManager::~ControllerManager()
{
	disconnect();
}

void node::ControllerManager::setAnimationManager(const std::shared_ptr<AnimationManager>& am)
{
	assert(!m_animationManager.lock());

	m_animationManager = am;
	m_manager->objectPalette.assigned()->objects.clear();

	am->objects().addListener(*this);
	for (int i = 0; i < am->objects().size(); i++)
		onInsert(i);
}

void node::ControllerManager::onInsert(int pos)
{
	auto animationManager = m_animationManager.lock();
	assert(animationManager);//they called us

	auto&& pal = m_manager->objectPalette.assigned();
	assert(pal->objects.size() >= (size_t)pos);
	pal->objects.insert(pos);
	pal->objects.at(pos).assign(animationManager->objects().at(pos));
}

void node::ControllerManager::onErase(int pos)
{
	auto&& pal = m_manager->objectPalette.assigned();
	assert(pal->objects.size() > (size_t)pos);
	pal->objects.erase(pos);
}

node::ControllerManager::Actions::Actions(const std::string& name, ControllerManager& node) :
	Field{ name }, m_rcvr{ node.m_manager }, m_sndr{ node.m_manager->ctlrSequences }
{
	connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sndr, m_rcvr));
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


node::ControllerSequence::ControllerSequence(File& file, const ni_ptr<NiControllerSequence>& obj) :
	m_file{ file }, m_obj{ obj }
{
	assert(m_obj && m_obj->textKeys.assigned());

	setClosable(true);
	setTitle("Action");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	m_obj->startTime.addListener(*this);
	m_obj->stopTime.addListener(*this);
	onSet(0.0f);

	//We listen to our manager field. When that is assigned to, we start listening to
	//their target field. When THAT is assigned to, we start listening to its name.
	m_obj->manager.addListener(*this);
	onAssign(m_obj->manager.assigned().get());

	m_behaviour = newField<Behaviour>(Behaviour::ID, *this);

	newChild<gui::Text>("Name");
	newChild<StringInput>(make_ni_ptr(m_obj, &NiControllerSequence::name));

	newChild<gui::VerticalSpacing>();

	auto button = newChild<gui::Button>("Edit", std::bind(&ControllerSequence::openActionEditor, this));
	button->setSize({ -1.0f, 0.0f });

	//until we have some other way to determine connector position for loading placement
	m_behaviour->connector->setTranslation({ 0.0f, 38.0f });
}

node::ControllerSequence::~ControllerSequence()
{
	disconnect();

	if (auto am = m_animationManager.lock()) {
		assert(am->blocks().size() == m_blockLsnrs.size());
		size_t i = 0;
		for (auto&& block : am->blocks()) {
			block.nodeName.removeListener(m_blockLsnrs[i]->nodeName);
			block.propertyType.removeListener(m_blockLsnrs[i]->propertyType);
			block.ctlrType.removeListener(m_blockLsnrs[i]->ctlrType);
			block.ctlrID.removeListener(m_blockLsnrs[i]->ctlrID);
			block.iplrID.removeListener(m_blockLsnrs[i]->iplrID);
			i++;
		}
		am->blocks().removeListener(*this);
		m_animationManager.reset();
	}

	//should be a consequence of disconnecting, but something might have gone wrong 
	//to prevent us from connecting in the first place
	if (m_obj->manager.assigned())
		m_obj->manager.assign(nullptr);
	assert(!m_managerTargetPtr.lock() && !m_accumRootNameProperty.lock());

	m_obj->manager.removeListener(*this);

	m_obj->startTime.removeListener(*this);
	m_obj->stopTime.removeListener(*this);
}

void node::ControllerSequence::setAnimationManager(const std::shared_ptr<AnimationManager>& am)
{
	if (am) {
		for (int i = m_obj->blocks.size(); i != 0;)
			onErase(--i);

		m_animationManager = am;

		for (int i = 0; (size_t)i < am->blocks().size(); i++)
			onInsert(i);

		am->blocks().addListener(*this);
	}
}

void node::ControllerSequence::onInsert(int pos)
{
	auto am = m_animationManager.lock();
	assert(am);//or we would not receive calls
	assert((size_t)pos <= m_obj->blocks.size());

	m_obj->blocks.insert(pos);
	ControlledBlock& our_block = m_obj->blocks.at(pos);
	AnimationManager::Block& their_block = am->blocks().at(pos);

	//use cached iplr if there is one
	size_t str_hash = std::hash<std::string>{}(
		their_block.nodeName.get() +
		their_block.propertyType.get() +
		their_block.ctlrType.get() +
		their_block.ctlrID.get() +
		their_block.iplrID.get());
	if (auto it = m_iplrCache.find({ their_block.controller.get(), str_hash }); it != m_iplrCache.end()) {
		our_block.interpolator.assign(it->second);
		m_iplrCache.erase(it);
	}
	//else let factory create one
	else {
		assert(their_block.controller);
		IplrFactoryVisitor v(m_file, their_block.iplrID.get());
		their_block.controller->receive(v);
		our_block.interpolator.assign(v.iplr);
	}

	our_block.controller.assign(their_block.controller);
	our_block.nodeName.set(their_block.nodeName.get());
	our_block.propertyType.set(their_block.propertyType.get());
	our_block.ctlrType.set(their_block.ctlrType.get());
	our_block.ctlrID.set(their_block.ctlrID.get());
	our_block.iplrID.set(their_block.iplrID.get());

	//setup listeners
	assert((size_t)pos <= m_blockLsnrs.size());
	auto it = m_blockLsnrs.insert(m_blockLsnrs.begin() + pos, std::make_unique<BlockListeners>(our_block));
	their_block.nodeName.addListener((*it)->nodeName);
	their_block.propertyType.addListener((*it)->propertyType);
	their_block.ctlrType.addListener((*it)->ctlrType);
	their_block.ctlrID.addListener((*it)->ctlrID);
	their_block.iplrID.addListener((*it)->iplrID);
}

void node::ControllerSequence::onErase(int pos)
{
	//we interpret pos as position in our m_obj->blocks, not in m_blockList
	//(those will be equal when setAnimationManager finishes)
	assert((size_t)pos < m_obj->blocks.size());
	ControlledBlock& this_block = m_obj->blocks.at(pos);

	//we need to cache our interpolator in case this erase is undone
	size_t str_hash = std::hash<std::string>{}(
		this_block.nodeName.get() + 
		this_block.propertyType.get() + 
		this_block.ctlrType.get() + 
		this_block.ctlrID.get() + 
		this_block.iplrID.get());
	m_iplrCache[{ this_block.controller.assigned().get(), str_hash }] = this_block.interpolator.assigned();

	m_obj->blocks.erase(pos);

	if (!m_blockLsnrs.empty()) {//will be empty during setAnimationManager
		assert((size_t)pos < m_blockLsnrs.size());//otherwise, should be synced with said manager
		//These are dangling listeners
		m_blockLsnrs.erase(m_blockLsnrs.begin() + pos);
	}
}

//Would be a lot nicer if we had onAssign/onUnassign events that passed ni_ptrs.
//Is that a good idea in general?
void node::ControllerSequence::onAssign(NiControllerManager* obj)
{
	if (auto current = m_managerTargetPtr.lock()) {
		current->removeListener(*this);
		m_managerTargetPtr.reset();
		onAssign((NiObjectNET*)nullptr);
	}

	if (obj) {
		//Get a ni_ptr to the target Ptr via our manager Ptr.
		//Start listening to the target Ptr.
		auto assigned = m_obj->manager.assigned();
		assert(obj == assigned.get());
		auto managerTargetPtr = make_ni_ptr(std::static_pointer_cast<NiTimeController>(assigned), &NiTimeController::target);
		m_managerTargetPtr = managerTargetPtr;
		managerTargetPtr->addListener(*this);
		onAssign(managerTargetPtr->assigned().get());
	}
}

void node::ControllerSequence::onAssign(NiObjectNET* obj)
{
	if (auto current = m_accumRootNameProperty.lock()) {
		current->removeListener(*this);
		m_accumRootNameProperty.reset();
		onSet("");
	}
	if (obj) {
		//get a ni_ptr to the name via our current target Ptr and start listening to it.
		if (auto ptr = m_managerTargetPtr.lock()) {
			auto target = ptr->assigned();
			assert(obj == target.get());
			auto accumRootNameProperty = make_ni_ptr(target, &NiObjectNET::name);
			m_accumRootNameProperty = accumRootNameProperty;
			accumRootNameProperty->addListener(*this);
			onSet(accumRootNameProperty->get());
		}
	}
}

void node::ControllerSequence::onSet(const float&)
{
	//Set both. Slightly ineffecient, but no big deal (too lazy to set up two listeners).
	m_obj->textKeys.assigned()->keys.front().time.set(m_obj->startTime.get());
	m_obj->textKeys.assigned()->keys.back().time.set(m_obj->stopTime.get());
}

void node::ControllerSequence::onSet(const std::string& accumRootName)
{
	m_obj->accumRootName.set(accumRootName);
}

void node::ControllerSequence::openActionEditor()
{
	auto c = std::make_unique<ActionEditor>(m_file, m_obj);
	c->open();
	asyncInvoke<gui::AddChild>(std::move(c), this, false);
}

node::ControllerSequence::Behaviour::Behaviour(const std::string& name, ControllerSequence& node) :
	Field{ name }, m_rcvr{ node.m_obj }, m_sndr{ node.m_obj->manager }
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
}

node::ControllerSequence::StringForwarder::StringForwarder(Property<std::string>& dst) :
	m_dst{ dst }
{
}

void node::ControllerSequence::StringForwarder::onSet(const std::string& s)
{
	m_dst.set(s);
}

node::ControllerSequence::BlockListeners::BlockListeners(ControlledBlock& block) :
	nodeName{ block.nodeName }, propertyType{ block.propertyType }, ctlrType{ block.ctlrType }, 
	ctlrID{ block.ctlrID }, iplrID{ block.iplrID }
{
}
