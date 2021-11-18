#include "pch.h"
#include "CppUnitTest.h"

#include "SyncTraversers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nif_tests
{
	using namespace nif;

	TEST_CLASS(SyncTests)
	{
	public:

		template<typename T>
		struct NiSyncTest
		{
			void preRead(File* file, T* object, typename type_map<T>::type* native) {}
			void postRead(File* file, T* object, typename type_map<T>::type* native) {}
			void preWrite(File* file, T* object, typename type_map<T>::type* native) {}
			void postWrite(File* file, T* object, typename type_map<T>::type* native) {}
		};

		template<>
		struct NiSyncTest<NiObjectNET> : NiSyncTest<NiObject>
		{
			std::string name;

			Niflib::Ref<Niflib::NiExtraData> native_data0;
			Niflib::Ref<Niflib::NiExtraData> native_data1;
			Niflib::Ref<Niflib::NiExtraData> native_data2;
			std::shared_ptr<NiExtraData> data0;
			std::shared_ptr<NiExtraData> data1;
			std::shared_ptr<NiExtraData> data2;

			Niflib::Ref<Niflib::NiTimeController> native_ctlr0;
			Niflib::Ref<Niflib::NiTimeController> native_ctlr1;
			Niflib::Ref<Niflib::NiTimeController> native_ctlr2;
			std::shared_ptr<NiTimeController> ctlr0;
			std::shared_ptr<NiTimeController> ctlr1;
			std::shared_ptr<NiTimeController> ctlr2;

			void preRead(File* file, NiObjectNET* object, Niflib::NiObjectNET* native)
			{
				NiSyncTest<NiObject>::preRead(file, object, native);

				native_data0 = new Niflib::NiExtraData;
				native_data1 = new Niflib::NiExtraData;
				native_data2 = new Niflib::NiExtraData;
				data0 = file->get<NiExtraData>(native_data0);
				data1 = file->get<NiExtraData>(native_data1);
				data2 = file->get<NiExtraData>(native_data2);

				native_ctlr0 = new Niflib::NiTimeController;
				native_ctlr1 = new Niflib::NiTimeController;
				native_ctlr2 = new Niflib::NiTimeController;
				ctlr0 = file->get<NiTimeController>(native_ctlr0);
				ctlr1 = file->get<NiTimeController>(native_ctlr1);
				ctlr2 = file->get<NiTimeController>(native_ctlr2);

				name = "gnosrihgnjsoeir";
				native->SetName(name);

				native->AddExtraData(native_data0);
				native->AddExtraData(native_data1);
				native->AddExtraData(native_data2);

				//pushes to front
				native->AddController(native_ctlr2);
				native->AddController(native_ctlr1);
				native->AddController(native_ctlr0);
			}
			void postRead(File* file, NiObjectNET* object, Niflib::NiObjectNET* native)
			{
				NiSyncTest<NiObject>::postRead(file, object, native);

				Assert::IsTrue(object->name.get() == name);

				Assert::IsTrue(object->extraData.size() == 3);
				Assert::IsTrue(object->extraData.has(data0.get()));
				Assert::IsTrue(object->extraData.has(data1.get()));
				Assert::IsTrue(object->extraData.has(data2.get()));

				Assert::IsTrue(object->controllers.size() == 3);
				Assert::IsTrue(object->controllers.find(ctlr0.get()) == 0);
				Assert::IsTrue(object->controllers.find(ctlr1.get()) == 1);
				Assert::IsTrue(object->controllers.find(ctlr2.get()) == 2);
			}
			void preWrite(File* file, NiObjectNET* object, Niflib::NiObjectNET* native)
			{
				NiSyncTest<NiObject>::preWrite(file, object, native);

				name = "oeasiugershg";
				object->name.set(name);
				object->extraData.remove(data0.get());
				object->controllers.erase(1);
			}
			void postWrite(File* file, NiObjectNET* object, Niflib::NiObjectNET* native)
			{
				NiSyncTest<NiObject>::postWrite(file, object, native);

				Assert::IsTrue(native->GetName() == name);

				auto extraData = native->GetExtraData();
				Assert::IsTrue(extraData.size() == 2);
				Assert::IsTrue(extraData.front() == native_data1 || extraData.front() == native_data2);
				Assert::IsTrue(extraData.back() == native_data1 || extraData.back() == native_data2);

				auto controllers = native->GetControllers();
				Assert::IsTrue(controllers.size() == 2);
				Assert::IsTrue(controllers.front() == native_ctlr0);
				Assert::IsTrue(controllers.back() == native_ctlr2);
			}
		};

		template<>
		struct NiSyncTest<NiAVObject> : NiSyncTest<NiObjectNET>
		{
			std::mt19937 rng;
			//flags is 32 bit, but it was 16 in Niflibs days. I can't be bothered to fix that right now.
			std::uniform_int_distribution<std::uint_fast32_t> I{ 0, std::numeric_limits<unsigned short>::max() };
			std::uniform_real_distribution<float> F;

			std::uint_fast32_t flags;
			Niflib::Vector3 tr;
			math::Rotation rot;
			float scale;

			void preRead(File* file, NiAVObject* object, Niflib::NiAVObject* native) 
			{
				NiSyncTest<NiObjectNET>::preRead(file, object, native);

				flags = I(rng);
				native->SetFlags(flags);

				tr = { F(rng), F(rng), F(rng) };
				native->SetLocalTranslation(tr);

				rot.setEuler({
					F(rng) * 180.0f - 90.0f,
					F(rng) * 180.0f - 90.0f,
					F(rng) * 180.0f - 90.0f,
					math::EulerOrder::XYZ });

				native->SetLocalRotation(nif_type_conversion<Niflib::Matrix33>::from(rot));

				scale = F(rng);
				native->SetLocalScale(scale);
			}
			void postRead(File* file, NiAVObject* object, Niflib::NiAVObject* native) 
			{
				NiSyncTest<NiObjectNET>::postRead(file, object, native);

				Assert::IsTrue(object->flags.get() == flags);
				translation_t t = object->transform.translation.get();
				Assert::AreEqual(t[0], tr.x);
				Assert::AreEqual(t[1], tr.y);
				Assert::AreEqual(t[2], tr.z);

				auto q1 = rot.getQuaternion();
				auto q2 = object->transform.rotation.get().getQuaternion();
				Assert::AreEqual(q1.s, q2.s, 1.0e-5f);
				Assert::AreEqual(q1.v[0], q1.v[0], 1.0e-5f);
				Assert::AreEqual(q1.v[1], q1.v[1], 1.0e-5f);
				Assert::AreEqual(q1.v[2], q1.v[2], 1.0e-5f);

				Assert::AreEqual(object->transform.scale.get(), scale);
			}
			void preWrite(File* file, NiAVObject* object, Niflib::NiAVObject* native) 
			{
				NiSyncTest<NiObjectNET>::preWrite(file, object, native);

				flags = I(rng);
				object->flags.clear(-1);
				object->flags.set(flags);

				tr = { F(rng), F(rng), F(rng) };
				object->transform.translation.set(nif_type_conversion<translation_t>::from(tr));

				rot.setEuler({
					F(rng) * 180.0f - 90.0f,
					F(rng) * 180.0f - 90.0f,
					F(rng) * 180.0f - 90.0f,
					math::EulerOrder::XYZ });

				object->transform.rotation.set(rot);

				scale = F(rng);
				object->transform.scale.set(scale);
			}
			void postWrite(File* file, NiAVObject* object, Niflib::NiAVObject* native) 
			{
				NiSyncTest<NiObjectNET>::postWrite(file, object, native);

				Assert::IsTrue(native->GetFlags() == flags);

				Assert::IsTrue(native->GetLocalTranslation() == tr);

				auto q1 = rot.getQuaternion();
				auto q2 = nif_type_conversion<rotation_t>::from(native->GetLocalRotation()).getQuaternion();
				Assert::AreEqual(q1.s, q2.s, 1.0e-5f);
				Assert::AreEqual(q1.v[0], q1.v[0], 1.0e-5f);
				Assert::AreEqual(q1.v[1], q1.v[1], 1.0e-5f);
				Assert::AreEqual(q1.v[2], q1.v[2], 1.0e-5f);

				Assert::IsTrue(native->GetLocalScale() == scale);
			}
		};

		template<typename T>
		struct SyncTest : NiSyncTest<T>
		{
			SyncTest(File* file, T* object, typename type_map<T>::type* native) :
				file{ file }, object{ object }, native{ native } {}

			File* file;
			T* object;
			typename type_map<T>::type* native;

			void run()
			{
				this->preRead(file, object, native);
				object->receive(NiReadSyncer(*file));
				this->postRead(file, object, native);
				this->preWrite(file, object, native);
				object->receive(NiWriteSyncer(*file));
				this->postWrite(file, object, native);
			}
		};

		TEST_METHOD(_NiObjectNET)
		{
			File file(File::Version::SKYRIM_SE);

			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<NiObjectNET>(native);

			SyncTest<NiObjectNET>(&file, object.get(), native).run();
		}

		TEST_METHOD(_NiAVObject)
		{
			File file(File::Version::SKYRIM_SE);

			Niflib::Ref<Niflib::NiAVObject> native = new Niflib::NiAVObject;
			auto object = file.get<NiAVObject>(native);

			SyncTest<NiAVObject>(&file, object.get(), native).run();
		}
	};
}
