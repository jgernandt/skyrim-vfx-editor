#pragma once
#include "CppUnitTest.h"
#include "Constructor.h"

namespace objects
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	class TestConstructor : public nif::HorizontalTraverser<TestConstructor>
	{
		File& m_file;

	public:
		//object stack
		std::vector<ni_ptr<NiObject>> objects;

		//For ConnectorTester
		std::vector<node::ConnectionInfo> connections;
		std::vector<std::pair<NiParticleSystem*, NiPSysModifier*>> modConnections;

		//For FactoryTester
		std::pair<NiObject*, std::unique_ptr<node::NodeBase>> node;

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
		void addModConnection(NiParticleSystem* target, NiPSysModifier* mod) { modConnections.push_back({ target, mod }); }

		template<typename ObjType, typename NodeType>
		void addNode(ObjType* obj, std::unique_ptr<NodeType>&& n) 
		{ 
			Assert::IsTrue(!node.first && !node.second);
			node = { obj, std::move(n) };
		}

		void pushObject(const ni_ptr<NiObject>& obj) { objects.push_back(obj); }
		void popObject() { objects.pop_back(); }
		ni_ptr<NiObject> getObject() const { return !objects.empty() ? objects.back() : ni_ptr<NiObject>(); }

		File& getFile() { return m_file; }
	};
}
