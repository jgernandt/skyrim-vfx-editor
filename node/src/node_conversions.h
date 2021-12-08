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
#include "type_conversion.h"
#include "nif_types.h"
#include "Angles.h"

namespace node
{
	template<typename T>
	struct Converter
	{
		template<typename From>
		static T convert(From&& f) { return util::DefaultConverter<T>::convert(f); }
	};

	template<>
	struct Converter<float>
	{
		constexpr static float convert(math::degf deg) { return deg.value; }
	};

	template<>
	struct Converter<math::degf>
	{
		constexpr static math::degf convert(float deg) { return math::degf{ deg }; }
	};

	template<>
	struct Converter<std::array<int, 2>>
	{
		static std::array<int, 2> convert(const std::vector<nif::SubtextureOffset>& offsets);
	};

	template<>
	struct Converter<std::vector<nif::SubtextureOffset>>
	{
		static std::vector<nif::SubtextureOffset> convert(const std::array<int, 2>& count);
	};
}

template<typename T>
using node_conversion = util::type_conversion<T, node::Converter<T>>;
