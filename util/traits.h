//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include <array>

namespace util
{
    template<typename T>
    struct array_traits
    {
        using array_type = T;
        using element_type = T;
        constexpr static size_t size = 1;
        constexpr static element_type& at(array_type& t, size_t i) { return t; }
    };

    template<typename T, size_t N>
    struct array_traits<std::array<T, N>>
    {
        using array_type = std::array<T, N>;
        using element_type = T;
        constexpr static size_t size = N;
        constexpr static element_type& at(array_type& t, size_t i) { return t[i]; }
        constexpr static const element_type& at(const array_type& t, size_t i) { return t[i]; }
    };

    template<typename T>
    struct colour_traits
    {
        using colour_type = T;
        using component_type = T;
        constexpr static size_t channels = 0;
        constexpr static bool is_array = false;//are the channels laid out contiguously in memory?
        constexpr static component_type& R(colour_type&) { static_assert(false, "not a colour type"); }
        constexpr static component_type& G(colour_type&) { static_assert(false, "not a colour type"); }
        constexpr static component_type& B(colour_type&) { static_assert(false, "not a colour type"); }
        constexpr static component_type& A(colour_type&) { static_assert(false, "not a colour type"); }
    };

    template<typename T>
    struct property_traits
    {
        using property_type = T;
        using value_type = T;
        using get_type = T;

        constexpr static get_type get(const T& t) { return t; }
        static void set(T& t, const value_type& val) { t = val; }
    };

    //defines a type of field property with an arbitrary index, suitable for bit fields or flag sets
    template<typename T>
    struct field_traits
    {
        using field_type = T;
        using index_type = T;
        using value_type = T;

        constexpr static value_type get(const T& t, index_type) { return t; }
        static void set(T& t, index_type, const value_type& val) { t = val; }
    };
}