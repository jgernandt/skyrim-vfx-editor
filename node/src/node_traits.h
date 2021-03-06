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
#include "nif_data.h"
#include "traits.h"

template<typename T>
struct util::property_traits<T*>
{
	using property_type = T*;
	using value_type = T;
	using get_type = T;

	static T get(T* p) { return *p; }
	static void set(T* p, const T& data) { *p = data; }
	static void set(T* p, T&& data) { *p = std::move(data); }
};

template<typename T>
struct util::property_traits<nif::ni_ptr<nif::Property<T>>>
{
	using property_type = nif::ni_ptr<nif::Property<T>>;
	using value_type = T;
	using get_type = T;

	static T get(const property_type& p) { return p->get(); }
	static void set(property_type& p, const T& data) { p->set(data); }
	static void set(property_type& p, T&& data) { p->set(std::move(data)); }
};

template<typename T>
struct util::property_traits<nif::Property<T>>
{
	using property_type = nif::Property<T>;
	using value_type = T;
	using get_type = T;

	static T get(const property_type& p) { return p.get(); }
	static void set(property_type& p, const T& data) { p.set(data); }
	static void set(property_type& p, T&& data) { p.set(std::move(data)); }
};
