//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "node_concepts.h"
#include "traits.h"

template<typename T>
struct util::property_traits<IProperty<T>>
{
	using property_type = IProperty<T>;
	using value_type = T;
	using get_type = T;

	static T get(const property_type& p) { return p.get(); }
	static void set(property_type& p, const T& data) { p.set(data); }
};
