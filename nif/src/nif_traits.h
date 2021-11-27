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
#include "nif_backend.h"
#include "nif_types.h"
#include "traits.h"

template<>
struct util::array_traits<Niflib::Vector3>
{
	using array_type = Niflib::Vector3;
	using element_type = float;
	constexpr static size_t size = 3;
	static element_type& at(array_type& t, size_t i)
	{
		assert(i >= 0 && i < 3);
		return t[i];
	}
};

template<>
struct util::array_traits<Niflib::Vector4>
{
	using array_type = Niflib::Vector4;
	using element_type = float;
	constexpr static size_t size = 4;
	static element_type& at(array_type& t, size_t i)
	{
		assert(i >= 0 && i < 4);
		return t[i];
	}
};

template<>
struct util::array_traits<Niflib::Color4>
{
	using array_type = Niflib::Color4;
	using element_type = float;
	constexpr static size_t size = 4;
	static element_type& at(array_type& t, size_t i)
	{
		static_assert(offsetof(Niflib::Color4, r) == 0);
		static_assert(offsetof(Niflib::Color4, g) == 4);
		static_assert(offsetof(Niflib::Color4, b) == 8);
		static_assert(offsetof(Niflib::Color4, a) == 12);

		assert(i >= 0 && i < 4);
		return *(reinterpret_cast<float*>(&t) + i);
	}
};

template<>
struct util::colour_traits<Niflib::Color4>
{
	using colour_type = Niflib::Color4;
	using component_type = float;
	constexpr static size_t channels = 4;
	constexpr static bool is_array = true;//are the channels laid out contiguously in memory?
	constexpr static component_type& R(colour_type& col) { return col.r; }
	constexpr static component_type& G(colour_type& col) { return col.g; }
	constexpr static component_type& B(colour_type& col) { return col.b; }
	constexpr static component_type& A(colour_type& col) { return col.a; }
};
