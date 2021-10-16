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
#include "ICommand.h"
#include "Connector.h"

namespace gui
{
	class SetConnectionState :
		public ICommand
	{
	public:
		SetConnectionState(const Connector::StateMap& s, bool reversible)
			: m_stateChanges{ s }, m_reversible{ reversible } {}
		SetConnectionState(Connector::StateMap&& s, bool reversible) noexcept
			: m_stateChanges{ std::move(s) }, m_reversible{ reversible } {}
		virtual ~SetConnectionState() = default;

		virtual void execute() override 
		{
			for (auto& item : m_stateChanges) {
				assert(item.first.first);
				item.first.first->setConnectionState(item.first.second, item.second);
			}
		}
		virtual void reverse() override 
		{
			assert(m_reversible);
			for (auto& item : m_stateChanges) {
				assert(item.first.first);
				item.first.first->setConnectionState(item.first.second, !item.second);
			}
		}
		virtual bool reversible() const override { return m_reversible && !m_stateChanges.empty(); }

	protected:
		Connector::StateMap m_stateChanges;
		const bool m_reversible;
	};
}