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
	struct Event<FlagSet<T>>
	{
		enum {
			RAISE,
			CLEAR,
		} type{ RAISE };
		T flags{ T() };
	};

	template<typename T>
	class IListener<FlagSet<T>>
	{
	public:
		virtual ~IListener() = default;
		void receive(const Event<FlagSet<T>>&e, Observable<FlagSet<T>>&)
		{ 
			switch (e.type) {
			case Event<FlagSet<T>>::RAISE:
				onRaise(e.flags);
				break;
			case Event<FlagSet<T>>::CLEAR:
				onClear(e.flags);
				break;
			}
		}
		virtual void onRaise(T) {}
		virtual void onClear(T) {}
	};

	template<typename T> using FlagSetListener = IListener<FlagSet<T>>;

	template<typename T>
	class FlagSet final : public Observable<FlagSet<T>>
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value);
	public:
		using flag_type = T;

	public:
		FlagSet(T val = T()) : m_flags{ val } {}
		~FlagSet()
		{
			clear(m_flags);
		}

		T raised() const { return m_flags; }

		void raise(T flags) 
		{
			if constexpr (std::is_enum<T>::value) {
				//We need to do bitmanipulations on the underlying type
				using E = typename std::underlying_type<T>::type;
				if (E to_set = ((E)flags & ~((E)m_flags & (E)flags)); to_set != E(0)) {
					//one or more flags will be set, but not necessarily every flag that was passed
					m_flags = (T)((E)m_flags | (E)flags);
					this->signal(Event<FlagSet<T>>{ Event<FlagSet<T>>::RAISE, (T)to_set });
				}
			}
			else {
				if (T to_set = (flags & ~(m_flags & flags)); to_set != T(0)) {
					//one or more flags will be set, but not necessarily every flag that was passed
					m_flags |= flags;
					this->signal(Event<FlagSet<T>>{ Event<FlagSet<T>>::RAISE, to_set });
				}
			}
		}
		void clear() { clear(m_flags); }
		void clear(T flags)
		{
			if constexpr (std::is_enum<T>::value) {
				//We need to do bitmanipulations on the underlying type
				using E = typename std::underlying_type<T>::type;
				if (E to_clear = ((E)m_flags & (E)flags); to_clear != E(0)) {
					m_flags = (T)(~(E)flags);
					this->signal(Event<FlagSet<T>>{ Event<FlagSet<T>>::CLEAR, (T)to_clear });
				}
			}
			else {
				if (T to_clear = (m_flags & flags); to_clear != T(0)) {
					m_flags &= ~flags;
					this->signal(Event<FlagSet<T>>{ Event<FlagSet<T>>::CLEAR, to_clear });
				}
			}
		}
		bool hasRaised(T flags) const 
		{ 
			return (m_flags & flags) == flags;
		}

	private:
		T m_flags;
	};
}
