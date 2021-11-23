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
#include <map>
#include <set>
#include "Observable.h"

namespace nif
{
	template<typename T> class Set;

	template<typename T> inline bool operator<(const std::shared_ptr<T>& lhs, T* rhs) noexcept
	{
		return lhs.get() < rhs;
	}
	template<typename T> inline bool operator<(T* lhs, const std::shared_ptr<T>& rhs) noexcept
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
		using ref_type = T;

	public:
		Set() = default;
		~Set() { clear(); }

		class const_iterator
		{
		public:
			//We are keeping an (non-const) iterator, to simplify code reuse.
			//This should be fine, since we control what it may be used for anyway.
			const_iterator(typename ctnr_type::iterator const& it) : m_it{ it } {}
			const_iterator(const const_iterator& other) : m_it{ other.m_it } {}

			T* operator*() const noexcept { return m_it->first; }
			T* operator->() const noexcept { return m_it->first; }

			const_iterator& operator++() noexcept { ++m_it; return *this; }
			const_iterator operator++(int) noexcept
			{ 
				const_iterator tmp = *this;
				operator++();
				return tmp;
			}

			friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.m_it == rhs.m_it; }
			friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

		protected:
			typename ctnr_type::iterator m_it;
		};
		class iterator : public const_iterator
		{
		public:
			iterator(typename ctnr_type::iterator const& it) : const_iterator{ it } {}
			iterator(const iterator& other) : const_iterator{ other } {}

			T* operator*() noexcept { return this->m_it->first; }
			T* operator->() noexcept { return this->m_it->first; }

			iterator& operator++() noexcept { const_iterator::operator++(); return *this; }
			iterator operator++(int) noexcept
			{
				iterator tmp = *this;
				const_iterator::operator++();
				return tmp;
			}
		};
		iterator begin() { return m_ctnr.begin(); }
		const_iterator begin() const { return const_cast<Set<T>&>(*this).begin(); }
		iterator end() { return m_ctnr.end(); }
		const_iterator end() const { return const_cast<Set<T>&>(*this).end(); }

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
			//No reason to go for speed here. Better to go for consistency
			//and let the call the listeners after each element has been removed.
			std::vector<T*> vec;
			vec.reserve(m_ctnr.size());
			for (auto&& obj : m_ctnr)
				vec.push_back(obj.first);

			for (T* obj : vec)
				remove(obj);
		}

	private:
		ctnr_type m_ctnr;
	};
}

