#pragma once
#include "Constructor.h"

namespace objects
{
	using namespace nif;

	class TestConstructor : public nif::HorizontalTraverser<TestConstructor>
	{
		File& m_file;

	public:
		//object stack
		std::vector<ni_ptr<NiObject>> objects;

		//For ConnectorTester
		std::vector<node::ConnectionInfo> connections;

		//For FactoryTester
		std::map<const NiObject*, std::unique_ptr<node::NodeBase>> nodes;

		//For ForwardTester
		std::vector<NiObject*> forwards;

	public:
		TestConstructor(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& obj)
		{
			if (!objects.empty() && objects.back().get() == &obj)
				forwards.push_back(&obj);
		}

		void addConnection(const node::ConnectionInfo& info) { connections.push_back(info); }

		template<typename ObjType, typename NodeType>
		void addNode(ObjType* obj, std::unique_ptr<NodeType>&& node) { nodes.insert({ obj, std::move(node) }); }

		void pushObject(const ni_ptr<NiObject>& obj) { objects.push_back(obj); }
		void popObject() { objects.pop_back(); }
		ni_ptr<NiObject> getObject() const { return !objects.empty() ? objects.back() : ni_ptr<NiObject>(); }

		File& getFile() { return m_file; }
	};
}
