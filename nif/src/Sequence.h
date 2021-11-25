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
#include <memory>
#include <vector>
#include "Observable.h"

namespace nif
{
	template<typename T> inline bool operator==(const std::shared_ptr<T>& lhs, T* rhs) noexcept { return lhs.get() == rhs; }
	template<typename T> inline bool operator==(T* lhs, const std::shared_ptr<T>& rhs) noexcept { return rhs == lhs; }

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
		using ref_type = T;

	public:
		Sequence() = default;
		~Sequence() { clear(); }

		using const_iterator = typename ctnr_type::const_iterator;
		using iterator = typename ctnr_type::iterator;
		iterator begin() { return m_ctnr.begin(); }
		const_iterator begin() const { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }
		const_iterator end() const { return m_ctnr.end(); }

		using const_reverse_iterator = typename ctnr_type::const_reverse_iterator;
		using reverse_iterator = typename ctnr_type::reverse_iterator;
		reverse_iterator rbegin() { return m_ctnr.rbegin(); }
		const_reverse_iterator rbegin() const { return m_ctnr.rbegin(); }
		reverse_iterator rend() { return m_ctnr.rend(); }
		const_reverse_iterator rend() const { return m_ctnr.rend(); }

		const std::shared_ptr<T>& at(int i) const
		{ 
			assert(i >= 0 && (size_t)i < m_ctnr.size());
			return m_ctnr[i]; 
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
