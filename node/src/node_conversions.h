#pragma once
#include "type_conversion.h"
#include "Angles.h"

namespace node
{
	template<typename T>
	struct NodeConverter
	{
		template<typename From>
		static T convert(From&& f) { return util::DefaultConverter<T>::convert(f); }
	};

	template<>
	struct NodeConverter<float>
	{
		constexpr static float convert(math::degf deg) { return deg.value; }
	};

	template<>
	struct NodeConverter<math::degf>
	{
		constexpr static math::degf convert(float deg) { return math::degf{ deg }; }
	};
}
