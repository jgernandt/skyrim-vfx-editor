#pragma once
#include "nif.h"

namespace common
{
	using namespace nif;

	//Tests the order in which subnodes are traversed
	template<typename T>
	struct ForwardOrderTester : nif::VerticalTraverser<T, ForwardOrderTester>
	{
		void operator() (
			const T& object, 
			std::vector<nif::NiObject*>::iterator& it, 
			std::vector<nif::NiObject*>::iterator end) 
		{ Assert::Fail(); }
	};

	template<>
	struct ForwardOrderTester<NiObject> : nif::VerticalTraverser<NiObject, ForwardOrderTester>
	{
		void operator() (const NiObject& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiObjectNET> : nif::VerticalTraverser<NiObjectNET, ForwardOrderTester>
	{
		void operator() (
			const NiObjectNET& object, 
			std::vector<nif::NiObject*>::iterator& it,
			const std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiAVObject> : nif::VerticalTraverser<NiAVObject, ForwardOrderTester>
	{
		void operator() (const NiAVObject&, std::vector<nif::NiObject*>::iterator&, std::vector<nif::NiObject*>::iterator) {}
	};
}
