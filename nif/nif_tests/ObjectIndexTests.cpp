#include "pch.h"
#include "CppUnitTest.h"

namespace nif_tests
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	TEST_CLASS(ObjectIndexTests)
	{
	public:
		nif::File file{ nif::File::Version::SKYRIM_SE };

		//File should correctly map the objects it creates
		TEST_METHOD(Consistency)
		{
			Niflib::Ref<Niflib::NiObject> native0 = new Niflib::BSFadeNode;
			auto object0 = file.get<NiObject>(native0);

			auto native1 = file.get<Niflib::NiObject>(object0.get());
			Assert::IsTrue(native0 == native1);

			auto object1 = file.get<BSFadeNode>(Niflib::StaticCast<Niflib::BSFadeNode>(native0));
			Assert::IsTrue(object0 == object1);
		}

		//Unused objects should be deleted
		TEST_METHOD(Cleanup)
		{
			Niflib::Ref<Niflib::NiObject> ref = new Niflib::BSFadeNode;
			auto shared = file.get<NiObject>(ref);

			std::weak_ptr<NiObject> weak = shared;
			Niflib::NiObject* ptr = ref;

			//Clearing the Ref should not destroy anything
			ref = nullptr;
			Assert::IsTrue(ptr->GetNumRefs() != 0);//could give a false positive

			//this should destroy the objects
			shared.reset();
			Assert::IsTrue(weak.expired());
			//How to reliably test for destruction of the niflib object?
		}
	};
}
