#pragma once
#include "CppUnitTest.h"
#include "nif.h"

namespace common
{
	using namespace nif;

	//Test an object and a native for equivalence in all fields
	template<typename T>
	struct EquivalenceTester : VerticalTraverser<T, EquivalenceTester>
	{
		void operator() (const T& object, const typename type_map<T>::type* native, File& file) {}
	};

	template<>
	struct EquivalenceTester<NiObjectNET> : VerticalTraverser<NiObjectNET, EquivalenceTester>
	{
		void operator() (const NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};
}