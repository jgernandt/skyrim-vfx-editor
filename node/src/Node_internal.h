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
}
