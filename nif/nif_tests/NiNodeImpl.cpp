#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include"ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool common::EquivalenceTester<nif::NiNode>::operator()(const NiNode& object, const Niflib::NiNode* native, File& file)
{
	auto&& children = native->GetChildren();
	Assert::IsTrue(children.size() == object.children.size());
	for (auto&& child : children)
		Assert::IsTrue(object.children.has(file.get<NiAVObject>(child).get()));

	return true;
}

bool common::ForwardOrderTester<NiNode>::operator()(const NiNode& object, std::vector<nif::NiObject*>::iterator& it, const std::vector<nif::NiObject*>::iterator end)
{
	fwdSet(object.children, it, end);

	return true;
}

bool common::Randomiser<NiNode>::operator()(NiNode& object, File& file, std::mt19937& rng)
{
	randomiseSet(object.children, file, rng);

	return true;
}

bool common::Randomiser<NiNode>::operator()(const NiNode&, Niflib::NiNode* native, File&, std::mt19937& rng)
{
	native->ClearChildren();
	for (auto&& obj : randomObjVector<NiAVObject>(rng))
		native->AddChild(obj);

	return true;
}
