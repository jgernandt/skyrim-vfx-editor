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
#include "Observable.h"

namespace nif
{
	template<typename T> class Vector;

	template<typename T>
	struct Event<Vector<T>>
	{
		enum {
			INSERT,
			ERASE,
		} type{ INSERT };
		int pos{ -1 };
	};

	template<typename T>
	class IListener<Vector<T>>
	{
	public:
		virtual ~IListener() = default;

		void receive(const Event<nif::Vector<T>>&e, Observable<nif::Vector<T>>&)
		{
			switch (e.type) {
			case Event<nif::Vector<T>>::INSERT:
				onInsert(e.pos);
				break;
			case Event<nif::Vector<T>>::ERASE:
				onErase(e.pos);
				break;
			}
		}

		//TODO: use iterators instead of ints
		virtual void onInsert(int) {}
		virtual void onErase(int) {}
	};
	template<typename T> using VectorListener = IListener<Vector<T>>;

	template<typename T>
	class Vector final : public Observable<Vector<T>>
	{
		using ctnr_type = std::vector<T>;

	public:
		using element_type = T;

	public:
		Vector() = default;
		~Vector() { clear(); }

		using iterator = typename ctnr_type::iterator;
		using const_iterator = typename ctnr_type::const_iterator;
		iterator begin() { return m_ctnr.begin(); }
		const_iterator begin() const { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }
		const_iterator end() const { return m_ctnr.end(); }

		T& at(int i) 
		{
			assert(i >= 0 && (size_t)i < size());
			return m_ctnr[i]; 
		}
		const T& at(int i) const 
		{
			assert(i >= 0 && (size_t)i < size());
			return m_ctnr[i]; 
		}

		T& back() { return m_ctnr.back(); }
		const T& back() const { return m_ctnr.back(); }
		T& front() { return m_ctnr.front(); }
		const T& front() const { return m_ctnr.front(); }

		//TODO: use iterators instead of ints
		//We could template insertions to forward ctor arguments
		void insert(int i)
		{
			assert(i >= 0 && (size_t)i <= m_ctnr.size());

			m_ctnr.emplace(m_ctnr.begin() + i);

			this->signal(Event<Vector<T>>{ Event<Vector<T>>::INSERT, i });
		}
		void erase(int i)
		{
			assert(i >= 0 && (size_t)i < m_ctnr.size());

			m_ctnr.erase(m_ctnr.begin() + i);

			this->signal(Event<Vector<T>>{ Event<Vector<T>>::ERASE, i });
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

		friend constexpr bool operator==(const Vector<T>& lhs, const Vector<T>& rhs)
		{ 
			return lhs.m_ctnr == rhs.m_ctnr; 
		}
		friend constexpr bool operator==(const Vector<T>& lhs, const std::vector<T>& rhs)
		{
			return lhs.m_ctnr == rhs;
		}
		friend constexpr bool operator==(const std::vector<T>& lhs, const Vector<T>& rhs)
		{
			return rhs == lhs;
		}

	private:
		ctnr_type m_ctnr;
	};
}
