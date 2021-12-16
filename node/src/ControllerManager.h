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
	class ControllerManager final : public NodeBase, public SequenceListener<NiAVObject>
	{
		class ObjectListener final : public AssignableListener<NiAVObject>
		{
		public:
			ObjectListener(Ptr<NiAVObject>& target) : m_target{ target } {}
			virtual void onAssign(NiAVObject* obj) override;

		private:
			Ptr<NiAVObject>& m_target;
		};

	public:
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

		virtual void setAnimationManager(AnimationManager& am) override;

		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;

		//Good for testing, but has no other purpose
		const ni_ptr<BSBehaviorGraphExtraData>& getBGED() const { return m_bged; }

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 116.0f;

	private:
		const ni_ptr<NiControllerManager> m_manager;
		const ni_ptr<BSBehaviorGraphExtraData> m_bged;

		std::unique_ptr<Root> m_root;

		AnimationManager* m_animationManager{ nullptr };
	};

	class ControllerSequence final : public NodeBase
	{
	public:
		ControllerSequence(const ni_ptr<NiControllerSequence>& obj);
		~ControllerSequence();

		constexpr static const char* BEHAVIOUR = "Behaviour";
	};
}
