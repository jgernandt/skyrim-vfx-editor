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
#include <exception>
#include <list>
#include <map>
#include <vector>
#include "nif.h"
#include "ConnectionHandler.h"
#include "nodes.h"

namespace node
{
	using namespace nif;

	struct ConnectionInfo
	{
		NiObject* object1;
		NiObject* object2;
		std::string field1;
		std::string field2;

		//possibly:
		int target{ -1 };//for controllers
	};

	class Constructor : public HorizontalTraverser<Constructor>
	{
	public:
		Constructor(File& file) : m_file{ file } {}//throw if unsupprtoed version

		template<typename T> void invoke(T& obj);

		std::vector<std::string>& warnings() { return m_warnings; }

		//should transfer ownership of our nodes to target and resolve our connections
		//(the caller is responsible for making sure target is a valid receiver).
		//Arranging of the nodes can be disabled (for testing, mostly).
		void extractNodes(gui::ConnectionHandler& target, bool arrange = true);


		//Used during traversal

		void addConnection(const node::ConnectionInfo& info);
		void addModConnection(NiParticleSystem* target, NiPSysModifier* mod);

		void addNode(NiObject* obj, std::unique_ptr<NodeBase>&& node);

		void pushObject(const ni_ptr<NiObject>& obj) { m_objectStack.push_back(obj); }
		void popObject() { m_objectStack.pop_back(); }
		ni_ptr<NiObject> getObject() const;

		File& getFile() { return m_file; }

	private:
		File& m_file;

		std::vector<ConnectionInfo> m_connections;
		std::map<NiParticleSystem*, std::vector<NiPSysModifier*>> m_modConnections;

		std::vector<std::unique_ptr<NodeBase>> m_nodes;
		std::map<NiObject*, int> m_objectMap;//maps processed objects to an index in m_nodes (-1 if none)

		//Traverse stack
		std::vector<ni_ptr<NiObject>> m_objectStack;

		//Traverse history
		std::set<NiObject*> m_traversed;

		std::vector<std::string> m_warnings;
	};

	/*class Constructor
	{
	public:
		Constructor(nif::File& file) : m_file{ file } {}
		~Constructor() {}

		void makeRoot();

		std::vector<std::string>& warnings() { return m_warnings; }

		//should transfer ownership of our nodes to target and resolve our connections
		//(the caller is responsible for making sure target is a valid receiver)
		void extractNodes(gui::ConnectionHandler& target);

		bool empty() const { return m_nodes.empty(); }
		size_t size() const { return m_nodes.size(); }
		const std::vector<std::unique_ptr<NodeBase>>& nodes() const { return m_nodes; }

	protected:
		//Entry point functions should test for duplication. 
		//Derived functions should pass their created nodes back to the entry point, which inserts it.
		void EP_process(nif::native::NiAVObject* obj);
		std::unique_ptr<NodeShared> process(nif::native::NiNode* obj);
		std::unique_ptr<ParticleSystem> process(nif::native::NiParticleSystem* obj);

		void EP_process(nif::native::NiExtraData* obj);
		std::unique_ptr<StringDataShared> process(nif::native::NiStringExtraData* obj);

		using CtlrList = std::list<Niflib::Ref<Niflib::NiTimeController>>;
		void EP_process(nif::native::NiPSysModifier* obj, const CtlrList& ctlrs);
		std::unique_ptr<Emitter> process(nif::native::NiPSysEmitter* obj, const CtlrList& ctlrs);
		std::unique_ptr<GravityModifier> process(nif::native::NiPSysGravityModifier* obj, const CtlrList& ctlrs);
		std::unique_ptr<RotationModifier> process(nif::native::NiPSysRotationModifier* obj, const CtlrList& ctlrs);

		void EP_process(nif::native::BSEffectShaderProperty* obj);

		//To resolve manager controlled interpolators, I expect this will need access to some strings,
		//or maybe just the target controller
		void EP_process(nif::native::NiInterpolator* obj, const nif::NiTimeController& ctlr);

	private:
		struct Connection
		{
			nif::native::NiObject* object1;
			nif::native::NiObject* object2;
			std::string field1;
			std::string field2;
		};
		nif::File& m_file;
		std::vector<std::unique_ptr<NodeBase>> m_nodes;
		std::map<nif::native::NiObject*, int> m_objectMap;//maps processed objects to an index in m_nodes (-1 if none)
		std::vector<std::string> m_warnings;
		std::list<Connection> m_connections;

		//For clone operations
		constexpr static unsigned int s_version = 0x14020007;
		constexpr static unsigned int s_userVersion = 12;
	};*/
}