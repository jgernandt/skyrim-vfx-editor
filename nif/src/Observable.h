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
#include <set>
#include <vector>

namespace nif
{
	template<typename T>
	struct Event {};

	template<typename T> class Observable;

	template<typename T> 
	class IListener 
	{ 
	public:
		IListener() = default; 
		virtual void receive(const Event<T>&, Observable<T>&) = 0;
	};

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
		Observable(Observable&& other) noexcept { *this = std::move(other); }

		~Observable() = default;

		Observable& operator=(const Observable&) = delete;
		Observable& operator=(Observable&& other) noexcept
		{
			m_repo = std::move(other.m_repo);
			m_dirty = true;
			other.m_dirty = true;
			return *this;
		}

		void addListener(IListener<T>& l)
		{
			m_dirty = m_repo.insert(&l).second;
		}
		void removeListener(IListener<T>& l)
		{
			m_dirty = m_repo.erase(&l);
		}

		//Postpone this change. It may or may not be a good idea.
		//[[nodiscard]] std::unique_ptr<Unsubscriber> addListener(IListener<T>&) {}

		void signal(const Event<T>& e)
		{
			if (m_dirty) {
				m_work.clear();
				m_work.insert(m_work.begin(), m_repo.begin(), m_repo.end());
				m_dirty = false;
			}

			for (IListener<T>* l : m_work) {
				assert(l);
				l->receive(e, *this);
			}
		}

	protected:
		std::set<IListener<T>*> m_repo;
		std::vector<IListener<T>*> m_work;
		bool m_dirty{ false };
	};
}
