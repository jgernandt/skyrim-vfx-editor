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
#include <utility>

namespace util
{
	/*This requires every conversion to be declared in one place, which is too limiting
	template<typename To>
	struct convert_to
	{
		using type = To;

		constexpr static type& from(type& t) { return t; }
		constexpr static const type& from(const type& t) { return t; }
		constexpr static type&& from(type&& t) { return std::move(t); }

		template<typename From>
		static type from(From f) 
		{ 
			if constexpr (std::is_convertible<From, To>::value)
				return f;
			else
				static_assert(false, "type conversion not defined"); 
		}
	};*/

	//Another approach, which delegates the actual conversions to a separate template.
	//This allows new conversions to be introduced, by defining a new type that implements convert.
	template<typename To>
	struct DefaultConverter
	{
		template<typename From>
		static To convert(From&& f)
		{
			if constexpr (std::is_convertible<decltype(f), To>::value)
				return f;
			else
				static_assert(false, "type conversion not defined");
		}
	};


	template<typename To, typename Converter = DefaultConverter<To>>
	struct type_conversion
	{
		constexpr static To& from(To& t) { return t; }
		constexpr static const To& from(const To& t) { return t; }
		constexpr static To&& from(To&& t) { return std::move(t); }

		template<typename From>
		static To from(From&& f) { return Converter::convert(std::forward<From>(f)); }
	};

}