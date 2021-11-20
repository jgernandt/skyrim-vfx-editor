#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

namespace sync
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	//Test that vertical sync of know types is correct
	TEST_CLASS(KnownType)
	{
	public:
		template<typename T>
		struct Test
		{
			void run()
			{
				static nif::File file{ nif::File::Version::SKYRIM_SE };
				static std::mt19937 rng;//or do we want a new one for every test?
				T object{};
				Niflib::Ref<typename type_map<T>::type> native = new typename type_map<T>::type;

				common::Randomiser<T>{}.down(object, native, rng);//this overload randomises native
				ReadSyncer<T>{}.down(object, native, file);
				common::EquivalenceTester<T>{}.down(object, native, file);
				common::Randomiser<T>{}.down(object, native, file, rng);//this overload randomises object
				WriteSyncer<T>{}.down(object, native, file);
				common::EquivalenceTester<T>{}.down(object, native, file);
			}
		};

		TEST_METHOD(NiObjectNET) { Test<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(NiAVObject) { Test<nif::NiAVObject>{}.run(); }
		TEST_METHOD(NiNode) { Test<nif::NiNode>{}.run(); }
		TEST_METHOD(BSFadeNode) { Test<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(NiAlphaProperty) { Test<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(BSEffectShaderProperty) { Test<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(NiBoolData) { Test<nif::NiBoolData>{}.run(); }
		TEST_METHOD(NiFloatData) { Test<nif::NiFloatData>{}.run(); }
		TEST_METHOD(NiBoolInterpolator) { Test<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(NiFloatInterpolator) { Test<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(NiTimeController) { Test<nif::NiTimeController>{}.run(); }
		TEST_METHOD(NiSingleInterpController) { Test<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(NiParticleSystem) { Test<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(NiPSysData) { Test<nif::NiPSysData>{}.run(); }
		TEST_METHOD(NiExtraData) { Test<nif::NiExtraData>{}.run(); }
		TEST_METHOD(NiStringExtraData) { Test<nif::NiStringExtraData>{}.run(); }
	};

	//Test that the syncer correctly identifies the final type of an object
	TEST_CLASS(UnknownType)
	{
	public:
		template<typename T>
		struct Test
		{
			void run()
			{
				static nif::File file{ nif::File::Version::SKYRIM_SE };
				static std::mt19937 rng;//or do we want a new one for every test?
				Niflib::Ref<typename type_map<T>::type> native = new typename type_map<T>::type;
				auto object = file.get<T>(native);

				common::Randomiser<T>{}.down(*object, native, rng);
				static_cast<NiObject&>(*object).receive(NonForwardingReadSyncer(file));
				common::EquivalenceTester<T>{}.down(*object, native, file);
				common::Randomiser<T>{}.down(*object, native, file, rng);
				static_cast<NiObject&>(*object).receive(NonForwardingWriteSyncer(file));
				common::EquivalenceTester<T>{}.down(*object, native, file);
			}
		};

		TEST_METHOD(NiObjectNET) { Test<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(NiAVObject) { Test<nif::NiAVObject>{}.run(); }
		TEST_METHOD(NiNode) { Test<nif::NiNode>{}.run(); }
		TEST_METHOD(BSFadeNode) { Test<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(NiAlphaProperty) { Test<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(BSEffectShaderProperty) { Test<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(NiBoolData) { Test<nif::NiBoolData>{}.run(); }
		TEST_METHOD(NiFloatData) { Test<nif::NiFloatData>{}.run(); }
		TEST_METHOD(NiBoolInterpolator) { Test<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(NiFloatInterpolator) { Test<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(NiTimeController) { Test<nif::NiTimeController>{}.run(); }
		TEST_METHOD(NiSingleInterpController) { Test<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(NiParticleSystem) { Test<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(NiPSysData) { Test<nif::NiPSysData>{}.run(); }
		TEST_METHOD(NiExtraData) { Test<nif::NiExtraData>{}.run(); }
		TEST_METHOD(NiStringExtraData) { Test<nif::NiStringExtraData>{}.run(); }
	};
}
