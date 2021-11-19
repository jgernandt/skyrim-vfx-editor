#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"


namespace nif_tests
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	//Test that vertical sync of know types is correct
	TEST_CLASS(SyncTests)
	{
	public:
		template<typename T>
		struct SyncTest
		{
			void run(T& object, typename type_map<T>::type* native, File& file)
			{
				SyncTestTraverser<T> t;
				t.down(object, native, file);
				ReadSyncer<T>{}.down(object, native, file);
				t.down(object, native, file, 0);
				WriteSyncer<T>{}.down(object, native, file);
				t.down(object, native, file, 0, 0);
			}
		};

		nif::File file{ nif::File::Version::SKYRIM_SE };

		TEST_METHOD(NiObjectNET)
		{
			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<nif::NiObjectNET>(native);

			SyncTest<nif::NiObjectNET>{}.run(*object, native, file);
		}

		TEST_METHOD(NiAVObject)
		{
			Niflib::Ref<Niflib::NiAVObject> native = new Niflib::NiAVObject;
			auto object = file.get<nif::NiAVObject>(native);

			SyncTest<nif::NiAVObject>{}.run(*object, native, file);
		}

		TEST_METHOD(NiNode)
		{
			Niflib::Ref<Niflib::NiNode> native = new Niflib::NiNode;
			auto object = file.get<nif::NiNode>(native);

			SyncTest<nif::NiNode>{}.run(*object, native, file);
		}

		TEST_METHOD(BSFadeNode)
		{
			Niflib::Ref<Niflib::BSFadeNode> native = new Niflib::BSFadeNode;
			auto object = file.get<nif::BSFadeNode>(native);

			SyncTest<nif::BSFadeNode>{}.run(*object, native, file);
		}

		TEST_METHOD(NiAlphaProperty)
		{
			Niflib::Ref<Niflib::NiAlphaProperty> native = new Niflib::NiAlphaProperty;
			auto object = file.get<nif::NiAlphaProperty>(native);

			SyncTest<nif::NiAlphaProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(BSEffectShaderProperty)
		{
			Niflib::Ref<Niflib::BSEffectShaderProperty> native = new Niflib::BSEffectShaderProperty;
			auto object = file.get<nif::BSEffectShaderProperty>(native);

			SyncTest<nif::BSEffectShaderProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(NiBoolData)
		{
			Niflib::Ref<Niflib::NiBoolData> native = new Niflib::NiBoolData;
			auto object = file.get<nif::NiBoolData>(native);

			SyncTest<nif::NiBoolData>{}.run(*object, native, file);
		}

		TEST_METHOD(NiFloatData)
		{
			Niflib::Ref<Niflib::NiFloatData> native = new Niflib::NiFloatData;
			auto object = file.get<nif::NiFloatData>(native);

			SyncTest<nif::NiFloatData>{}.run(*object, native, file);
		}
	};

	//Test that the syncer correctly identifies the final type of an object
	TEST_CLASS(SyncTraverserTests)
	{
	public:
		template<typename T>
		struct SyncTest
		{
			void run(T& object, typename type_map<T>::type* native, File& file)
			{
				SyncTestTraverser<T> t;
				t.down(object, native, file);
				static_cast<NiObject&>(object).receive(NiReadSyncer(file));
				t.down(object, native, file, 0);
				static_cast<NiObject&>(object).receive(NiWriteSyncer(file));
				t.down(object, native, file, 0, 0);
			}
		};

		nif::File file{ nif::File::Version::SKYRIM_SE };

		TEST_METHOD(NiObjectNET)
		{
			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<nif::NiObjectNET>(native);

			SyncTest<nif::NiObjectNET>{}.run(*object, native, file);
		}

		TEST_METHOD(NiAVObject)
		{
			Niflib::Ref<Niflib::NiAVObject> native = new Niflib::NiAVObject;
			auto object = file.get<nif::NiAVObject>(native);

			SyncTest<nif::NiAVObject>{}.run(*object, native, file);
		}

		TEST_METHOD(NiNode)
		{
			Niflib::Ref<Niflib::NiNode> native = new Niflib::NiNode;
			auto object = file.get<nif::NiNode>(native);

			SyncTest<nif::NiNode>{}.run(*object, native, file);
		}

		TEST_METHOD(BSFadeNode)
		{
			Niflib::Ref<Niflib::BSFadeNode> native = new Niflib::BSFadeNode;
			auto object = file.get<nif::BSFadeNode>(native);

			SyncTest<nif::BSFadeNode>{}.run(*object, native, file);
		}

		TEST_METHOD(NiAlphaProperty)
		{
			Niflib::Ref<Niflib::NiAlphaProperty> native = new Niflib::NiAlphaProperty;
			auto object = file.get<nif::NiAlphaProperty>(native);

			SyncTest<nif::NiAlphaProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(BSEffectShaderProperty)
		{
			Niflib::Ref<Niflib::BSEffectShaderProperty> native = new Niflib::BSEffectShaderProperty;
			auto object = file.get<nif::BSEffectShaderProperty>(native);

			SyncTest<nif::BSEffectShaderProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(NiBoolData)
		{
			Niflib::Ref<Niflib::NiBoolData> native = new Niflib::NiBoolData;
			auto object = file.get<nif::NiBoolData>(native);

			SyncTest<nif::NiBoolData>{}.run(*object, native, file);
		}

		TEST_METHOD(NiFloatData)
		{
			Niflib::Ref<Niflib::NiFloatData> native = new Niflib::NiFloatData;
			auto object = file.get<nif::NiFloatData>(native);

			SyncTest<nif::NiFloatData>{}.run(*object, native, file);
		}
	};
}
