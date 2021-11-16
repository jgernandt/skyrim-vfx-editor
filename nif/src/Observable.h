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
#include <vector>

namespace nif
{
	//Must be specialised
	template<typename T> class IListener { IListener() = default; };

	/*class Unsubscriber
	{
	public:
		void unsub() {}
	};*/

	template<typename T>
	class Observable
	{
	public:
		Observable() = default;
		Observable(const Observable&) = delete;

		~Observable() = default;

		Observable& operator=(const Observable&) = delete;

		void addListener(IListener<T>& l)
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it == m_lsnrs.end())
				m_lsnrs.push_back(&l);
		}
		void removeListener(IListener<T>& l)
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it != m_lsnrs.end())
				m_lsnrs.erase(it);
		}

		//Postpone this change. It may or may not be a good idea.
		//[[nodiscard]] std::unique_ptr<Unsubscriber> addListener(IListener<T>&) {}

	protected:
		std::vector<IListener<T>*> m_lsnrs;
	};
}
