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
			std::vector<std::pair<NiTimeController*, std::string>> visited;
			for (auto&& seq : obj.ctlrSequences) {
				assert(seq);
				for (auto&& block : seq->blocks) {
					//skip duplicate blocks
					std::pair<NiTimeController*, std::string> id = 
						{ block.controller.assigned().get(), block.iplrID.get() };
					if (id.first && std::find(visited.begin(), visited.end(), id) == visited.end()) {
						visited.push_back(id);
						v.setCurrentBlock(&block);
						id.first->receive(v);
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
}
