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
	class IListener<Vector<T>>
	{
	public:
		virtual ~IListener() = default;

		//TODO: use iterators instead of ints
		virtual void onInsert(int) {}
		virtual void onErase(int) {}
	};
	template<typename T> using VectorListener = IListener<Vector<T>>;

	template<typename T>
	class Vector final : public Observable<Vector<T>>
	{
	public:
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;

	public:
		Vector() = default;
		~Vector() = default;

		//We use these to iterate through our container during pre-write sync.
		//They should be changed to dereference to get/set, not the raw element.
		using iterator = typename std::vector<T>::iterator;
		iterator begin() { return m_ctnr.begin(); }
		iterator end() { return m_ctnr.end(); }

		//Is this inconsistency (not calling vector::at) going to be a problem? Probably, right?
		T& at(int i) { return m_ctnr[i]; }
		const T& at(int i) const { return m_ctnr[i]; }

		//TODO: use iterators instead of ints
		int insert(int i, const T& val)
		{
			assert(i >= 0);

			typename std::vector<T>::iterator it;
			if ((size_t)i < m_ctnr.size())
				it = m_ctnr.begin() + i;
			else {
				it = m_ctnr.end();
				i = m_ctnr.size();
			}

			m_ctnr.insert(it, val);
			for (VectorListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onInsert(i);
			}

			return i;
		}
		int erase(int i)
		{
			assert(i >= 0 && (size_t)i < m_ctnr.size());

			m_ctnr.erase(m_ctnr.begin() + i);

			for (VectorListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onErase(i);
			}

			return i;
		}

		void push_back(const T& val)
		{

		}

		size_t size() const
		{
			return m_ctnr.size();
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
		std::vector<T> m_ctnr;
	};
}
