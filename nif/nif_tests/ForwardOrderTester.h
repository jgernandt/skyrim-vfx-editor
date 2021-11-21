#pragma once
#include "nif.h"

namespace common
{
	using namespace nif;

	//Tests the order in which subnodes are traversed
	template<typename T>
	struct ForwardOrderTester : nif::VerticalTraverser<T, ForwardOrderTester>
	{
		void operator() (const T&, std::vector<nif::NiObject*>::const_iterator) { Assert::Fail(); }
	};

	template<>
	struct ForwardOrderTester<NiObject> : nif::VerticalTraverser<NiObject, ForwardOrderTester>
	{
		void operator() (const NiObject&, std::vector<nif::NiObject*>::const_iterator) {}
	};
}
