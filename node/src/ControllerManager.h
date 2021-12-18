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
#include "AnimationManager.h"
#include "NodeBase.h"

namespace node
{
	class ControllerManager final : public NodeBase, public SequenceListener<NiAVObject>
	{
	public:
		class Actions final : public Field
		{
		public:
			Actions(const std::string& name, ControllerManager& node);

			constexpr static const char* ID = "Actions";

		private:
			PtrReceiver<NiControllerManager> m_rcvr;
			Sender<Set<NiControllerSequence>> m_sndr;
		};

		class Root final : public Field
		{
			class Rcvr final : public Receiver<IControllableRoot>, public PropertyListener<std::string>
			{
			public:
				Rcvr(Root& root);
				~Rcvr();

				virtual void onConnect(IControllableRoot& ifc) override;
				virtual void onDisconnect(IControllableRoot& ifc) override;

				virtual void onSet(const std::string& file) override;

			private:
				Root& m_root;
				IControllableRoot* m_ifc{ nullptr };
			};

		public:
			Root(const std::string& name, ControllerManager& node);
			~Root();

		public:
			constexpr static const char* ID = "Root";

		private:
			ControllerManager& m_node;
			Rcvr m_rcvr;
			Sender<Ptr<NiAVObject>> m_sndr;
		};

	public:
		ControllerManager(const ni_ptr<NiControllerManager>& manager, 
			const ni_ptr<BSBehaviorGraphExtraData>& bged);
		~ControllerManager();

		virtual void setAnimationManager(const std::shared_ptr<AnimationManager>& am) override;

		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;

		//Good for testing, but has no other purpose
		const ni_ptr<BSBehaviorGraphExtraData>& getBGED() const { return m_bged; }

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 130.0f;

	private:
		const ni_ptr<NiControllerManager> m_manager;
		const ni_ptr<BSBehaviorGraphExtraData> m_bged;

		std::unique_ptr<Actions> m_actions;
		std::unique_ptr<Root> m_root;

		std::weak_ptr<AnimationManager> m_animationManager;
	};

	class ControllerSequence final : 
		public NodeBase,
		public ListListener<AnimationManager::Block>,
		public AssignableListener<NiControllerManager>,
		public AssignableListener<NiObjectNET>,
		public PropertyListener<float>,
		public PropertyListener<std::string>
	{
	public:
		class Behaviour final : public Field
		{
		public:
			Behaviour(const std::string& name, ControllerSequence& node);

			constexpr static const char* ID = "Behaviour";

		private:
			SetReceiver<NiControllerSequence> m_rcvr;
			Sender<Ptr<NiControllerManager>> m_sndr;
		};

	private:
		using CacheKey = std::pair<NiTimeController*, size_t>;

		class StringForwarder final : public PropertyListener<std::string>
		{
		public:
			StringForwarder(Property<std::string>& dst);
			virtual void onSet(const std::string& s) override;

		private:
			Property<std::string>& m_dst;
		};

		struct BlockListeners
		{
			BlockListeners(ControlledBlock& dst);
			//propertyType, ctlrType and iplrID should be constants as far as I've seen,
			//but might as well go all the way here
			StringForwarder nodeName;
			StringForwarder propertyType;
			StringForwarder ctlrType;
			StringForwarder ctlrID;
			StringForwarder iplrID;
		};

	public:
		ControllerSequence(File& file, const ni_ptr<NiControllerSequence>& obj);
		~ControllerSequence();

		virtual void setAnimationManager(const std::shared_ptr<AnimationManager>& am) override;

		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;

		virtual void onAssign(NiControllerManager* obj) override;
		virtual void onAssign(NiObjectNET* obj) override;
		virtual void onSet(const float&) override;
		virtual void onSet(const std::string& accumRootName) override;

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 116.0f;

	private:
		File& m_file;
		const ni_ptr<NiControllerSequence> m_obj;

		std::unique_ptr<Behaviour> m_behaviour;

		std::weak_ptr<Ptr<NiObjectNET>> m_managerTargetPtr;
		std::weak_ptr<Property<std::string>> m_accumRootNameProperty;

		std::weak_ptr<AnimationManager> m_animationManager;
		std::vector<std::unique_ptr<BlockListeners>> m_blockLsnrs;
		std::map<CacheKey, ni_ptr<NiInterpolator>> m_iplrCache;
	};

	//ControllerSequences will not know what interpolator to use. 
	//They get one by sending a IplrFactoryVisitor to their ctlr.
	class IplrFactoryVisitor : public HorizontalTraverser<IplrFactoryVisitor>
	{
	public:
		IplrFactoryVisitor(File& file, const std::string& iplrID) :
			m_file{ file }, m_iplrID{ iplrID } {}

		template<typename T> void invoke(const T& obj);

	public:
		ni_ptr<NiInterpolator> iplr;

	private:
		nif::File& m_file;
		const std::string m_iplrID;
	};
}
