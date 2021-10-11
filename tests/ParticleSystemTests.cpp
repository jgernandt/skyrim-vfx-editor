#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"
#include "ModifierRequirements.h"

namespace nif
{
	TEST_CLASS(Converters)
	{
	public:
		std::mt19937 m_engine;

		TEST_METHOD(Subtextures)
		{
			//Convert back and forth and see if we get the same count. Not a failsafe test, but good enough.
			std::uniform_int_distribution<int> D(1, 10);
			for (int i = 0; i < 3; i++) {
				nif::SubtextureCount cnt{ D(m_engine), D(m_engine) };
				if (cnt == nif::SubtextureCount{ 1, 1 }) {//special case, reroll
					i--;
					continue;
				}
				auto offsets = nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(cnt);
				Assert::IsTrue(cnt[0] * cnt[1] == offsets.size());
				Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(offsets) == cnt);
			}

			//Special cases
			Assert::IsTrue(nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(
				nif::SubtextureCount{ 1, 1 }).empty());
			Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(
				std::vector<nif::SubtextureOffset>()) == nif::SubtextureCount{ 1, 1 });

			Assert::IsTrue(nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(
				nif::SubtextureCount{ 0, 0 }).empty());
			std::uniform_real_distribution<float> Df;
			std::vector<nif::SubtextureOffset> irregular{
				{ Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) },
				{ Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) } };
			Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(irregular) == nif::SubtextureCount{ 0, 0 });
		}
	};

	TEST_CLASS(NiPSysDataTests)
	{
		nif::NiPSysData obj;
		std::mt19937 m_engine;

		TEST_METHOD(MaxCount)
		{
			PropertyTest<unsigned short>(obj.maxCount(), m_engine);
		}

		TEST_METHOD(SubtexOffsets)
		{
			std::uniform_int_distribution<size_t> Di(1, 10);
			std::uniform_real_distribution<float> Df;

			for (int i = 0; i < 3; i++) {
				//populate a vector of random size
				std::vector<nif::SubtextureOffset> offsets(Di(m_engine));
				for (auto&& offset : offsets)
					offset = { Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) };

				obj.subtexOffsets().set(offsets);

				Assert::IsTrue(obj.subtexOffsets().get() == offsets);
			}
		}

		TEST_METHOD(HasColour)
		{
			PropertyTest<bool>(obj.hasColour(), m_engine);
		}

		TEST_METHOD(HasRotation)
		{
			PropertyTest<bool>(obj.hasRotationAngles(), m_engine);
			PropertyTest<bool>(obj.hasRotationSpeeds(), m_engine);
		}
	};

	TEST_CLASS(NiParticleSystemTests)
	{
	public:
		nif::NiParticleSystem obj;
		std::mt19937 m_engine;

		TEST_METHOD(Data)
		{
			AssignableTest<NiPSysData>(obj.data());
		}

		TEST_METHOD(Modifiers)
		{
			SequenceTest<NiPSysModifier, NiPSysRotationModifier>(obj.modifiers());
		}

		TEST_METHOD(ShaderProperty)
		{
			AssignableTest<BSEffectShaderProperty>(obj.shaderProperty());
		}

		TEST_METHOD(AlphaProperty)
		{
			AssignableTest<NiAlphaProperty>(obj.alphaProperty());
		}

		TEST_METHOD(WorldSpace)
		{
			PropertyTest<bool>(obj.worldSpace(), m_engine);
		}

	};
}

namespace node
{
	TEST_CLASS(ParticleSystemTests)
	{
	public:
		std::mt19937 m_engine;

		//Editing the texure atlas layout widget should set subtexture offsets in the PSysData
		TEST_METHOD(TextureAtlasLayout)
		{
			ParticleSystem node;

			Assert::IsTrue(node.subtexCount().get() == nif::SubtextureCount{ 1, 1 });

			nif::SubtextureCount cnt{ 3, 5 };
			node.subtexCount().set(cnt);
			Assert::IsTrue(node.subtexCount().get() == cnt);

			Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(node.data().subtexOffsets().get()) == cnt);
		}

		//We're moving the subtexture count to ParticleSystem, so should now only send Assignable<BSEffectShaderProperty> (single)
		TEST_METHOD(Shader)
		{
			auto node = std::make_unique<ParticleSystem>();
			auto& ass = node->object().shaderProperty();
			AssignableSenderTest(ParticleSystem::SHADER, false, std::move(node), ass);

			/*MockProperty<nif::SubtextureCount> target0;
			MockProperty<nif::SubtextureCount> target;
			ConnectorTester tester(std::make_unique<ParticleSystem>());

			//Shader should send Assignable<BSEffectShader> and receive Observable<Subtexture>, single connector
			tester.tryConnect<IAssignable<nif::BSEffectShaderProperty>, IProperty<nif::SubtextureCount>>(
				ParticleSystem::SHADER, false, &target0);
			auto ifc = tester.tryConnect<IAssignable<nif::BSEffectShaderProperty>, IProperty<nif::SubtextureCount>>(
				ParticleSystem::SHADER, false, &target);
			Assert::IsNotNull(ifc);

			//if we assign to the interface, it should set the shader on the psys
			nif::BSEffectShaderProperty shader;
			ifc->assign(&shader);
			Assert::IsTrue(tester.getNode()->object().shaderProperty().isAssigned(&shader));
			ifc->assign(nullptr);
			Assert::IsFalse(tester.getNode()->object().shaderProperty().isAssigned(&shader));
			Assert::IsFalse(tester.getNode()->object().shaderProperty().isAssigned(nullptr));//there should be a default

			//if we call listeners on the target, they should set the subtex offsets on the psys data
			std::uniform_int_distribution<int> D(1, 10);
			for (int i = 0; i < 3; i++) {
				nif::SubtextureCount cnt{ D(m_engine), D(m_engine) };
				auto offs_exp = nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(cnt);

				target.set(cnt);
				std::vector<nif::SubtextureOffset> offs_act = tester.getNode()->data().subtexOffsets().get();
				for (auto&& offset : offs_exp)
					Assert::IsTrue(std::find(offs_act.begin(), offs_act.end(), offset) != offs_act.end());
			}

			tester.disconnect(&target);

			//subtex count should be cleared
			Assert::IsTrue(tester.getNode()->data().subtexOffsets().get().empty());
			//and should no longer receive calls
			target.set({ 3, 2 });
			Assert::IsTrue(tester.getNode()->data().subtexOffsets().get().empty());*/
		}

		//Modifiers should send Modifiable, single connector
		TEST_METHOD(Modifiers)
		{
			ConnectorTester tester(std::make_unique<ParticleSystem>());
			tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);
			IModifiable* ifc = tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);

			Assert::IsNotNull(ifc);

			//Test the effect of interacting with the interface

			//Inserting a modifier should place it in the native sequence. 
			//Not necessarily in the indicated position, but order of subsequent insertions must be respected.
			nif::NiPSysRotationModifier mod1;
			ifc->modifiers().insert(-1, mod1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod1) != -1);
			nif::NiPSysRotationModifier mod2;
			ifc->modifiers().insert(-1, mod2);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod2) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod2) > tester.getNode()->object().modifiers().find(mod1));
			nif::NiPSysRotationModifier mod3;
			ifc->modifiers().insert(0, mod3);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) < tester.getNode()->object().modifiers().find(mod1));

			//Searching for a modifier through the interface must yield its order in the native sequence
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod1) == ifc->modifiers().find(mod1));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod2) == ifc->modifiers().find(mod2));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) == ifc->modifiers().find(mod3));

			//Erasing must also affect the native sequence
			ifc->modifiers().erase(ifc->modifiers().find(mod1));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod1) == -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod2) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) != -1);
			ifc->modifiers().erase(ifc->modifiers().find(mod2));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod2) == -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) != -1);
			ifc->modifiers().erase(ifc->modifiers().find(mod3));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(mod3) == -1);

			//Similary with controllers
			nif::NiPSysEmitterCtlr ctlr1;
			ifc->controllers().insert(-1, ctlr1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr1) != -1);
			nif::NiPSysEmitterCtlr ctlr2;
			ifc->controllers().insert(-1, ctlr2);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr2) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr2) > tester.getNode()->object().controllers().find(ctlr1));
			nif::NiPSysEmitterCtlr ctlr3;
			ifc->controllers().insert(0, ctlr3);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) < tester.getNode()->object().controllers().find(ctlr1));

			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr1) == ifc->controllers().find(ctlr1));
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr2) == ifc->controllers().find(ctlr2));
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) == ifc->controllers().find(ctlr3));

			ifc->controllers().erase(ifc->controllers().find(ctlr1));
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr1) == -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr2) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) != -1);
			ifc->controllers().erase(ifc->controllers().find(ctlr2));
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr2) == -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) != -1);
			ifc->controllers().erase(ifc->controllers().find(ctlr3));
			Assert::IsTrue(tester.getNode()->object().controllers().find(ctlr3) == -1);

			//Requirements may have to be tested with specific modifiers individually?
			Modifier::Requirement reqs[]{
				Modifier::Requirement::COLOUR,
				Modifier::Requirement::LIFETIME,
				Modifier::Requirement::MOVEMENT,
				Modifier::Requirement::ROTATION };

			Assert::IsTrue(ifc->requirements().has(Modifier::Requirement::UPDATE));//should always be set

			//The bool properties are simple to test
			if (ifc->requirements().has(Modifier::Requirement::COLOUR))
				ifc->requirements().remove(Modifier::Requirement::COLOUR);
			Assert::IsFalse(tester.getNode()->data().hasColour().get());

			ifc->requirements().add(Modifier::Requirement::COLOUR);
			Assert::IsTrue(tester.getNode()->data().hasColour().get());

			ifc->requirements().add(Modifier::Requirement::COLOUR);
			ifc->requirements().remove(Modifier::Requirement::COLOUR);
			Assert::IsTrue(tester.getNode()->data().hasColour().get());

			ifc->requirements().remove(Modifier::Requirement::COLOUR);
			Assert::IsFalse(tester.getNode()->data().hasColour().get());


			if (ifc->requirements().has(Modifier::Requirement::ROTATION))
				ifc->requirements().remove(Modifier::Requirement::ROTATION);
			Assert::IsFalse(tester.getNode()->data().hasRotationAngles().get());
			Assert::IsFalse(tester.getNode()->data().hasRotationSpeeds().get());

			ifc->requirements().add(Modifier::Requirement::ROTATION);
			Assert::IsTrue(tester.getNode()->data().hasRotationAngles().get());
			Assert::IsTrue(tester.getNode()->data().hasRotationSpeeds().get());

			ifc->requirements().add(Modifier::Requirement::ROTATION);
			ifc->requirements().remove(Modifier::Requirement::ROTATION);
			Assert::IsTrue(tester.getNode()->data().hasRotationAngles().get());
			Assert::IsTrue(tester.getNode()->data().hasRotationSpeeds().get());

			ifc->requirements().remove(Modifier::Requirement::ROTATION);
			Assert::IsFalse(tester.getNode()->data().hasRotationAngles().get());
			Assert::IsFalse(tester.getNode()->data().hasRotationSpeeds().get());

		}

		//The modifiers are a little more difficult. We want to test that the requirement managers keep 
		//their modifiers in the right position and that they have the right name and target.
		TEST_METHOD(ModifierRequirements2)
		{
			nif::NiParticleSystem obj;
			nif::NiPSysData data;

			ReservableSequence<nif::NiPSysModifier> mods(obj.modifiers());
			ReservableSequence<nif::NiTimeController> ctlrs(obj.controllers());

			nif::NiPSysBoundUpdateModifier modObj;
			nif::NiPSysUpdateCtlr ctlrObj;
			UpdateRequirement upd(modObj, ctlrObj, mods, ctlrs);
			upd.incr();

			//Modifier should be assigned to the right target
			Assert::IsTrue(upd.modifier().target().isAssigned(&obj));

			//Bound update modifier + update controller should always be last
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 0);
			Assert::IsTrue(obj.controllers().find(upd.controller()) == 0);
			//And name should match position
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:0");

			//Insert a mod+ctlr at the end
			nif::NiPSysBoxEmitter mod1;
			nif::NiPSysEmitterCtlr ctlr1;
			mods.insert(-1, mod1);
			ctlrs.insert(-1, ctlr1);

			//Position and name of update mod should have changed
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 1);
			Assert::IsTrue(obj.controllers().find(upd.controller()) == 1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:1");

			//Add a movement requirement
			nif::NiPSysPositionModifier posObj;
			MovementRequirement mov(posObj, mods);
			mov.incr();
			Assert::IsTrue(mov.modifier().target().isAssigned(&obj));

			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 2);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == 1);

			Assert::IsTrue(upd.modifier().name().get() == "Modifier:2");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:1");

			//Insert another mod at the end
			nif::NiPSysRotationModifier mod2;
			mods.insert(-1, mod2);

			//Position mod should be second last, update last
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 3);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == 2);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:2");

			//Add a lifetime requirement
			nif::NiPSysAgeDeathModifier ageObj;
			LifetimeRequirement liv(ageObj, mods);
			liv.incr();
			Assert::IsTrue(liv.modifier().target().isAssigned(&obj));

			//It should insert at the beginning
			Assert::IsTrue(obj.modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");
			//And the others should hear it
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:3");

			//Insert a mod at the beginning
			nif::NiPSysRotationModifier mod3;
			mods.insert(0, mod3);

			//and verify positions and names
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 5);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == 4);
			Assert::IsTrue(obj.modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:5");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");

			//Remove a mod and make sure no one does anything odd
			mods.erase(2);
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 4);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == 3);
			Assert::IsTrue(obj.modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");

			//Remove the lifetime requirement
			liv.decr();
			Assert::IsFalse(liv.modifier().target().isAssigned(&obj));

			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 3);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == 2);
			Assert::IsTrue(obj.modifiers().find(liv.modifier()) == -1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:2");

			//Remove the position requirement
			mov.decr();
			Assert::IsFalse(mov.modifier().target().isAssigned(&obj));

			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == 2);
			Assert::IsTrue(obj.modifiers().find(mov.modifier()) == -1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:2");

			//Remove the update requirement
			upd.decr();
			Assert::IsFalse(upd.modifier().target().isAssigned(&obj));
			Assert::IsTrue(obj.modifiers().find(upd.modifier()) == -1);
		}
	};
}
