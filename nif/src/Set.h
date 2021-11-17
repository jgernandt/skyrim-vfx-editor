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
#include <set>
#include "Observable.h"

namespace nif
{
	template<typename T> class Set;

	template<typename T> inline bool operator<(const std::shared_ptr<T>& lhs, T* rhs)
	{
		return lhs.get() < rhs;
	}
	template<typename T> inline bool operator<(T* lhs, const std::shared_ptr<T>& rhs)
	{
		return lhs < rhs.get();
	}

	template<typename T>
	class IListener<Set<T>>
	{
	public:
		virtual ~IListener() = default;

		virtual void onAdd(T*) {}
		virtual void onRemove(T*) {}
	};

	template<typename T> using SetListener = IListener<Set<T>>;

	template<typename T>
	class Set final : public Observable<Set<T>>
	{
		using ctnr_type = std::map<T*, std::shared_ptr<T>>;
	public:
		Set() = default;
		~Set()
		{
			for (auto&& obj : m_ctnr) {
				for (SetListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onRemove(obj.first);
				}
			}
		}

		class iterator
		{
		public:
			iterator(typename ctnr_type::iterator const& it) : m_it{ it } {}

			T* operator*() noexcept { return m_it->first; }
			const T* operator*() const noexcept { return m_it->first; }
			T* operator->() noexcept { return m_it->first; }
			const T* operator->() const noexcept { return m_it->first; }

			iterator& operator++() noexcept { ++m_it; return *this; }
			iterator operator++(int) noexcept 
			{ 
				iterator tmp;
				operator++();
				return tmp;
			}

			friend bool operator==(const iterator& lhs, const iterator& rhs) noexcept { return lhs.m_it == rhs.m_it; }
			friend bool operator!=(const iterator& lhs, const iterator& rhs) noexcept { return !(lhs == rhs); }

		private:
			typename ctnr_type::iterator m_it;
		};
		iterator begin() { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }

		void add(const std::shared_ptr<T>& obj)
		{
			if (obj) {
				if (auto res = m_ctnr.insert({ obj.get(), obj }); res.second) {
					for (SetListener<T>* l : this->m_lsnrs) {
						assert(l);
						l->onAdd(obj.get());
					}
				}
			}
		}
		void remove(T* obj)
		{
			if (obj) {
				if (int n = m_ctnr.erase(obj)) {
					for (SetListener<T>* l : this->m_lsnrs) {
						assert(l);
						l->onRemove(obj);
					}
				}
			}
		}
		bool has(T* obj) const
		{
			return obj ? m_ctnr.find(obj) != m_ctnr.end() : false;
		}
		virtual size_t size() const { return m_ctnr.size(); }

		void clear()
		{
			
		}

	private:
		ctnr_type m_ctnr;
	};
}

