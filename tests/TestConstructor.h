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
		std::pair<NiParticleSystem*, std::vector<NiPSysModifier*>> modConnections;

		//For FactoryTester
		std::pair<NiObject*, std::unique_ptr<node::NodeBase>> node;

		//For ForwardTester
		std::vector<NiObject*> forwards;

		std::vector<std::string> m_warnings;
		std::vector<std::function<void()>> m_postProcess;

	public:
		TestConstructor(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& obj)
		{
			if (!objects.empty() && objects.back().get() == &obj)
				forwards.push_back(&obj);
		}

		std::vector<std::string>& warnings() { return m_warnings; }

		void addConnection(const node::ConnectionInfo& info) { connections.push_back(info); }
		void addModConnections(NiParticleSystem* target, std::vector<NiPSysModifier*>&& mods) { modConnections = { target, std::move(mods) }; }

		template<typename ObjType, typename NodeType>
		void addNode(ObjType* obj, std::unique_ptr<NodeType>&& n) 
		{ 
			Assert::IsTrue(!node.first && !node.second);
			node = { obj, std::move(n) };
		}

		void addPostProcess(const std::function<void()>& fcn) { m_postProcess.push_back(fcn); }

		void pushObject(const ni_ptr<NiObject>& obj) { objects.push_back(obj); }
		void popObject() { objects.pop_back(); }
		ni_ptr<NiObject> getObject() const { return !objects.empty() ? objects.back() : ni_ptr<NiObject>(); }

		File& getFile() { return m_file; }
	};
}
