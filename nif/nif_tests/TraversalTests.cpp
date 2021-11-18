#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nif_tests
{
	using namespace nif;

	template<typename T>
	struct TraversalTester : VerticalTraverser<T, TraversalTester>
	{
		void operator() (T&, std::list<size_t>& types)
		{
			types.push_back(T::TYPE);
		}
	};

	TEST_CLASS(TraversalTests)
	{
	public:

		TEST_METHOD(VerticalTraverser)
		{
			NiNode node;
			std::list<size_t> types;

			TraversalTester<NiNode>{}.down(node, types);
			std::list<size_t> expected{ NiObject::TYPE, NiObjectNET::TYPE, NiAVObject::TYPE, NiNode::TYPE };
			Assert::IsTrue(types == expected);

			types.clear();
			TraversalTester<NiNode>{}.up(node, types);
			expected.reverse();
			Assert::IsTrue(types == expected);
		}

	};
}
