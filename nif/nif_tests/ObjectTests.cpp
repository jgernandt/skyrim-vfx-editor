#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace objects
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace common;

	std::mt19937 g_rng;

	//Test the syncer
	template<typename T>
	struct SyncerTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file{ nif::File::Version::SKYRIM_SE };
			T object{};
			Niflib::Ref<typename type_map<T>::type> native = new typename type_map<T>::type;

			Randomiser<T>{}.down(object, native, file, g_rng);//this overload randomises native
			nif::ReadSyncer<T>{}.down(object, native, file);
			EquivalenceTester<T>{}.down(object, native, file);
			Randomiser<T>{}.down(object, file, g_rng);//this overload randomises object
			nif::WriteSyncer<T>{}.down(object, native, file);
			EquivalenceTester<T>{}.down(object, native, file);
		}
	};

	//Test that the correct type is created by File
	template<typename T>
	struct CreateTypeTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file{ nif::File::Version::SKYRIM_SE };

			Niflib::Ref<Niflib::NiObject> native = new typename nif::type_map<T>::type;
			auto object = file.get<nif::NiObject>(native);
			Assert::IsTrue(object->type() == T::TYPE);
		}
	};

	//Test that created objects are synced with their native
	template<typename T>
	struct CreateSyncTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file{ nif::File::Version::SKYRIM_SE };

			Niflib::Ref<typename type_map<T>::type> native = new typename nif::type_map<T>::type;
			T dummy{};

			Randomiser<T>{}.down(dummy, native, file, g_rng);
			auto object = file.get<T>(native);
			EquivalenceTester<T>{}.down(*object, native, file);
		}
	};

	class HorizontalTestTraverser : public nif::HorizontalTraverser<HorizontalTestTraverser>
	{
		nif::ni_type& m_type;

	public:
		HorizontalTestTraverser(nif::ni_type& type) : m_type{ type } {}
		template<typename T>
		void invoke(T&)
		{
			m_type = T::TYPE;
		}
	};

	template<typename T>
	struct TraverseHTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			T object{};
			nif::ni_type type = nif::NiObject::TYPE;
			HorizontalTestTraverser t{ type };
			static_cast<nif::NiObject&>(object).receive(t);
			Assert::IsTrue(type == T::TYPE);
		}
	};
	template<typename T>
	struct VerticalTestTraverser : nif::VerticalTraverser<T, VerticalTestTraverser>
	{
		void operator() (T&, std::vector<nif::ni_type>& types)
		{
			types.push_back(T::TYPE);
		}
	};

	template<typename T>
	struct TraverseVTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			T object{};
			std::vector<nif::ni_type> types;

			nif::ni_type type = T::TYPE;
			nif::ni_type baseType = T::base_type::TYPE;

			//This would be interesting to test as well, but it's not a test of traversal.
			//Also, we may not always *want* the same inheritance as Niflib. We may want
			//to skip some old unused steps.
			//nif::ni_type nativeBase = std::hash<std::string>{}(nif::type_map<T>::type::TYPE.base_type->GetTypeName());
			//Assert::IsTrue(baseType == nativeBase);

			VerticalTestTraverser<T>{}.down(object, types);
			Assert::IsTrue(types.size() > 1);
			Assert::IsTrue(types.back() == T::TYPE);
			Assert::IsTrue(types[types.size() - 2] == baseType);

			types.clear();

			VerticalTestTraverser<T>{}.up(object, types);
			Assert::IsTrue(types.size() > 1);
			Assert::IsTrue(types[0] == T::TYPE);
			Assert::IsTrue(types[1] == baseType);
		}
	};

	class ForwarderTestTraverser : public HorizontalTraverser<ForwarderTestTraverser>
	{
		std::vector<nif::NiObject*>& m_traversed;
	public:
		ForwarderTestTraverser(std::vector<nif::NiObject*>& traversed) : m_traversed{ traversed } {}

		template<typename T>
		void invoke(T& object)
		{
			m_traversed.push_back(&object);
			nif::Forwarder<T>{}.down(object, *this);
		}
	};

	//Test the forwarding of horizontal traversers to subnodes
	template<typename T>
	struct ForwarderTest
	{
		void run()
		{
			static int duplicate = 0;
			Assert::IsTrue(!duplicate++);

			nif::File file{ nif::File::Version::SKYRIM_SE };
			T object{};

			Randomiser<T>{}.down(object, file, g_rng);

			std::vector<nif::NiObject*> traversed;
			ForwarderTestTraverser t(traversed);
			object.receive(t);

			ForwardOrderTester<T>{}.down(object, traversed.cbegin());
		}
	};

	TEST_CLASS(NiObjectNET)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiObjectNET>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiObjectNET>{}.run(); }
	};

	TEST_CLASS(NiAVObject)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiAVObject>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiAVObject>{}.run(); }
	};

	TEST_CLASS(NiNode)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiNode>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiNode>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiNode>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiNode>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiNode>{}.run(); }
	};

	TEST_CLASS(BSFadeNode)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::BSFadeNode>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::BSFadeNode>{}.run(); }
	};

	TEST_CLASS(NiProperty)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiProperty>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiProperty>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiProperty>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiProperty>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiProperty>{}.run(); }
	};

	TEST_CLASS(NiAlphaProperty)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiAlphaProperty>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiAlphaProperty>{}.run(); }
	};

	TEST_CLASS(BSShaderProperty)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::BSShaderProperty>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::BSShaderProperty>{}.run(); }
	};

	TEST_CLASS(BSEffectShaderProperty)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::BSEffectShaderProperty>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::BSEffectShaderProperty>{}.run(); }
	};

	TEST_CLASS(NiBoolData)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiBoolData>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiBoolData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiBoolData>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiBoolData>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiBoolData>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiBoolData>{}.run(); }
	};

	TEST_CLASS(NiFloatData)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiFloatData>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiFloatData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiFloatData>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiFloatData>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiFloatData>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiFloatData>{}.run(); }
	};

	TEST_CLASS(NiInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiInterpolator>{}.run(); }
	};

	TEST_CLASS(NiBoolInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiBoolInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiBoolInterpolator>{}.run(); }
	};

	TEST_CLASS(NiFloatInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiFloatInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiFloatInterpolator>{}.run(); }
	};

	TEST_CLASS(NiBlendInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiBlendInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiBlendInterpolator>{}.run(); }
	};

	TEST_CLASS(NiBlendBoolInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiBlendBoolInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiBlendBoolInterpolator>{}.run(); }
	};

	TEST_CLASS(NiBlendFloatInterpolator)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiBlendFloatInterpolator>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiBlendFloatInterpolator>{}.run(); }
	};

	TEST_CLASS(NiTimeController)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiTimeController>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiTimeController>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiTimeController>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiTimeController>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiTimeController>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiTimeController>{}.run(); }
	};

	TEST_CLASS(NiSingleInterpController)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiSingleInterpController>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiSingleInterpController>{}.run(); }
	};

	TEST_CLASS(NiParticleSystem)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiParticleSystem>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiParticleSystem>{}.run(); }
	};

	TEST_CLASS(NiPSysData)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysData>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysData>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysData>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysData>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysData>{}.run(); }
	};

	TEST_CLASS(NiPSysModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysAgeDeathModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysAgeDeathModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysAgeDeathModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysBoundUpdateModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysBoundUpdateModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysGravityModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysGravityModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysGravityModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysPositionModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysPositionModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysPositionModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysRotationModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysRotationModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysRotationModifier>{}.run(); }
	};

	TEST_CLASS(BSPSysScaleModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::BSPSysScaleModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::BSPSysScaleModifier>{}.run(); }
	};

	TEST_CLASS(BSPSysSimpleColorModifier)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::BSPSysSimpleColorModifier>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::BSPSysSimpleColorModifier>{}.run(); }
	};

	TEST_CLASS(NiPSysModifierCtlr)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysModifierCtlr>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysModifierCtlr>{}.run(); }
	};

	TEST_CLASS(NiPSysUpdateCtlr)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysUpdateCtlr>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysUpdateCtlr>{}.run(); }
	};

	TEST_CLASS(NiPSysEmitter)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysEmitter>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysVolumeEmitter)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysVolumeEmitter>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysVolumeEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysBoxEmitter)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysBoxEmitter>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysBoxEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysCylinderEmitter)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysCylinderEmitter>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysCylinderEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysSphereEmitter)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysSphereEmitter>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysSphereEmitter>{}.run(); }
	};

	TEST_CLASS(NiPSysEmitterCtlr)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiPSysEmitterCtlr>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiPSysEmitterCtlr>{}.run(); }
	};

	TEST_CLASS(NiExtraData)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiExtraData>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiExtraData>{}.run(); }
	};

	TEST_CLASS(NiStringExtraData)
	{
	public:
		TEST_METHOD(CreateSync) { CreateSyncTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(CreateType) { CreateTypeTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(Forwarder) { ForwarderTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(Syncer) { SyncerTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(TraverseH) { TraverseHTest<nif::NiStringExtraData>{}.run(); }
		TEST_METHOD(TraverseV) { TraverseVTest<nif::NiStringExtraData>{}.run(); }
	};
}