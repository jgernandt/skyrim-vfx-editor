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
#include "nif_types.h"
#include "Assignable.h"
#include "ListProperty.h"
#include "Property.h"
#include "Sequence.h"
#include "Set.h"

namespace nif
{
	//Really just a Set, but intended for int or enum types
	template<typename T>
	class FlagSet
	{
	public:
		virtual ~FlagSet() = default;

		virtual void set(T, bool) = 0;
		virtual bool isSet(T) const = 0;
	};

	class Transformable
	{
	public:
		virtual ~Transformable() = default;

		virtual IProperty<translation_t>& translation() = 0;
		virtual IProperty<rotation_t>& rotation() = 0;
		virtual IProperty<scale_t>& scale() = 0;
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