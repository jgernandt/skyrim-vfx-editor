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
#include <cassert>
#include <list>
#include "Observable.h"

namespace nif
{
	template<typename T> class Sequence;
	template<typename T>
	class IListener<Sequence<T>>
	{
	public:
		virtual ~IListener() = default;

		virtual void onInsert(int) {}
		virtual void onErase(int) {}
	};

	template<typename T> using SequenceListener = IListener<Sequence<T>>;

	template<typename T>
	class Sequence final : public Observable<Sequence<T>>
	{
	public:
		Sequence() = default;
		~Sequence()
		{
			size_t i = m_ctnr.size();
			while (i) {
				i--;
				for (SequenceListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onErase(i);
				}
			}
		}

		//We use these to iterate through our container during pre-write sync.
		//They should be changed to dereference to get/set, not the raw element.
		using iterator = typename std::vector<std::shared_ptr<T>>::iterator;
		iterator begin() { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }

		//TODO: use iterators instead of ints
		int insert(int i, const std::shared_ptr<T>& obj)
		{
			assert(i >= 0);

			if (obj) {
				i = std::min((size_t)i, m_ctnr.size());
				if (auto it = std::find(m_ctnr.begin(), m_ctnr.end(), obj); it == m_ctnr.end()) {
					m_ctnr.insert(m_ctnr.begin() + i, obj);
					for (SequenceListener<T>* l : this->m_lsnrs) {
						assert(l);
						l->onInsert(i);
					}
					return i;
				}
				else
					//If already in the sequence, do not move. Return current location. Do not call listeners.
					return std::distance(m_ctnr.begin(), it);
			}
			else
				return -1;
		}
		int erase(int i)
		{
			assert(i >= 0 && (size_t)i < m_ctnr.size());

			m_ctnr.erase(m_ctnr.begin() + i);

			for (SequenceListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onErase(i);
			}

			return i;
		}
		int find(T* obj) const
		{
			int result = -1;

			if (obj) {
				if (auto it = std::find(m_ctnr.begin(), m_ctnr.end(), obj); it != m_ctnr.end())
					result = std::distance(m_ctnr.begin(), it);
			}

			return result;
		}
		int size() const
		{
			if (m_ctnr.size() > std::numeric_limits<int>::max())
				throw std::range_error("Never going to happen");
			return m_ctnr.size();
		}

	private:
		//Advantage of list is that iterators aren't invalidated on insert/erase, but
		//we're not really taking advantage of that. vector might be better for now.
		std::vector<std::shared_ptr<T>> m_ctnr;
	};
}
