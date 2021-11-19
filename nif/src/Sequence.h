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
#include <vector>
#include "Observable.h"

namespace nif
{
	template<typename T> inline bool operator==(const std::shared_ptr<T>& lhs, T* rhs) { return lhs.get() == rhs; }
	template<typename T> inline bool operator==(T* rhs, const std::shared_ptr<T>& lhs) { return rhs == lhs; }

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
		//Advantage of list is that iterators aren't invalidated on insert/erase, but
		//we're not really taking advantage of that. vector might be better for now.
		using ctnr_type = std::vector<std::shared_ptr<T>>;

	public:
		Sequence() = default;
		~Sequence() { clear(); }

		class const_iterator
		{
		public:
			//We are keeping an (non-const) iterator, to simplify code reuse.
			//This should be fine, since we control what it may be used for anyway.
			const_iterator(typename ctnr_type::iterator const& it) : m_it{ it } {}
			const_iterator(const const_iterator& other) : m_it{ other.m_it } {}

			T* operator*() const noexcept { return m_it->get(); }
			T* operator->() const noexcept { return m_it->get(); }

			const_iterator& operator++() noexcept { ++m_it; return *this; }
			const_iterator operator++(int) noexcept
			{
				const_iterator tmp = *this;
				operator++();
				return tmp;
			}

			friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.m_it == rhs.m_it; }
			friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

		private:
			typename ctnr_type::iterator m_it;
		};
		class iterator : public const_iterator
		{
		public:
			iterator(typename ctnr_type::iterator const& it) : const_iterator{ it } {}
			iterator(const iterator& other) : const_iterator{ other } {}

			T* operator*() noexcept { return this->m_it->get(); }
			T* operator->() noexcept { return this->m_it->get(); }

			iterator& operator++() noexcept { const_iterator::operator++(); return *this; }
			iterator operator++(int) noexcept
			{
				iterator tmp = *this;
				const_iterator::operator++();
				return tmp;
			}
		};
		iterator begin() { return m_ctnr.begin(); }
		const_iterator begin() const { return const_cast<Sequence<T>&>(*this).begin(); }
		iterator end() { return m_ctnr.end(); }
		const_iterator end() const { return const_cast<Sequence<T>&>(*this).end(); }

		class const_reverse_iterator
		{
		public:
			const_reverse_iterator(typename ctnr_type::reverse_iterator const& it) : m_it{ it } {}
			const_reverse_iterator(const const_reverse_iterator& other) : m_it{ other.m_it } {}

			T* operator*() const noexcept { return m_it->get(); }
			T* operator->() const noexcept { return m_it->get(); }

			const_reverse_iterator& operator++() noexcept { ++m_it; return *this; }
			const_reverse_iterator operator++(int) noexcept
			{
				const_reverse_iterator tmp = *this;
				operator++();
				return tmp;
			}

			friend bool operator==(const const_reverse_iterator& lhs, const const_reverse_iterator& rhs) noexcept { return lhs.m_it == rhs.m_it; }
			friend bool operator!=(const const_reverse_iterator& lhs, const const_reverse_iterator& rhs) noexcept { return !(lhs == rhs); }

		private:
			typename ctnr_type::reverse_iterator m_it;
		};
		class reverse_iterator : public const_reverse_iterator
		{
		public:
			reverse_iterator(typename ctnr_type::reverse_iterator const& it) : const_reverse_iterator{ it } {}
			reverse_iterator(const reverse_iterator& other) : const_iterator{ other } {}

			T* operator*() noexcept { return this->m_it->get(); }
			T* operator->() noexcept { return this->m_it->get(); }

			reverse_iterator& operator++() noexcept { const_reverse_iterator::operator++(); return *this; }
			reverse_iterator operator++(int) noexcept
			{
				reverse_iterator tmp = *this;
				const_reverse_iterator::operator++();
				return tmp;
			}
		};
		reverse_iterator rbegin() { return m_ctnr.rbegin(); }
		const_reverse_iterator rbegin() const { return const_cast<Sequence<T>&>(*this).rbegin(); }
		reverse_iterator rend() { return m_ctnr.rend(); }
		const_reverse_iterator rend() const { return const_cast<Sequence<T>&>(*this).rend(); }

		T* at(int i) const
		{ 
			assert(i >= 0 && (size_t)i < m_ctnr.size());
			return m_ctnr[i].get(); 
		}

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
			if (m_ctnr.size() > (size_t)std::numeric_limits<int>::max())
				throw std::range_error("Never going to happen");
			return m_ctnr.size();
		}
		void clear()
		{
			while (!m_ctnr.empty())
				erase(m_ctnr.size() - 1);
		}

	private:
		ctnr_type m_ctnr;
	};
}
