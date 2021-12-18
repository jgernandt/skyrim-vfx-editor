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

using namespace nif;
using namespace node;

node::AnimationManager::~AnimationManager()
{
	m_blocks.clear();
}

node::AnimationManager::Block* node::AnimationManager::registerBlock(const BlockInfo& info)
{
	Block* result = nullptr;

	if (info.ctlr && info.target) {
		//first check for duplicates (same ctlr, same nodeName, ctlrId and iplrID)
		//when a BlendIplr node is first connected after loading a file, its block will already exist
		for (auto&& block : m_blocks) {
			if (info.ctlr == block.controller && 
				info.target->name.get() == block.nodeName.get() &&
				info.ctlrID == block.ctlrID.get() &&
				info.iplrID == block.iplrID.get())
				return &block;
		}
		//this block is new
		m_blocks.push_back(info.ctlr, info.target->name.get(), info.propertyType, 
			info.ctlrType, info.ctlrIDProperty ? info.ctlrIDProperty->get() : info.ctlrID, info.iplrID);
		result = &m_blocks.back();

		result->targetLsnr.setAnimationManager(*this);

		result->target.assign(info.target);
		//result->controller = info.ctlr;
		//result->propertyType.set(info.propertyType);
		//result->ctlrType.set(info.ctlrType);
		//result->iplrID.set(info.iplrID);

		if (info.ctlrIDProperty) {
			result->ctlrIDProperty = info.ctlrIDProperty;
			info.ctlrIDProperty->addListener(result->ctlrIDSyncer);
			//result->ctlrIDSyncer.onSet(info.ctlrIDProperty->get());
		}
		//else
		//	result->ctlrID.set(info.ctlrID);
	}

	return result;
}

void node::AnimationManager::unregisterBlock(Block* block)
{
	//stupid that List doesn't have proper iterator support
	int i = 0;
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it) {
		if (&(*it) == block)
			break;
		i++;
	}
	if ((size_t)i < m_blocks.size())
		m_blocks.erase(i);
}

void node::AnimationManager::addObject(const ni_ptr<NiAVObject>& obj)
{
	if (obj)
		//Doesn't matter if this succeeds or not. If it doesn't, the file is broken.
		//Worst case scenario is that we lose some animations that were broken anyway.
		m_objMap.insert({ obj->name.get(), obj });
}

ni_ptr<NiAVObject> node::AnimationManager::findObject(const std::string& name) const
{
	if (auto it = m_objMap.find(name); it != m_objMap.end())
		return it->second;
	else
		return ni_ptr<NiAVObject>();
}

void node::AnimationManager::incrCount(const ni_ptr<NiAVObject>& obj)
{
	auto res = m_objCount.insert({ obj.get(), 0 });
	assert(res.first->second >= 0);
	if (res.first->second++ == 0) {
		//first encounter, add to object list
		m_objects.insert(m_objects.size(), obj);
	}
}

void node::AnimationManager::decrCount(NiAVObject* obj)
{
	if (auto it = m_objCount.find(obj); it != m_objCount.end()) {
		assert(it->second >= 0);
		if (--it->second == 0) {
			//last ref, remove from object list
			if (int pos = m_objects.find(obj); pos != -1)
				m_objects.erase(pos);
		}
	}
}


node::AnimationManager::Block::Block(
	const ni_ptr<NiTimeController>& ctlr,
	const std::string& nodeName,
	const std::string& propertyType,
	const std::string& ctlrType,
	const std::string& ctlrID,
	const std::string& iplrID) :
	controller{ ctlr }, nodeName{ nodeName }, propertyType{ propertyType }, 
	ctlrType{ ctlrType }, ctlrID{ ctlrID }, iplrID{ iplrID },
	targetLsnr{ *this }, ctlrIDSyncer{ *this }
{
	target.addListener(targetLsnr);
}

node::AnimationManager::Block::~Block()
{
	target.removeListener(targetLsnr);
	if (ctlrIDProperty)
		ctlrIDProperty->removeListener(ctlrIDSyncer);
}


node::AnimationManager::Block::TargetListener::TargetListener(Block& block) :
	m_block{ block }
{
}

node::AnimationManager::Block::TargetListener::~TargetListener()
{
	if (auto locked = m_currentSrc.lock()) {
		m_animationManager->decrCount(locked.get());
		locked->name.removeListener(*this);
	}
}

void node::AnimationManager::Block::TargetListener::onAssign(NiAVObject* obj)
{
	assert(m_animationManager);

	if (auto locked = m_currentSrc.lock()) {
		m_animationManager->decrCount(locked.get());

		locked->name.removeListener(*this);

		m_currentSrc.reset();
	}

	assert(obj == m_block.target.assigned().get());
	if (auto&& tgt = m_block.target.assigned()) {
		m_currentSrc = tgt;

		tgt->name.addListener(*this);
		onSet(tgt->name.get());

		m_animationManager->incrCount(tgt);
	}
}

void node::AnimationManager::Block::TargetListener::onSet(const std::string& name)
{
	m_block.nodeName.set(name);
}

void node::AnimationManager::Block::TargetListener::setAnimationManager(AnimationManager& am)
{
	m_animationManager = &am;
}


node::AnimationManager::Block::CtlrIDListener::CtlrIDListener(Block& block) :
	m_block{ block }
{
}

void node::AnimationManager::Block::CtlrIDListener::onSet(const std::string& name)
{
	m_block.ctlrID.set(name);
}
