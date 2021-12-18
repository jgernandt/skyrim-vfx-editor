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

#pragma once
#include "ControllerManager.h"
#include "Constructor.h"
#include "Controllers_internal.h"

namespace node
{
	using namespace nif;

	constexpr bool DEFAULT_CUMULATIVE = false;
	constexpr const char* BGED_NAME = "BGED";
	constexpr bool DEFAULT_CONTROLS_BASE_SKELETON = false;

	constexpr unsigned int DEFAULT_ARRAY_GROW_BY = 1;//no idea what this does
	constexpr float DEFAULT_WEIGHT = 1.0f;
	constexpr CycleType DEFAULT_CYCLE_TYPE = CycleType::CLAMP;

	template<>
	class Default<ControllerManager>
	{
	public:
		std::unique_ptr<ControllerManager> create(
			File& file,
			ni_ptr<NiControllerManager> mngr = ni_ptr<NiControllerManager>(),
			ni_ptr<BSBehaviorGraphExtraData> graph = ni_ptr<BSBehaviorGraphExtraData>())
		{
			if (!mngr) {
				mngr = file.create<NiControllerManager>();
				if (!mngr)
					throw std::runtime_error("Failed to create NiControllerManager");

				//I doubt these matter, but might as well default them
				mngr->flags.raise(DEFAULT_CTLR_FLAGS);
				mngr->frequency.set(DEFAULT_FREQUENCY);
				mngr->phase.set(DEFAULT_PHASE);
				mngr->startTime.set(DEFAULT_STARTTIME);
				mngr->stopTime.set(DEFAULT_STOPTIME);

				mngr->cumulative.set(DEFAULT_CUMULATIVE);
			}
			if (!mngr->objectPalette.assigned()) {
				auto palette = file.create<NiDefaultAVObjectPalette>();
				if (!palette)
					throw std::runtime_error("Failed to create NiDefaultAVObjectPalette");

				mngr->objectPalette.assign(palette);
			}
			if (!graph) {
				graph = file.create<BSBehaviorGraphExtraData>();
				if (!graph)
					throw std::runtime_error("Failed to create BSBehaviorGraphExtraData");

				graph->name.set(BGED_NAME);
				graph->controlsBaseSkeleton.set(DEFAULT_CONTROLS_BASE_SKELETON);
			}
			return std::make_unique<ControllerManager>(mngr, graph);
		}
	};

	template<>
	class AnimationInit<NiControllerManager> : public VerticalTraverser<NiControllerManager, AnimationInit>
	{
	public:
		template<typename VisitorType>
		bool operator() (NiControllerManager& obj, VisitorType& v) 
		{ 
			//go through all blocks in all sequences
			//forward to each controller, but only once per block
			std::vector<ControlledBlock*> visited;
			for (auto&& seq : obj.ctlrSequences) {
				assert(seq);
				for (auto&& block : seq->blocks) {
					//skip indentical blocks
					auto up = [&block](ControlledBlock* b)
					{
						return block.controller.assigned().get() == b->controller.assigned().get() &&
							block.nodeName.get() == b->nodeName.get() &&
							block.ctlrID.get() == b->ctlrID.get() &&
							block.iplrID.get() == b->iplrID.get();
					};
					if (block.controller.assigned() && std::find_if(visited.begin(), visited.end(), up) == visited.end()) {
						visited.push_back(&block);
						v.setCurrentBlock(&block);
						block.controller.assigned()->receive(v);
						v.setCurrentBlock(nullptr);
					}
				}
			}
			return false; 
		}
	};

	template<>
	class Connector<NiControllerManager> : public VerticalTraverser<NiControllerManager, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiControllerManager& obj, C& ctor)
		{
			//Connect to the target
			if (auto target = obj.target.assigned())
				ctor.addConnection({ &obj, target.get(), ControllerManager::Root::ID, Root::Behaviour::ID });
			return true;
		}
	};

	template<>
	class Factory<NiControllerManager> : public VerticalTraverser<NiControllerManager, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiControllerManager& obj, C& ctor)
		{
			if (auto mngr = std::static_pointer_cast<NiControllerManager>(ctor.getObject()); mngr.get() == &obj) {
				//we need to find our BGED
				ni_ptr<BSBehaviorGraphExtraData> bged;
				if (auto target = obj.target.assigned()) {
					for (auto&& data : target->extraData) {
						assert(data);
						if (data->type() == BSBehaviorGraphExtraData::TYPE) {
							if (static_cast<BSBehaviorGraphExtraData*>(data.get())->name.get() == BGED_NAME) {
								bged = std::static_pointer_cast<BSBehaviorGraphExtraData>(data);
								break;
							}
						}
					}
				}
				ctor.addNode(&obj, Default<ControllerManager>{}.create(ctor.getFile(), mngr, bged));
			}
			return false;
		}
	};

	template<>
	class Forwarder<NiControllerManager> : public VerticalTraverser<NiControllerManager, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiControllerManager& obj, C& ctor)
		{
			for (auto&& seq : obj.ctlrSequences) {
				assert(seq);
				ctor.pushObject(seq);
				seq->receive(ctor);
				ctor.popObject();
			}
			return true;
		}
	};


	template<>
	class Default<ControllerSequence>
	{
	public:
		std::unique_ptr<ControllerSequence> create(
			File& file,	ni_ptr<NiControllerSequence> obj = ni_ptr<NiControllerSequence>())
		{
			if (!obj) {
				obj = file.create<nif::NiControllerSequence>();
				if (!obj)
					throw std::runtime_error("Failed to create NiControllerSequence");

				obj->arrayGrowBy.set(DEFAULT_ARRAY_GROW_BY);
				obj->weight.set(DEFAULT_WEIGHT);
				obj->cycleType.set(DEFAULT_CYCLE_TYPE);
				obj->frequency.set(DEFAULT_FREQUENCY);
				obj->startTime.set(DEFAULT_STARTTIME);
				obj->stopTime.set(DEFAULT_STOPTIME);
			}
			if (!obj->textKeys.assigned()) {
				auto new_keys = file.create<NiTextKeyExtraData>();
				if (!new_keys)
					throw std::runtime_error("Failed to create NiTextKeyExtraData");
				obj->textKeys.assign(new_keys);
			}
			auto&& keys = obj->textKeys.assigned()->keys;

			if (keys.size() == 0) {
				keys.push_back();
				keys.back().value.set("start");
			}
			else {
				//find "start" and move to front, or add
				int pos = 0;
				for (auto&& key : keys) {
					if (key.value.get() == "start")
						break;
					pos++;
				}
				if ((size_t)pos < keys.size())
					keys.move(pos, 0);
				else {
					keys.push_front();
					keys.front().value.set("start");
				}
			}
			keys.front().time.set(obj->startTime.get());

			if (keys.size() == 1) {
				keys.push_back();
				keys.back().value.set("end");
			}
			else {
				//find "end" and move to back, or add
				int pos = 0;
				for (auto&& key : keys) {
					if (key.value.get() == "end")
						break;
					pos++;
				}
				if ((size_t)pos < keys.size())
					keys.move(pos, (int)keys.size() - 1);
				else {
					keys.push_back();
					keys.back().value.set("end");
				}
			}
			keys.back().time.set(obj->stopTime.get());

			return std::make_unique<ControllerSequence>(file, obj);
		}
	};

	template<>
	class Connector<NiControllerSequence> : public VerticalTraverser<NiControllerSequence, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiControllerSequence& obj, C& ctor)
		{
			//Connect to the manager
			if (auto manager = obj.manager.assigned())
				ctor.addConnection({ &obj, manager.get(), ControllerSequence::Behaviour::ID, ControllerManager::Actions::ID });
			return true;
		}
	};

	template<>
	class Factory<NiControllerSequence> : public VerticalTraverser<NiControllerSequence, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiControllerSequence& obj, C& ctor)
		{
			if (auto ptr = std::static_pointer_cast<NiControllerSequence>(ctor.getObject()); ptr.get() == &obj)
				ctor.addNode(&obj, Default<ControllerSequence>{}.create(ctor.getFile(), ptr));
			return false;
		}
	};

	template<typename T> 
	inline void IplrFactoryVisitor::invoke(const T& obj) 
	{
		InterpolatorFactory<T>{}.up(obj, *this, m_file, m_iplrID);
	}
}
