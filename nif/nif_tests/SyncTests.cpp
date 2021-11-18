#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nif_tests
{
	using namespace nif;

	TEST_CLASS(SyncTests)
	{
	public:

		template<typename T, template<typename> typename TestType>
		struct NiTestable : TestType<typename T::base_type>
		{
			virtual void run(File& file, NiObject* object, Niflib::NiObject* native)
			{
				TestType<typename T::base_type>::run();
				static_cast<TestType<T>&>(*this).test(
					file, static_cast<T*>(object), static_cast<typename type_map<T>::type*>(native));
			}
		};

		template<typename T>
		struct NiSyncTest
		{
			virtual void run() {}
		};
		template<>
		struct NiSyncTest<NiObjectNET> : NiTestable<NiObjectNET, NiSyncTest>
		{
			void test(File& file, NiObjectNET* object, Niflib::NiObjectNET* native)
			{
				Niflib::Ref<Niflib::NiExtraData> native_data0 = new Niflib::NiExtraData;
				Niflib::Ref<Niflib::NiExtraData> native_data1 = new Niflib::NiExtraData;
				auto data0 = file.get<NiExtraData>(native_data0);
				auto data1 = file.get<NiExtraData>(native_data1);

				Niflib::Ref<Niflib::NiTimeController> native_ctlr0 = new Niflib::NiTimeController;
				Niflib::Ref<Niflib::NiTimeController> native_ctlr1 = new Niflib::NiTimeController;
				auto ctlr0 = file.get<NiTimeController>(native_ctlr0);
				auto ctlr1 = file.get<NiTimeController>(native_ctlr1);

				std::string name = "gnosrihgnjsoeir";
				native->SetName(name);

				native->AddExtraData(native_data0);
				native->AddExtraData(native_data1);

				native->AddController(native_ctlr0);
				native->AddController(native_ctlr1);

				object->receive(NiReadSyncer(file));

				Assert::IsTrue(object->name.get() == name);

				Assert::IsTrue(object->extraData.size() == 2);
				Assert::IsTrue(object->extraData.has(data0.get()));
				Assert::IsTrue(object->extraData.has(data1.get()));

				Assert::IsTrue(object->controllers.size() == 2);
				Assert::IsTrue(object->controllers.find(ctlr0.get()) == 0);
				Assert::IsTrue(object->controllers.find(ctlr1.get()) == 1);

				name = "oeasiugershg";
				object->name.set(name);
				object->extraData.remove(data0.get());
				object->controllers.erase(0);

				object->receive(NiWriteSyncer(file));

				Assert::IsTrue(native->GetName() == name);

				auto extraData = native->GetExtraData();
				Assert::IsTrue(extraData.size() == 1);
				Assert::IsTrue(extraData.front() == native_data1);

				auto controllers = native->GetControllers();
				Assert::IsTrue(controllers.size() == 1);
				Assert::IsTrue(controllers.front() == native_ctlr1);
			}
		};

		TEST_METHOD(_NiObjectNET)
		{
			File file(File::Version::SKYRIM_SE);

			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<NiObjectNET>(native);

			NiSyncTest<NiObjectNET>().run(file, object.get(), native);
		}
	};
}
