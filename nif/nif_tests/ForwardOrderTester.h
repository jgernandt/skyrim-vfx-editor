#pragma once
#include "CppUnitTest.h"
#include "nif.h"

namespace common
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	template<typename T>
	void fwdRef(const Ref<T>& ass, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
	{
		if (auto&& obj = ass.assigned()) {
			Assert::IsTrue(it != end);
			Assert::IsTrue(*it == obj.get());
			++it;
		}
	}

	template<typename T>
	void fwdSequence(const Sequence<T>& seq, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
	{
		//Sequence should be traversed from first to last
		for (auto&& item : seq) {
			Assert::IsTrue(it != end);
			Assert::IsTrue(*it == item.get());
			++it;
		}
	}

	template<typename T>
	void fwdSet(const Set<T>& set, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
	{
		//Order of traversal of a Set is not specified
		for (size_t i = 0; i < set.size(); i++) {
			Assert::IsTrue(it != end);
			//We don't need to check the type, we're just looking at the address
			Assert::IsTrue(set.has(static_cast<T*>(*it)));
			++it;
		}
	}

	//Tests the order in which subnodes are traversed
	template<typename T>
	struct ForwardOrderTester : nif::VerticalTraverser<T, ForwardOrderTester>
	{
		bool operator() (const T& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end) { return true; }
	};

	template<>
	struct ForwardOrderTester<NiObject> : nif::VerticalTraverser<NiObject, ForwardOrderTester>
	{
		bool operator() (const NiObject& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiObjectNET> : nif::VerticalTraverser<NiObjectNET, ForwardOrderTester>
	{
		bool operator() (const NiObjectNET& object, std::vector<nif::NiObject*>::iterator& it, const std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiNode> : nif::VerticalTraverser<NiNode, ForwardOrderTester>
	{
		bool operator() (const NiNode& object, std::vector<nif::NiObject*>::iterator& it, const std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiBoolInterpolator> : nif::VerticalTraverser<NiBoolInterpolator, ForwardOrderTester>
	{
		bool operator() (const NiBoolInterpolator& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiFloatInterpolator> : nif::VerticalTraverser<NiFloatInterpolator, ForwardOrderTester>
	{
		bool operator() (const NiFloatInterpolator& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiSingleInterpController> : nif::VerticalTraverser<NiSingleInterpController, ForwardOrderTester>
	{
		bool operator() (const NiSingleInterpController& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiParticleSystem> : nif::VerticalTraverser<NiParticleSystem, ForwardOrderTester>
	{
		bool operator() (const NiParticleSystem& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};

	template<>
	struct ForwardOrderTester<NiPSysEmitterCtlr> : nif::VerticalTraverser<NiPSysEmitterCtlr, ForwardOrderTester>
	{
		bool operator() (const NiPSysEmitterCtlr& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end);
	};
}
