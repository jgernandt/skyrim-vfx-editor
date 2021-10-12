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
#include <string>
#include "type_conversion.h"
#include "traits.h"

struct ImVec2;
struct ImVec4;

namespace gui
{
	template<size_t N>
	using Floats = std::array<float, N>;

	template<size_t N>
	using Ints = std::array<int, N>;

	using ColRGB = Floats<3>;
	using ColRGBA = Floats<4>;

	template<typename T>
	struct GuiConverter
	{
		template<typename From>
		static T convert(From&& f) { return util::DefaultConverter<T>::convert(std::forward<From>(f)); }
	};

	template<typename T>
	using gui_type_conversion = util::type_conversion<T, GuiConverter<T>>;

	template<>
	struct GuiConverter<std::array<float, 2>>
	{
		static std::array<float, 2> convert(const ImVec2& f);
	};
	template<>
	struct GuiConverter<std::array<float, 4>>
	{
		static std::array<float, 4> convert(const ImVec4& f);
	};

	template<>
	struct GuiConverter<ImVec2>
	{
		static ImVec2 convert(const std::array<float, 2>& f);
	};
	template<>
	struct GuiConverter<ImVec4>
	{
		static ImVec4 convert(const std::array<float, 4>& f);
	};
}


//The downside of this design; we need to declare types from dependent or completely unrelated libraries here.
//We might get away with it so far, unless gui ends up needing to convert to array<float, 3>.
/*namespace Niflib
{
	struct Vector3;
	struct Matrix33;
	struct Quaternion;
}*/

/*template<>
struct util::convert_to<std::array<float, 2>>
{
	using type = std::array<float, 2>;

	constexpr static type& from(type& t) { return t; }
	constexpr static const type& from(const type& t) { return t; }
	constexpr static type&& from(type&& t) { return std::move(t); }

	static type from(const ImVec2& f);

	template<typename From>
	static type from(From f)
	{
		if constexpr (std::is_convertible<From, type>::value)
			return f;
		else
			static_assert(false, "type conversion not defined");
	}
};
template<>
struct util::convert_to<std::array<float, 4>>
{
	using type = std::array<float, 4>;

	constexpr static type& from(type& t) { return t; }
	constexpr static const type& from(const type& t) { return t; }
	constexpr static type&& from(type&& t) { return std::move(t); }

	static type from(const ImVec4& f);

	template<typename From>
	static type from(From f)
	{
		if constexpr (std::is_convertible<From, type>::value)
			return f;
		else
			static_assert(false, "type conversion not defined");
	}
};

template<>
struct util::convert_to<ImVec2>
{
	using type = ImVec2;

	constexpr static type& from(type& t) { return t; }
	constexpr static const type& from(const type& t) { return t; }
	constexpr static type&& from(type&& t) { return std::move(t); }

	static type from(const std::array<float, 2>& f);

	template<typename From>
	static type from(From f)
	{
		if constexpr (std::is_convertible<From, type>::value)
			return f;
		else
			static_assert(false, "type conversion not defined");
	}
};

template<>
struct util::convert_to<ImVec4>
{
	using type = ImVec4;

	constexpr static type& from(type& t) { return t; }
	constexpr static const type& from(const type& t) { return t; }
	constexpr static type&& from(type&& t) { return std::move(t); }

	static type from(const std::array<float, 4>& f);

	template<typename From>
	static type from(From f)
	{
		if constexpr (std::is_convertible<From, type>::value)
			return f;
		else
			static_assert(false, "type conversion not defined");
	}
};*/

template<typename T>
struct util::colour_traits<std::array<T, 3>>
{
	using colour_type = std::array<T, 3>;
	using component_type = T;
	constexpr static size_t channels = 3;
	constexpr static bool is_array = true;
	constexpr static component_type& R(colour_type& t) { return t[0]; }
	constexpr static const component_type& R(const colour_type& t) { return t[0]; }
	constexpr static component_type& G(colour_type& t) { return t[1]; }
	constexpr static const component_type& G(const colour_type& t) { return t[1]; }
	constexpr static component_type& B(colour_type& t) { return t[2]; }
	constexpr static const component_type& B(const colour_type& t) { return t[2]; }
	//This works in a partial specialisation only. Would it get weird if we left it out?
	//We do want it to be a compilation error to access the alpha channel of an RGB colour.
	constexpr static component_type& A(const colour_type&) { static_assert(false, "colour has no alpha channel"); }
};

template<typename T>
struct util::colour_traits<std::array<T, 4>>
{
	using colour_type = std::array<T, 4>;
	using component_type = T;
	constexpr static size_t channels = 4;
	constexpr static bool is_array = true;
	constexpr static component_type& R(colour_type& t) { return t[0]; }
	constexpr static const component_type& R(const colour_type& t) { return t[0]; }
	constexpr static component_type& G(colour_type& t) { return t[1]; }
	constexpr static const component_type& G(const colour_type& t) { return t[1]; }
	constexpr static component_type& B(colour_type& t) { return t[2]; }
	constexpr static const component_type& B(const colour_type& t) { return t[2]; }
	constexpr static component_type& A(colour_type& t) { return t[3]; }
	constexpr static const component_type& A(const colour_type& t) { return t[3]; }
};