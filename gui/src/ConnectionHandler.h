//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include <utility>
#include <vector>
#include "Connector.h"

namespace gui
{
	class ConnectionHandler :
		public Composite
	{
	public:
		ConnectionHandler();
		virtual ~ConnectionHandler();

		virtual void accept(Visitor& v) override;
		virtual void frame() override;

		void startSearch(Connector* c, Connector::StateMap&& s);
		bool query(Connector* c);
		void connect(Connector* c);

		void setIncidence(Connector* c1, Connector* c2, bool state);

	private:
		std::vector<std::pair<Connector*, Connector*>> m_incidence;//sparse matrix of current connections
		Connector* m_seeker{ nullptr };
		Connector::StateMap m_stateChanges;
	};
}