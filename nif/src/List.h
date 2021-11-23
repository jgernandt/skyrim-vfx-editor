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
	//An ordered list of items that are not copyable or movable

	template<typename T> class List;

	template<typename T>
	class IListener<List<T>>
	{
	public:
		virtual ~IListener() = default;

		//TODO: use iterators instead of ints
		virtual void onInsert(int) {}
		virtual void onErase(int) {}
	};
	template<typename T> using ListListener = IListener<List<T>>;

	template<typename T>
	class List final : public Observable<List<T>>
	{
		using ctnr_type = std::list<T>;
	public:
		using element_type = T;

	public:
		List() = default;
		~List() { clear(); }

		using iterator = typename ctnr_type::iterator;
		using const_iterator = typename ctnr_type::const_iterator;
		iterator begin() { return m_ctnr.begin(); }
		const_iterator begin() const { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }
		const_iterator end() const { return m_ctnr.end(); }

		T& at(int i) 
		{ 
			assert(i >= 0 && (size_t)i < size());
			return *getIt(i);
		}
		const T& at(int i) const 
		{
			assert(i >= 0 && (size_t)i < size());
			return *getIt(i);
		}

		T& back() { return m_ctnr.back(); }
		const T& back() const { return m_ctnr.back(); }
		T& front() { return m_ctnr.front(); }
		const T& front() const { return m_ctnr.front(); }

		//TODO: use iterators instead of ints
		//We could template insertions to forward ctor arguments
		int insert(int i)
		{
			assert(i >= 0 && (size_t)i <= m_ctnr.size());

			if (i == 0)
				m_ctnr.emplace_front();
			else if (i == m_ctnr.size())
				m_ctnr.emplace_back();
			else
				m_ctnr.emplace(getIt(i));

			for (ListListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onInsert(i);
			}

			return i;
		}
		int erase(int i)
		{
			assert(i >= 0 && (size_t)i < m_ctnr.size());

			if (i == 0)
				m_ctnr.pop_front();
			else if (i == m_ctnr.size())
				m_ctnr.pop_back();
			else
				m_ctnr.erase(getIt(i));

			for (ListListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onErase(i);
			}

			return i;
		}

		void pop_back()
		{
			erase(size() - 1);
		}

		void pop_front()
		{
			erase(0);
		}

		void push_back()
		{
			insert(size());
		}

		void push_front()
		{
			insert(0);
		}

		void resize(int size)
		{
			assert(size >= 0);
			while (m_ctnr.size() > (size_t)size)
				pop_back();
			while (m_ctnr.size() < (size_t)size)
				push_back();
		}

		size_t size() const
		{
			return m_ctnr.size();
		}

		void clear()
		{
			while (!m_ctnr.empty())
				pop_back();
		}

	private:
		typename ctnr_type::iterator getIt(int i)
		{
			if (int ri = size() - i; i <= ri) {
				//go front-to-back
				auto it = m_ctnr.begin();
				for (int j = 0; j < i; j++)
					++it;
				return it;
			}
			else {
				//go back-to-front
				auto rit = m_ctnr.rbegin();
				for (int j = 0; j < ri; j++)
					++rit;
				return rit.base();
			}
		}
		typename ctnr_type::const_iterator getIt(int i) const
		{
			if (int ri = size() - i; i <= ri) {
				//go front-to-back
				auto it = m_ctnr.begin();
				for (int j = 0; j < i; j++)
					++it;
				return it;
			}
			else {
				//go back-to-front
				auto rit = m_ctnr.rbegin();
				for (int j = 0; j < ri; j++)
					++rit;
				return rit.base();
			}
		}

		ctnr_type m_ctnr;
	};
}
