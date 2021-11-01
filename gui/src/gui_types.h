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
#include <string>
#include "type_conversion.h"
#include "traits.h"
#include "Eigen/Core"

struct ImVec2;
struct ImVec4;

namespace gui
{
	template<size_t N>
	using Floats = Eigen::Array<float, N, 1>;

	template<size_t N>
	using Ints = Eigen::Array<int, N, 1>;

	using ColRGB = std::array<float, 3>;
	using ColRGBA = std::array<float, 4>;

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
	struct GuiConverter<Eigen::Array<float, 2, 1>>
	{
		static Eigen::Array<float, 2, 1> convert(const ImVec2& f);
	};
	template<>
	struct GuiConverter<Eigen::Array<float, 4, 1>>
	{
		static Eigen::Array<float, 4, 1> convert(const ImVec4& f);
	};

	template<>
	struct GuiConverter<Eigen::Vector<float, 2>>
	{
		static Eigen::Vector<float, 2> convert(const ImVec2& f);
	};
	template<>
	struct GuiConverter<Eigen::Vector<float, 4>>
	{
		static Eigen::Vector<float, 4> convert(const ImVec4& f);
	};

	template<>
	struct GuiConverter<ImVec2>
	{
		static ImVec2 convert(const std::array<float, 2>& f);
		static ImVec2 convert(const Eigen::Array<float, 2, 1>& f);
		static ImVec2 convert(const Eigen::Vector<float, 2>& f);
	};
	template<>
	struct GuiConverter<ImVec4>
	{
		static ImVec4 convert(const std::array<float, 4>& f);
		static ImVec4 convert(const Eigen::Array<float, 4, 1>& f);
		static ImVec4 convert(const Eigen::Vector<float, 4>& f);
	};
}

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