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
