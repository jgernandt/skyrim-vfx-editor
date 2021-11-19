#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"


namespace nif_tests
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;


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

		TEST_METHOD(_NiObjectNET)
		{
			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<NiObjectNET>(native);

			SyncTest<NiObjectNET>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiAVObject)
		{
			Niflib::Ref<Niflib::NiAVObject> native = new Niflib::NiAVObject;
			auto object = file.get<NiAVObject>(native);

			SyncTest<NiAVObject>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiNode)
		{
			Niflib::Ref<Niflib::NiNode> native = new Niflib::NiNode;
			auto object = file.get<NiNode>(native);

			SyncTest<NiNode>{}.run(*object, native, file);
		}

		TEST_METHOD(_BSFadeNode)
		{
			Niflib::Ref<Niflib::BSFadeNode> native = new Niflib::BSFadeNode;
			auto object = file.get<BSFadeNode>(native);

			SyncTest<BSFadeNode>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiAlphaProperty)
		{
			Niflib::Ref<Niflib::NiAlphaProperty> native = new Niflib::NiAlphaProperty;
			auto object = file.get<NiAlphaProperty>(native);

			SyncTest<NiAlphaProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(_BSEffectShaderProperty)
		{
			Niflib::Ref<Niflib::BSEffectShaderProperty> native = new Niflib::BSEffectShaderProperty;
			auto object = file.get<BSEffectShaderProperty>(native);

			SyncTest<BSEffectShaderProperty>{}.run(*object, native, file);
		}
	};

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

		TEST_METHOD(_NiObjectNET)
		{
			Niflib::Ref<Niflib::NiObjectNET> native = new Niflib::NiObjectNET;
			auto object = file.get<NiObjectNET>(native);

			SyncTest<NiObjectNET>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiAVObject)
		{
			Niflib::Ref<Niflib::NiAVObject> native = new Niflib::NiAVObject;
			auto object = file.get<NiAVObject>(native);

			SyncTest<NiAVObject>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiNode)
		{
			Niflib::Ref<Niflib::NiNode> native = new Niflib::NiNode;
			auto object = file.get<NiNode>(native);

			SyncTest<NiNode>{}.run(*object, native, file);
		}

		TEST_METHOD(_BSFadeNode)
		{
			Niflib::Ref<Niflib::BSFadeNode> native = new Niflib::BSFadeNode;
			auto object = file.get<BSFadeNode>(native);

			SyncTest<BSFadeNode>{}.run(*object, native, file);
		}

		TEST_METHOD(_NiAlphaProperty)
		{
			Niflib::Ref<Niflib::NiAlphaProperty> native = new Niflib::NiAlphaProperty;
			auto object = file.get<NiAlphaProperty>(native);

			SyncTest<NiAlphaProperty>{}.run(*object, native, file);
		}

		TEST_METHOD(_BSEffectShaderProperty)
		{
			Niflib::Ref<Niflib::BSEffectShaderProperty> native = new Niflib::BSEffectShaderProperty;
			auto object = file.get<BSEffectShaderProperty>(native);

			SyncTest<BSEffectShaderProperty>{}.run(*object, native, file);
		}
	};
}
