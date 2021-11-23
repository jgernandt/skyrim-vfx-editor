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
