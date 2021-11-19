#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiNode>::operator()(NiNode& object, Niflib::NiNode* native, File& file)
{
	native_child0 = new Niflib::NiAVObject;
	native_child1 = new Niflib::NiAVObject;
	native_child2 = new Niflib::NiAVObject;
	child0 = file.get<NiAVObject>(native_child0);
	child1 = file.get<NiAVObject>(native_child1);
	child2 = file.get<NiAVObject>(native_child2);

	native->AddChild(native_child0);
	native->AddChild(native_child1);
}

void nif_tests::SyncTestTraverser<NiNode>::operator()(NiNode& object, Niflib::NiNode* native, File& file, int)
{
	Assert::IsTrue(object.children.size() == 2);
	Assert::IsTrue(object.children.has(child0.get()));
	Assert::IsTrue(object.children.has(child1.get()));

	object.children.add(child2);
}

void nif_tests::SyncTestTraverser<NiNode>::operator()(NiNode& object, Niflib::NiNode* native, File& file, int, int)
{
	Assert::IsTrue(native->GetChildren().size() == 3);
}
