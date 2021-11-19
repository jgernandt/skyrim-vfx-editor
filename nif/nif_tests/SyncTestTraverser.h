#pragma once
#include "CppUnitTest.h"
#include "SyncTraversers.h"

namespace nif_tests
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	template<typename T, typename NativeType = T>
	struct FlagSetSyncTest
	{
		T val;

		template<typename ObjType, typename ArgType, typename Distribution>
		void preRead(ObjType* native, void(ObjType::* set)(ArgType), std::mt19937& rng, const Distribution& D)
		{
			val = static_cast<T>(D(rng));
			(native->*set)(nif_type_conversion<NativeType>::from(val));
		}
		template<typename Distribution>
		void preWrite(FlagSet<T>& prop, std::mt19937& rng, const Distribution& D)
		{
			val = static_cast<T>(D(rng));
			prop.clear();
			prop.set(val);
		}
		template<typename ObjType, typename RetType>
		void test(FlagSet<T>& prop, ObjType* native, RetType(ObjType::* fcn)() const)
		{
			Assert::IsTrue(prop.get() == val);
			Assert::IsTrue((native->*fcn)() == nif_type_conversion<NativeType>::from(val));
		}
	};

	template<typename T, typename NativeType = T>
	struct PropertySyncTest
	{
		T val;

		template<typename ObjType, typename ArgType, typename Distribution>
		void preRead(ObjType* native, void(ObjType::* set)(ArgType), std::mt19937& rng, const Distribution& D)
		{
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(val, i) = static_cast<typename util::array_traits<T>::element_type>(D(rng));
			(native->*set)(nif_type_conversion<NativeType>::from(val));
		}
		template<typename Distribution>
		void preWrite(Property<T>& prop, std::mt19937& rng, const Distribution& D)
		{
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(val, i) = static_cast<typename util::array_traits<T>::element_type>(D(rng));
			prop.set(val);
		}
		template<typename ObjType, typename RetType>
		void test(Property<T>& prop, ObjType* native, RetType(ObjType::* get)() const)
		{
			Assert::IsTrue(prop.get() == val);
			Assert::IsTrue((native->*get)() == nif_type_conversion<NativeType>::from(val));
		}
	};
	template<>
	struct PropertySyncTest<std::string, std::string>
	{
		std::string val;

		template<typename ObjType, typename ArgType>
		void preRead(ObjType* native, void(ObjType::* set)(ArgType))
		{
			val = "oinaborebseg";
			(native->*set)(val);
		}
		void preWrite(Property<std::string>& prop)
		{
			val = "ebgneslobnso";
			prop.set(val);
		}
		template<typename ObjType, typename RetType>
		void test(Property<std::string>& prop, ObjType* native, RetType(ObjType::* get)() const)
		{
			Assert::IsTrue(prop.get() == val);
			Assert::IsTrue((native->*get)() == val);
		}
	};

	template<typename T>
	struct SyncTestTraverser : VerticalTraverser<T, SyncTestTraverser>
	{
		void operator() (T& object, typename type_map<T>::type* native, File& file) {}//pre read
		void operator() (T& object, typename type_map<T>::type* native, File& file, int) {}//inter r/w
		void operator() (T& object, typename type_map<T>::type* native, File& file, int, int) {}//post write
	};

	template<>
	struct SyncTestTraverser<NiObjectNET> : VerticalTraverser<NiObjectNET, SyncTestTraverser>
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

		void operator() (NiObjectNET& object, Niflib::NiObjectNET* native, File& file);
		void operator() (NiObjectNET& object, Niflib::NiObjectNET* native, File& file, int);
		void operator() (NiObjectNET& object, Niflib::NiObjectNET* native, File& file, int, int);
	};

	template<>
	struct SyncTestTraverser<NiAVObject> : VerticalTraverser<NiAVObject, SyncTestTraverser>
	{
		std::mt19937 rng;
		//flags is 32 bit, but it was 16 in Niflib's days. I can't be bothered to fix that right now.
		std::uniform_int_distribution<std::uint_fast32_t> I{ 0, std::numeric_limits<unsigned short>::max() };
		std::uniform_real_distribution<float> F;

		std::uint_fast32_t flags;
		Niflib::Vector3 tr;
		math::Rotation rot;
		float scale;

		void operator() (NiAVObject& object, Niflib::NiAVObject* native, File& file);
		void operator() (NiAVObject& object, Niflib::NiAVObject* native, File& file, int);
		void operator() (NiAVObject& object, Niflib::NiAVObject* native, File& file, int, int);
	};

	template<>
	struct SyncTestTraverser<NiNode> : VerticalTraverser<NiNode, SyncTestTraverser>
	{
		Niflib::Ref<Niflib::NiAVObject> native_child0;
		Niflib::Ref<Niflib::NiAVObject> native_child1;
		Niflib::Ref<Niflib::NiAVObject> native_child2;
		std::shared_ptr<NiAVObject> child0;
		std::shared_ptr<NiAVObject> child1;
		std::shared_ptr<NiAVObject> child2;

		void operator() (NiNode& object, Niflib::NiNode* native, File& file);
		void operator() (NiNode& object, Niflib::NiNode* native, File& file, int);
		void operator() (NiNode& object, Niflib::NiNode* native, File& file, int, int);
	};

	template<>
	struct SyncTestTraverser<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, SyncTestTraverser>
	{
		AlphaMode mode{ AlphaMode::BLEND };
		BlendFunction srcFcn{ BlendFunction::DST_ALPHA };
		BlendFunction dstFcn{ BlendFunction::ONE_MINUS_SRC_ALPHA };
		TestFunction testFcn{ TestFunction::GEQUAL };
		std::uint_fast8_t threshold{ 134 };
		bool noSorting{ true };

		void operator() (NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file);
		void operator() (NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file, int);
		void operator() (NiAlphaProperty& object, Niflib::NiAlphaProperty* native, File& file, int, int);

		void test(NiAlphaProperty& object, Niflib::NiAlphaProperty* native);
	};

	template<>
	struct SyncTestTraverser<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, SyncTestTraverser>
	{
		std::mt19937 rng;
		std::uniform_int_distribution<std::uint_fast32_t> I;
		std::uniform_real_distribution<float> F;

		PropertySyncTest<ColRGBA, Niflib::Color4> emissiveCol;
		PropertySyncTest<float> emissiveMult;
		PropertySyncTest<std::string> sourceTex;
		PropertySyncTest<std::string> greyscaleTex;
		FlagSetSyncTest<ShaderFlags, Niflib::SkyrimShaderPropertyFlags1> shaderFlags1;
		FlagSetSyncTest<ShaderFlags, Niflib::SkyrimShaderPropertyFlags2> shaderFlags2;

		void operator() (BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file);
		void operator() (BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file, int);
		void operator() (BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, File& file, int, int);

		void test(BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native);
	};

	template<>
	struct SyncTestTraverser<NiBoolData> : VerticalTraverser<NiBoolData, SyncTestTraverser>
	{
		std::mt19937 rng;
		std::uniform_int_distribution<KeyType> I;
		std::uniform_real_distribution<float> F;
		std::uniform_int_distribution<int> B{ 0, 1 };

		PropertySyncTest<KeyType, Niflib::KeyType> keyType;
		std::vector<Niflib::Key<unsigned char>> keys;

		void operator() (NiBoolData& object, Niflib::NiBoolData* native, File& file);
		void operator() (NiBoolData& object, Niflib::NiBoolData* native, File& file, int);
		void operator() (NiBoolData& object, Niflib::NiBoolData* native, File& file, int, int);

		void test(NiBoolData& object, Niflib::NiBoolData* native);
	};

	template<>
	struct SyncTestTraverser<NiFloatData> : VerticalTraverser<NiFloatData, SyncTestTraverser>
	{
		std::mt19937 rng;
		std::uniform_int_distribution<KeyType> I;
		std::uniform_real_distribution<float> F;

		PropertySyncTest<KeyType, Niflib::KeyType> keyType;
		std::vector<Niflib::Key<float>> keys;

		void operator() (NiFloatData& object, Niflib::NiFloatData* native, File& file);
		void operator() (NiFloatData& object, Niflib::NiFloatData* native, File& file, int);
		void operator() (NiFloatData& object, Niflib::NiFloatData* native, File& file, int, int);

		void test(NiFloatData& object, Niflib::NiFloatData* native);
	};
}
