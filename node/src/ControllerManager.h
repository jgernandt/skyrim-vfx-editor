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
#include "NodeBase.h"

namespace node
{
	class ControllerManager final : public NodeBase, public ListListener<AVObject>
	{
	public:
		class Root final : public Field, public AssignableListener<NiAVObject>
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

			virtual void onAssign(NiAVObject* obj) override;

		public:
			constexpr static const char* ID = "Root";

		private:
			ControllerManager& m_node;
			Rcvr m_rcvr;
			Sender<Ptr<NiAVObject>> m_sndr;
		};

		class NameSyncer final : public AssignableListener<NiAVObject>, public PropertyListener<std::string>
		{
		public:
			NameSyncer(AVObject& av);
			~NameSyncer();
			//Start listening to the name of obj
			virtual void onAssign(NiAVObject* obj) override;
			//set the name of target
			virtual void onSet(const std::string& name) override;

		private:
			AVObject& m_av;
			std::weak_ptr<Property<std::string>> m_source;
		};

	public:
		ControllerManager(const ni_ptr<NiControllerManager>& manager, 
			const ni_ptr<BSBehaviorGraphExtraData>& bged);
		~ControllerManager();

		//We are responsible for syncing the name of every AVObject added to the palette
		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;

		//Good for testing, but has no other purpose
		const ni_ptr<BSBehaviorGraphExtraData>& getBGED() const { return m_bged; }

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 116.0f;

	private:
		const ni_ptr<NiControllerManager> m_manager;
		const ni_ptr<BSBehaviorGraphExtraData> m_bged;

		std::vector<std::unique_ptr<NameSyncer>> m_lsnrs;

		std::unique_ptr<Root> m_root;
	};

	class ControllerSequence final : public NodeBase
	{
	public:
		ControllerSequence(const ni_ptr<NiControllerSequence>& obj);
		~ControllerSequence();

		constexpr static const char* BEHAVIOUR = "Behaviour";
	};
}
