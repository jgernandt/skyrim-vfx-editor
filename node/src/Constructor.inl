#pragma once
#include "Constructor.h"
#include "nodes_internal.h"

namespace node
{
	template<typename T>
	void Constructor::invoke(T& obj)
	{
		bool firstVisit = m_traversed.insert(&obj).second;
		if (firstVisit) {
			if (m_objectStack.empty()) {
				//This must be the root node
				assert(m_nodes.empty() && m_file.getRoot().get() == static_cast<nif::NiObject*>(&obj));
				m_objectStack.push_back(m_file.getRoot());

				//Create a root node
				RootFactory<T>{}.up(obj, *this);

				//if it failed, the root object must be invalid
				if (m_nodes.empty())
					throw std::runtime_error("Invalid root object");
			}
			else
				Factory<T>{}.up(obj, *this);
		}

		Connector<T>{}.down(obj, *this);

		if (firstVisit)
			Forwarder<T>{}.down(obj, *this);
	}
}
