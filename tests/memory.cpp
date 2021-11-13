#include "pch.h"
#include "CppUnitTest.h"
#include "nodes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace node
{
	TEST_CLASS(MemoryTests)
	{
	public:
		TEST_METHOD(Interface_uniqueness)
		{
			auto file = std::make_unique<nif::File>(nif::File::Version::SKYRIM_SE);

			auto root = file->getRoot();

			Niflib::NiNodeRef rootRef = &root->getNative();
			Niflib::NiNode* nativePtr = rootRef;

			Assert::IsTrue(rootRef->GetNumRefs() == 3);//Ref, interface, file index

			//getting should give us the same interface
			auto node = file->get<nif::NiNode>(nativePtr);
			Assert::IsTrue(node == root);
			Assert::IsTrue(rootRef->GetNumRefs() == 3);//Ref, interface, file index
			//so releasing it does nothing
			node.reset();
			Assert::IsTrue(rootRef->GetNumRefs() == 3);//Ref, interface, file index

			file.reset();
			Assert::IsTrue(rootRef->GetNumRefs() == 2);//Ref, interface

			root.reset();
			Assert::IsTrue(rootRef->GetNumRefs() == 1);//Ref
		}
	};
}