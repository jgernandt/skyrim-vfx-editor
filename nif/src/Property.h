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
	template<typename T> class Property;

	template<typename T>
	struct Event<Property<T>>
	{
		const T& value;
	};

	template<typename T>
	class IListener<Property<T>>
	{
	public:
		virtual ~IListener() = default;

		void receive(const Event<nif::Property<T>>& e, Observable<nif::Property<T>>&)
		{ 
			onSet(e.value); 
		}
		virtual void onSet(const T&) {}
	};

	template<typename T> using PropertyListener = IListener<Property<T>>;

	template<typename T>
	class Property final : public Observable<Property<T>>
	{
	public:
		using value_type = T;

	public:
		Property(const T& val = T()) : m_value{ val } {}
		Property(const Property<T>&) = delete;
		Property(Property<T>&& other) noexcept { *this = std::move(other); }

		~Property() = default;

		Property<T>& operator=(const Property<T>&) = delete;
		Property<T>& operator=(Property<T>&& other) noexcept
		{
			static_assert(std::is_nothrow_move_assignable<T>::value);

			Observable<Property<T>>::operator=(std::move(other));
			m_value = std::move(other.m_value);
			return *this;
		}

		T get() const
		{
			return m_value;
		}
		void set(const T& val)
		{
			if (val != m_value) {
				m_value = val;
				this->signal(Event<Property<T>>{ m_value });
			}
		}
		void set(T&& val)
		{
			if (val != m_value) {
				m_value = std::move(val);
				this->signal(Event<Property<T>>{ m_value });
			}
			else
				//Disard val? Inconsistent otherwise?
				T tmp = std::move(val);
		}

	private:
		T m_value{ T() };
	};
}
