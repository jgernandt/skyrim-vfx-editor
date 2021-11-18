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
#include "traits.h"

namespace nif
{
	template<typename T> class FlagSet;
	template<typename T>
	class IListener<FlagSet<T>>
	{
	public:
		virtual ~IListener() = default;
		virtual void onSet(T) {}
		virtual void onClear(T) {}
	};

	template<typename T> using FlagSetListener = IListener<FlagSet<T>>;

	template<typename T>
	class FlagSet final : public Observable<FlagSet<T>>
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value);
	public:
		FlagSet(T val = T()) : m_flags{ val } {}
		~FlagSet()
		{
			if (m_flags != T(0)) {
				for (FlagSetListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onClear(m_flags);
				}
			}
		}

		T get() const { return m_flags; }

		void set(T flags) 
		{
			if (T to_set = (flags & ~(m_flags & flags)); to_set != T(0)) {
				//one or more flags will be set, but not necessarily every flag that was passed
				m_flags |= flags;
				for (FlagSetListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onSet(to_set);
				}
			}
		}
		void clear(T flags)
		{
			if (T to_clear = (m_flags & flags); to_clear != T(0)) {
				m_flags &= ~flags;
				for (FlagSetListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onClear(to_clear);
				}
			}
		}
		bool isSet(T flags) const 
		{ 
			return (m_flags & flags) == flags;
		}

	private:
		T m_flags;
	};
}

template<typename T>
struct util::field_traits<nif::FlagSet<T>>
{
	using field_type = nif::FlagSet<T>;
	using index_type = T;
	using value_type = bool;

	static bool get(const field_type& t, index_type i) { return t.isSet(i); }
	static void set(field_type& t, index_type i, bool val) { t.set(i, val); }
};
