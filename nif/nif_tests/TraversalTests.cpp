#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace traversal
{
	template<typename T>
	struct TraversalTester : nif::VerticalTraverser<T, TraversalTester>
	{
		void operator() (T&, std::list<size_t>& types)
		{
			types.push_back(T::TYPE);
		}
	};

	struct ForwarderTester : nif::NiTraverser
	{
		ForwarderTester(std::vector<std::pair<nif::ni_type, nif::NiObject*>>& tr) : 
			m_traversed{ tr } {}

		virtual void traverse(nif::NiObject& obj) { m_traversed.push_back({ nif::NiObject::TYPE, &obj }); }
		virtual void traverse(nif::NiObjectNET& obj) { m_traversed.push_back({ nif::NiObjectNET::TYPE, &obj }); }
		//etc (we really need a macro or template for this...)

		std::vector<std::pair<nif::ni_type, nif::NiObject*>>& m_traversed;
	};

	//Tests forwarding behaviour from each part independently
	TEST_CLASS(Forwarder)
	{
	public:

		TEST_METHOD(NiObjectNET)
		{
			nif::NiObjectNET obj;

			auto data0 = std::make_shared<nif::NiStringExtraData>();
			auto data1 = std::make_shared<nif::NiExtraData>();
			obj.extraData.add(data0);
			obj.extraData.add(data1);

			auto ctlr0 = std::make_shared<nif::NiPSysEmitterCtlr>();
			auto ctlr1 = std::make_shared<nif::NiTimeController>();
			obj.controllers.insert(0, ctlr0);
			obj.controllers.insert(1, ctlr1);

			std::vector<std::pair<nif::ni_type, nif::NiObject*>> traversed;
			ForwarderTester t(traversed);
			nif::Forwarder<nif::NiObjectNET>{}(obj, t);

			//Iteration order of Set is not specified, so there are two valid outcomoes
			std::vector<std::pair<nif::ni_type, nif::NiObject*>> expected1 =
				{ { nif::NiStringExtraData::TYPE, data0.get() }, { nif::NiExtraData::TYPE, data1.get() },
				{ nif::NiPSysEmitterCtlr::TYPE, ctlr0.get() }, { nif::NiTimeController::TYPE, ctlr1.get() } };

			std::vector<std::pair<nif::ni_type, nif::NiObject*>> expected2 =
				{ { nif::NiExtraData::TYPE, data1.get() }, { nif::NiStringExtraData::TYPE, data0.get() },
				{ nif::NiPSysEmitterCtlr::TYPE, ctlr0.get() }, { nif::NiTimeController::TYPE, ctlr1.get() } };

			Assert::IsTrue(traversed == expected1 || traversed == expected2);
		}

	};

	TEST_CLASS(VerticalTraverser)
	{
	public:

		//We either need tests for only the leaves in the inheritance graph
		// or tests that check only the first/last two visited parts.
		//The latter is probably better. Easier to extend.

		TEST_METHOD(NiNode)
		{
			nif::NiNode node;
			std::list<size_t> types;

			TraversalTester<nif::NiNode>{}.down(node, types);
			std::list<nif::ni_type> expected{ nif::NiObject::TYPE, nif::NiObjectNET::TYPE, nif::NiAVObject::TYPE, nif::NiNode::TYPE };
			Assert::IsTrue(types == expected);

			types.clear();
			TraversalTester<nif::NiNode>{}.up(node, types);
			expected.reverse();
			Assert::IsTrue(types == expected);
		}

	};
}
