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
#include "AVObject.h"

namespace node
{
	using namespace nif;

	class NodeShared : public AVObject
	{
	protected:
		NodeShared(const ni_ptr<NiNode>& obj);

	public:
		virtual ~NodeShared() = default;

	public:
		constexpr static const char* CHILDREN = "Children";

	protected:
		class ChildField;

		std::unique_ptr<Field> m_children;
	};

	class Node : public NodeShared
	{
	public:
		using default_object = NiNode;

		Node(const ni_ptr<NiNode>& obj);
		virtual ~Node();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 180.0f;
	};

	class Root final : public NodeShared
	{
	public:
		class Behaviour final : public Field
		{
			class ControllableRoot final : public IControllableRoot
			{
			public:
				ControllableRoot(const ni_ptr<NiAVObject>& obj);

				virtual void addController(const ni_ptr<NiTimeController>& ctlr) override;
				virtual void removeController(NiTimeController* ctlr) override;

				virtual Set<NiExtraData>& extraData() override;

			private:
				const ni_ptr<NiAVObject> m_obj;
			};

		public:
			Behaviour(const std::string& name, NodeBase& node, const ni_ptr<NiAVObject>& obj);
			constexpr static const char* ID = "Behaviour";

		private:
			ControllableRoot m_ifc;
			PtrReceiver<NiAVObject> m_rcvr;
			Sender<IControllableRoot> m_sndr;
		};

		using default_object = BSFadeNode;

	public:
		Root(const ni_ptr<NiNode>& obj);
		~Root();

	public:
		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 140.0f;

	private:
		std::unique_ptr<Behaviour> m_behaviour;
	};

	class BillboardNode final : public Node
	{
	public:
		BillboardNode(const ni_ptr<NiBillboardNode>& obj);
		~BillboardNode();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 225.0f;
	};
}
