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
#include "constants.h"

namespace math
{
	template<typename T> struct radians;

	template<typename T>
	struct degrees
	{
		T value;

		explicit degrees(T t = T()) : value{ t } {}
		degrees(const degrees<T>&) = default;
		template<typename Y> explicit degrees(degrees<Y> other) : value{ static_cast<T>(other) } {}
		degrees(radians<T> r);

		~degrees() = default;

		degrees<T>& operator=(const degrees<T>&) = default;

		degrees& operator+=(const degrees& r) { value += r.value; return *this; }
		degrees& operator-=(const degrees& r) { value -= r.value; return *this; }
		degrees& operator*=(T r) { value *= r; return *this; }
		degrees& operator/=(T r) { value /= r; return *this; }

		friend constexpr bool operator==(const degrees<T>& lhs, const degrees<T>& rhs) { return lhs.value == rhs.value; }
		friend constexpr bool operator!=(const degrees<T>& lhs, const degrees<T>& rhs) { return !(lhs == rhs); }
		friend constexpr bool operator<(const degrees<T>& lhs, const degrees<T>& rhs) { return lhs.value < rhs.value; }
		friend constexpr bool operator>(const degrees<T>& lhs, const degrees<T>& rhs) { return rhs < lhs; }
		friend constexpr bool operator<=(const degrees<T>& lhs, const degrees<T>& rhs) { return !(rhs < lhs); }
		friend constexpr bool operator>=(const degrees<T>& lhs, const degrees<T>& rhs) { return !(lhs < rhs); }
	};

	template<typename T>
	struct radians
	{
		T value;

		explicit radians(T t = T()) : value{ t } {}
		radians(const radians<T>&) = default;
		template<typename Y> explicit radians(radians<Y> other) : value{ static_cast<T>(other) } {}
		radians(degrees<T> d);

		~radians() = default;

		radians<T>& operator=(const radians<T>&) = default;

		radians& operator+=(const radians& r) { value += r.value; return *this; }
		radians& operator-=(const radians& r) { value -= r.value; return *this; }
		radians& operator*=(T r) { value *= r; return *this; }
		radians& operator/=(T r) { value /= r; return *this; }

		friend constexpr bool operator==(const radians<T>& lhs, const radians<T>& rhs) { return lhs.value == rhs.value; }
		friend constexpr bool operator!=(const radians<T>& lhs, const radians<T>& rhs) { return !(lhs == rhs); }
		friend constexpr bool operator<(const radians<T>& lhs, const radians<T>& rhs) { return lhs.value < rhs.value; }
		friend constexpr bool operator>(const radians<T>& lhs, const radians<T>& rhs) { return rhs < lhs; }
		friend constexpr bool operator<=(const radians<T>& lhs, const radians<T>& rhs) { return !(rhs < lhs); }
		friend constexpr bool operator>=(const radians<T>& lhs, const radians<T>& rhs) { return !(lhs < rhs); }
	};

	template<typename T> constexpr degrees<T> operator+(degrees<T> l, degrees<T> r) { return l += r; }
	template<typename T> constexpr degrees<T> operator-(degrees<T> l, degrees<T> r) { return l -= r; }
	template<typename T> constexpr degrees<T> operator*(degrees<T> l, T r) { return l *= r; }
	template<typename T> constexpr degrees<T> operator*(T l, degrees<T> r) { return r * l; }
	template<typename T> constexpr degrees<T> operator/(degrees<T> l, T r) { return l /= r; }

	template<typename T> constexpr radians<T> operator+(radians<T> l, radians<T> r) { return l += r; }
	template<typename T> constexpr radians<T> operator-(radians<T> l, radians<T> r) { return l -= r; }
	template<typename T> constexpr radians<T> operator*(radians<T> l, T r) { return l *= r; }
	template<typename T> constexpr radians<T> operator*(T l, radians<T> r) { return r * l; }
	template<typename T> constexpr radians<T> operator/(radians<T> l, T r) { return l /= r; }

	template<typename T>
	inline degrees<T>::degrees(radians<T> r) : value{ r.value * T(180.0) / pi<T> } {}

	template<typename T>
	inline radians<T>::radians(degrees<T> d) : value{ d.value * pi<T> / T(180.0) } {}

	using deg = degrees<double>;
	using rad = radians<double>;
	using degf = degrees<float>;
	using radf = radians<float>;
}