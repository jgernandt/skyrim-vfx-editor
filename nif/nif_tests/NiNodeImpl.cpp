#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void common::EquivalenceTester<nif::NiNode>::operator()(const NiNode& object, const Niflib::NiNode* native, File& file)
{
	auto&& children = native->GetChildren();
	Assert::IsTrue(children.size() == object.children.size());
	for (auto&& child : children)
		Assert::IsTrue(object.children.has(file.get<NiAVObject>(child).get()));
}

void common::Randomiser<NiNode>::operator()(NiNode& object, File& file, std::mt19937& rng)
{
	randomiseSet(object.children, file, rng);
}

void common::Randomiser<NiNode>::operator()(const NiNode&, Niflib::NiNode* native, File&, std::mt19937& rng)
{
	native->ClearChildren();
	for (auto&& obj : randomObjVector<NiAVObject>(rng))
		native->AddChild(obj);
}
