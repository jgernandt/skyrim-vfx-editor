#pragma once
#include "Constructor.h"
#include "nodes_internal.h"

namespace node
{
	template<typename T>
	void Constructor::invoke(T& obj)
	{
		Connector<T>{}.down(obj, *this);

		if (m_objectStack.empty()) {
			//This must be the root node
			assert(m_nodes.empty() && m_file.getRoot().get() == static_cast<nif::NiObject*>(&obj));
			m_objectStack.push_back(m_file.getRoot());

			//Create a root node
			RootFactory<T>{}.down(obj, *this);

			//if it failed, the root object must be invalid
			if (m_nodes.empty())
				throw std::runtime_error("Invalid root object");

			Forwarder<T>{}.down(obj, *this);
		}
		else if (auto it = m_traversed.insert(&obj); it.second) {
			//We have not seen this object before
			Factory<T>{}.up(obj, *this);
			Forwarder<T>{}.down(obj, *this);
		}
		//else ignore
	}
}
