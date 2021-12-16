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
#include "nif.h"
#include "node_concepts.h"

namespace node
{
	using namespace nif;

	//This class is turning out stupidly complicated. Needs refactoring!
	class AnimationManager final : public HorizontalTraverser<AnimationManager>
	{
	public:
		struct Block
		{
			Block();
			~Block();

			Ref<NiAVObject> target;
			ni_ptr<NiTimeController> controller;
			Property<std::string> nodeName;
			Property<std::string> propertyType;
			Property<std::string> ctlrType;
			Property<std::string> ctlrID;
			Property<std::string> iplrID;

		private:
			friend class AnimationManager;

			//Listen to the target assignable and set the nodeName accordingly
			class TargetListener final :
				public AssignableListener<NiAVObject>, public PropertyListener<std::string>
			{
			public:
				TargetListener(Block& block);
				~TargetListener();

				//Set obj as current source (i.e. start listening to its name)
				virtual void onAssign(NiAVObject* obj) override;
				//set nodeName to match
				virtual void onSet(const std::string& name) override;

				//Should be called immediately after construction (this is stupid, I don't like it)
				void setAnimationManager(AnimationManager& am);

			private:
				Block& m_block;
				AnimationManager* m_animationManager{ nullptr };
				std::weak_ptr<NiAVObject> m_currentSrc;
			};

			class CtlrIDListener final : public PropertyListener<std::string>
			{
			public:
				CtlrIDListener(Block& block);
				virtual void onSet(const std::string& name) override;

			private:
				Block& m_block;
			};

			ni_ptr<Property<std::string>> ctlrIDProperty;

			TargetListener targetLsnr;
			CtlrIDListener ctlrIDSyncer;
		};

		struct BlockInfo
		{
			ni_ptr<NiTimeController> ctlr;
			ni_ptr<Property<std::string>> ctlrIDProperty;
			ni_ptr<NiAVObject> target;
			std::string nodeName;
			std::string propertyType;
			std::string ctlrType;
			std::string ctlrID;
			std::string iplrID;
		};

	private:
		/*class ObjectCounter final : public AssignableListener<NiAVObject>
		{
		public:
		};*/

	public:
		~AnimationManager();

		template<typename T>
		void invoke(T& obj);

		//ControllerSequences should observe this list and its fields
		List<Block>& blocks() { return m_blocks; }

		//ControllerManager should observe this list
		Sequence<NiAVObject>& objects() { return m_objects; }

		//Called by BlendIplr nodes or during initial traversal
		Block* registerBlock(const BlockInfo& info);
		void unregisterBlock(Block* it);

		//Controllers that we have not yet implemented will need some info from the loaded file.
		//Get that from here.
		ControlledBlock* getCurrentBlock() const { return m_currentBlock; }
		void setCurrentBlock(ControlledBlock* block) { m_currentBlock = block; }

	private:
		void incrCount(const ni_ptr<NiAVObject>& obj);
		void decrCount(NiAVObject* obj);

	private:
		List<Block> m_blocks;
		Sequence<NiAVObject> m_objects;
		ControlledBlock* m_currentBlock{ nullptr };

		std::map<NiAVObject*, int> m_objCount;
		//std::vector<std::unique_ptr<ObjectCounter>> m_objCounters;
	};
}
