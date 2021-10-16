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
		degrees(T t = T()) : value{ t } {}
		explicit degrees(radians<T> r);
		T value;
		degrees& operator+=(const degrees& r) { value += r.value; return *this; }
		degrees& operator-=(const degrees& r) { value -= r.value; return *this; }
		degrees& operator*=(T r) { value *= r; return *this; }
		degrees& operator/=(T r) { value /= r; return *this; }
		explicit operator radians<T>() const;

		friend bool operator==(const degrees<T>& l, const degrees<T>& r) { return l.value == r.value; }
		friend bool operator!=(const degrees<T>& l, const degrees<T>& r) { return !(l == r); }
	};

	template<typename T>
	struct radians
	{
		radians(T t = T()) : value{ t } {}
		explicit radians(degrees<T> d);
		T value;
		radians& operator+=(const radians& r) { value += r.value; return *this; }
		radians& operator-=(const radians& r) { value -= r.value; return *this; }
		radians& operator*=(T r) { value *= r; return *this; }
		radians& operator/=(T r) { value /= r; return *this; }
		explicit operator degrees<T>() const;

		friend bool operator==(const radians<T>& l, const radians<T>& r) { return l.value == r.value; }
		friend bool operator!=(const radians<T>& l, const radians<T>& r) { return !(l == r); }
	};

	template<typename T> inline degrees<T> operator+(degrees<T> l, degrees<T> r) { return l += r; }
	template<typename T> inline degrees<T> operator-(degrees<T> l, degrees<T> r) { return l -= r; }
	template<typename T> inline degrees<T> operator*(degrees<T> l, T r) { return l *= r; }
	template<typename T> inline degrees<T> operator*(T l, degrees<T> r) { return r * l; }
	template<typename T> inline degrees<T> operator/(degrees<T> l, T r) { return l /= r; }

	template<typename T> inline radians<T> operator+(radians<T> l, radians<T> r) { return l += r; }
	template<typename T> inline radians<T> operator-(radians<T> l, radians<T> r) { return l -= r; }
	template<typename T> inline radians<T> operator*(radians<T> l, T r) { return l *= r; }
	template<typename T> inline radians<T> operator*(T l, radians<T> r) { return r * l; }
	template<typename T> inline radians<T> operator/(radians<T> l, T r) { return l /= r; }

	template<typename T>
	inline degrees<T>::degrees(radians<T> r) : value{ r.value * T(180.0) / pi<T> }	{}

	template<typename T>
	inline degrees<T>::operator radians<T>() const { return radians<T>{ value * pi<T> / T(180.0) }; }

	template<typename T>
	inline math::radians<T>::radians(degrees<T> d) : value{ d.value * pi<T> / T(180.0) } {}

	template<typename T>
	inline math::radians<T>::operator degrees<T>() const { return degrees<T>{ value * T(180.0) / pi<T> }; }

	using deg = degrees<float>;
	using rad = radians<float>;
}