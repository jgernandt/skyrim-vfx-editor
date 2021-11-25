#pragma once
#include "AVObject.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	template<>
	class Default<AVObject>
	{
	public:
		void setDefaults(NiAVObject& obj)
		{
			obj.flags.raise(14);
		}
	};

}
