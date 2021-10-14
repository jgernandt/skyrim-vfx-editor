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

#include "pch.h"
#include "gui_types.h"
#include "ConnectionHandler.h"
#include "ConnectorActions.h"

gui::ConnectionHandler::ConnectionHandler()
{
}

gui::ConnectionHandler::~ConnectionHandler()
{
}

void gui::ConnectionHandler::accept(Visitor& v)
{
	v.visit(*this);
}

void gui::ConnectionHandler::frame()
{
	Composite::frame();

	//Currently held connector
	if (m_seeker) {
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			//send reversible state change action and reset ourselves
			asyncInvoke<SetConnectionState>(std::move(m_stateChanges), true);
			m_seeker = nullptr;
		}
		else {
			ImGui::GetForegroundDrawList()->
				AddLine(gui_type_conversion<ImVec2>::from(m_seeker->getPosition()), ImGui::GetMousePos(), { 0xc0ffffff }, 3.0f);
		}
	}

	//Draw all connections
	//If we want to draw some other curve (not line), we have to fetch some sort of "end point"
	//struct from the connectors (position + direction). The connector itself might have to
	//refer to its PositionController to determine its end point.
	for (auto& e : m_incidence) {
		assert(e.first && e.second);
		ImGui::GetBackgroundDrawList()->
			AddLine(gui_type_conversion<ImVec2>::from(e.first->getPosition()),
				gui_type_conversion<ImVec2>::from(e.second->getPosition()),
				{ 0xc0ffffff }, 3.0f);
	}
}

void gui::ConnectionHandler::startSearch(Connector* c, Connector::StateMap&& s)
{
	if (c) {
		m_seeker = c;
		m_stateChanges = std::move(s);
	}
}

bool gui::ConnectionHandler::query(Connector* c)
{
	//We could keep a set of valid connectors, to avoid calling this from every connector every frame.
	//No big deal, though.
	if (m_seeker && c && c != m_seeker)
		return c->getReceiver().canReceive(m_seeker->getSender()) && m_seeker->getReceiver().canReceive(c->getSender());
	else
		return false;

}

void gui::ConnectionHandler::connect(Connector* c)
{
	if (query(c)) {
		assert(c && m_seeker);
		//Invoke the state change, save changes for the complete command later

		Connector::StateMap s = c->queryStateChange(m_seeker, true);
		merge(s, m_seeker->queryStateChange(c, true));
		
		asyncInvoke<SetConnectionState>(s, false);

		merge(m_stateChanges, std::move(s));
	}
}

void gui::ConnectionHandler::setIncidence(Connector* c1, Connector* c2, bool state)
{
	auto it = std::find_if(m_incidence.begin(), m_incidence.end(),
		[c1, c2](const std::pair<Connector*, Connector*>& p) { 
		return p == std::make_pair(c1, c2) || p == std::make_pair(c2, c1);//unordered pair
	});
	if ((it != m_incidence.end()) != state) {
		if (state)
			m_incidence.push_back({ c1, c2 });
		else
			m_incidence.erase(it);
	}
}
