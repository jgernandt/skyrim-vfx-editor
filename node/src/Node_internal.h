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
#include "Node.h"
#include "Constructor.h"
#include "AVObject_internal.h"

namespace node
{
	using namespace nif;

	template<>
	class Default<Node> : public Default<AVObject>
	{
	public:
		std::unique_ptr<Node> create(File& file)
		{
			auto obj = file.create<NiNode>();
			if (!obj)
				throw std::runtime_error("Failed to create NiNode");

			setDefaults(*obj);
			return std::make_unique<Node>(obj);
		}
		void setDefaults(NiNode& obj)
		{
			Default<AVObject>::setDefaults(obj);

			static int n = 0;
			obj.name.set(std::string("Node") + std::to_string(++n));
		}
	};

	template<>
	class AnimationInit<NiNode> : public VerticalTraverser<NiNode, AnimationInit>
	{
	public:
		template<typename VisitorType>
		bool operator() (NiNode& obj, VisitorType& v) 
		{
			for (auto&& child : obj.children) {
				assert(child);
				v.addObject(child);
				child->receive(v);
			}

			for (auto&& ctlr : obj.controllers) {
				assert(ctlr);
				if (ctlr->type() == NiControllerManager::TYPE) {
					ctlr->receive(v);
					break;
				}
			}
			return false; 
		}
	};

	template<>
	class Connector<NiNode> : public VerticalTraverser<NiNode, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiNode& obj, C& ctor) 
		{ 
			//Register a connection with each child and move on
			for (auto&& child : obj.children)
				ctor.addConnection(ConnectionInfo{ &obj, child.get(), Node::CHILDREN, AVObject::PARENT });

			return true; 
		}
	};

	template<>
	class Factory<NiNode> : public VerticalTraverser<NiNode, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiNode& obj, C& ctor)
		{
			if (auto&& ptr = ctor.getObject(); ptr.get() == &obj) {
				auto node = std::make_unique<Node>(std::static_pointer_cast<NiNode>(ptr));
				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};
	template<>
	class RootFactory<NiNode> : public VerticalTraverser<NiNode, RootFactory>
	{
	public:
		template<typename C>
		bool operator() (NiNode& obj, C& ctor) 
		{
			if (auto&& ptr = ctor.getObject(); ptr.get() == &obj) {
				auto node = std::make_unique<Root>(std::static_pointer_cast<NiNode>(ptr));
				ctor.addNode(&obj, std::move(node));
			}
			return false; 
		}
	};

	template<>
	class Forwarder<NiNode> : public VerticalTraverser<NiNode, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiNode& obj, C& ctor) 
		{ 
			for (auto&& child : obj.children) {
				assert(child);
				ctor.pushObject(child);
				child->receive(ctor);
				ctor.popObject();
			}
			return true; 
		}
	};


	template<>
	class Default<BillboardNode> : public Default<Node>
	{
	public:
		std::unique_ptr<Node> create(File& file, const ni_ptr<NiBillboardNode>& obj = ni_ptr<NiBillboardNode>())
		{
			if (obj)
				return std::make_unique<BillboardNode>(obj);
			else {
				auto new_obj = file.create<NiBillboardNode>();
				if (!new_obj)
					throw std::runtime_error("Failed to create NiBillboardNode");

				setDefaults(*new_obj);
				return std::make_unique<BillboardNode>(new_obj);
			}
		}
	};

	template<>
	class Factory<NiBillboardNode> : public VerticalTraverser<NiBillboardNode, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiBillboardNode& obj, C& ctor)
		{
			if (auto&& ptr = std::static_pointer_cast<NiBillboardNode>(ctor.getObject()); ptr.get() == &obj) {
				auto node = Default<BillboardNode>{}.create(ctor.getFile(), ptr);
				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};
}
