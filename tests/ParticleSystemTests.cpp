#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"
#include "ModifierRequirements.h"

#include "Constructor.h"
#include "nif_backend.h"

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
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(MaxCount)
		{
			auto obj = file.create<NiPSysData>();
			Assert::IsNotNull(obj.get());

			PropertyTest<unsigned short>(obj->maxCount(), m_engine);
		}

		TEST_METHOD(SubtexOffsets)
		{
			auto obj = file.create<NiPSysData>();
			Assert::IsNotNull(obj.get());

			std::uniform_int_distribution<size_t> Di(1, 10);
			std::uniform_real_distribution<float> Df;

			for (int i = 0; i < 3; i++) {
				//populate a vector of random size
				std::vector<nif::SubtextureOffset> offsets(Di(m_engine));
				for (auto&& offset : offsets)
					offset = { Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) };

				obj->subtexOffsets().set(offsets);

				Assert::IsTrue(obj->subtexOffsets().get() == offsets);
			}
		}

		TEST_METHOD(HasColour)
		{
			auto obj = file.create<NiPSysData>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->hasColour(), m_engine);
		}

		TEST_METHOD(HasRotation)
		{
			auto obj = file.create<NiPSysData>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->hasRotationAngles(), m_engine);
			PropertyTest<bool>(obj->hasRotationSpeeds(), m_engine);
		}
	};

	TEST_CLASS(NiParticleSystemTests)
	{
	public:
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Data)
		{
			auto obj = file.create<NiParticleSystem>();
			Assert::IsNotNull(obj.get());

			AssignableTest<NiPSysData>(obj->data(), std::bind(&File::create<NiPSysData>, &file));
		}

		TEST_METHOD(Modifiers)
		{
			auto obj = file.create<NiParticleSystem>();
			Assert::IsNotNull(obj.get());

			SequenceTest<NiPSysModifier>(obj->modifiers(), std::bind(&File::create<NiPSysRotationModifier>, &file));
		}

		TEST_METHOD(ShaderProperty)
		{
			auto obj = file.create<NiParticleSystem>();
			Assert::IsNotNull(obj.get());

			AssignableTest<BSEffectShaderProperty>(obj->shaderProperty(), std::bind(&File::create<BSEffectShaderProperty>, &file));
		}

		TEST_METHOD(AlphaProperty)
		{
			auto obj = file.create<NiParticleSystem>();
			Assert::IsNotNull(obj.get());

			AssignableTest<NiAlphaProperty>(obj->alphaProperty(), std::bind(&File::create<NiAlphaProperty>, &file));
		}

		TEST_METHOD(WorldSpace)
		{
			auto obj = file.create<NiParticleSystem>();
			Assert::IsNotNull(obj.get());

			PropertyTest<bool>(obj->worldSpace(), m_engine);
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
			nif::File file{ nif::File::Version::SKYRIM_SE };
			ParticleSystem node(file);

			Assert::IsTrue(node.subtexCount().get() == nif::SubtextureCount{ 1, 1 });

			nif::SubtextureCount cnt{ 3, 5 };
			node.subtexCount().set(cnt);
			Assert::IsTrue(node.subtexCount().get() == cnt);

			Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(node.data().subtexOffsets().get()) == cnt);
		}

		//We're moving the subtexture count to ParticleSystem, so should now only send Assignable<BSEffectShaderProperty> (single)
		TEST_METHOD(Shader)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto node = std::make_unique<ParticleSystem>(file);
			auto& ass = node->object().shaderProperty();
			AssignableSenderTest(ParticleSystem::SHADER, false, std::move(node), ass, 
				std::bind(&nif::File::create<nif::BSEffectShaderProperty>, &file));
		}

		//Modifiers should send Modifiable, single connector
		TEST_METHOD(Modifiers)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			ConnectorTester tester(std::make_unique<ParticleSystem>(file));
			tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);
			IModifiable* ifc = tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);

			Assert::IsNotNull(ifc);

			//Test the effect of interacting with the interface

			//Inserting a modifier should place it in the native sequence. 
			//Not necessarily in the indicated position, but order of subsequent insertions must be respected.
			auto mod1 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod1.get());
			ifc->modifiers().insert(-1, *mod1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod1) != -1);

			auto mod2 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod2.get());
			ifc->modifiers().insert(-1, *mod2);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod2) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod2) > tester.getNode()->object().modifiers().find(*mod1));

			auto mod3 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod3.get());
			ifc->modifiers().insert(0, *mod3);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) < tester.getNode()->object().modifiers().find(*mod1));

			//Searching for a modifier through the interface must yield its order in the native sequence
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod1) == ifc->modifiers().find(*mod1));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod2) == ifc->modifiers().find(*mod2));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) == ifc->modifiers().find(*mod3));

			//Erasing must also affect the native sequence
			ifc->modifiers().erase(ifc->modifiers().find(*mod1));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod1) == -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod2) != -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) != -1);
			ifc->modifiers().erase(ifc->modifiers().find(*mod2));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod2) == -1);
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) != -1);
			ifc->modifiers().erase(ifc->modifiers().find(*mod3));
			Assert::IsTrue(tester.getNode()->object().modifiers().find(*mod3) == -1);

			//Similary with controllers
			auto ctlr1 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr1.get());
			ifc->controllers().insert(-1, *ctlr1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr1) != -1);

			auto ctlr2 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr2.get());
			ifc->controllers().insert(-1, *ctlr2);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr2) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr2) > tester.getNode()->object().controllers().find(*ctlr1));

			auto ctlr3 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr3.get());
			ifc->controllers().insert(0, *ctlr3);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) < tester.getNode()->object().controllers().find(*ctlr1));

			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr1) == ifc->controllers().find(*ctlr1));
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr2) == ifc->controllers().find(*ctlr2));
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) == ifc->controllers().find(*ctlr3));

			ifc->controllers().erase(ifc->controllers().find(*ctlr1));
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr1) == -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr2) != -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) != -1);
			ifc->controllers().erase(ifc->controllers().find(*ctlr2));
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr2) == -1);
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) != -1);
			ifc->controllers().erase(ifc->controllers().find(*ctlr3));
			Assert::IsTrue(tester.getNode()->object().controllers().find(*ctlr3) == -1);

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
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto obj = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(obj.get());
			auto data = file.create<nif::NiPSysData>();
			Assert::IsNotNull(data.get());

			ReservableSequence<nif::NiPSysModifier> mods(obj->modifiers());
			ReservableSequence<nif::NiTimeController> ctlrs(obj->controllers());

			auto modObj = file.create<nif::NiPSysBoundUpdateModifier>();
			Assert::IsNotNull(modObj.get());
			auto ctlrObj = file.create<nif::NiPSysUpdateCtlr>();
			Assert::IsNotNull(ctlrObj.get());
			UpdateRequirement upd(*modObj, *ctlrObj, mods, ctlrs);
			upd.incr();

			//Modifier should be assigned to the right target
			Assert::IsTrue(upd.modifier().target().isAssigned(obj.get()));

			//Bound update modifier + update controller should always be last
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 0);
			Assert::IsTrue(obj->controllers().find(upd.controller()) == 0);
			//And name should match position
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:0");

			//Insert a mod+ctlr at the end
			auto mod1 = file.create<nif::NiPSysBoxEmitter>();
			Assert::IsNotNull(mod1.get());
			auto ctlr1 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr1.get());
			mods.insert(-1, *mod1);
			ctlrs.insert(-1, *ctlr1);

			//Position and name of update mod should have changed
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 1);
			Assert::IsTrue(obj->controllers().find(upd.controller()) == 1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:1");

			//Add a movement requirement
			auto posObj = file.create<nif::NiPSysPositionModifier>();
			Assert::IsNotNull(posObj.get());
			MovementRequirement mov(*posObj, mods);
			mov.incr();
			Assert::IsTrue(mov.modifier().target().isAssigned(obj.get()));

			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 2);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == 1);

			Assert::IsTrue(upd.modifier().name().get() == "Modifier:2");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:1");

			//Insert another mod at the end
			auto mod2 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod2.get());
			mods.insert(-1, *mod2);

			//Position mod should be second last, update last
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 3);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == 2);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:2");

			//Add a lifetime requirement
			auto ageObj = file.create<nif::NiPSysAgeDeathModifier>();
			Assert::IsNotNull(ageObj.get());
			LifetimeRequirement liv(*ageObj, mods);
			liv.incr();
			Assert::IsTrue(liv.modifier().target().isAssigned(obj.get()));

			//It should insert at the beginning
			Assert::IsTrue(obj->modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");
			//And the others should hear it
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:3");

			//Insert a mod at the beginning
			auto mod3 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod3.get());
			mods.insert(0, *mod3);

			//and verify positions and names
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 5);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == 4);
			Assert::IsTrue(obj->modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:5");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");

			//Remove a mod and make sure no one does anything odd
			mods.erase(2);
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 4);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == 3);
			Assert::IsTrue(obj->modifiers().find(liv.modifier()) == 0);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:4");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(liv.modifier().name().get() == "Modifier:0");

			//Remove the lifetime requirement
			liv.decr();
			Assert::IsFalse(liv.modifier().target().isAssigned(obj.get()));

			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 3);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == 2);
			Assert::IsTrue(obj->modifiers().find(liv.modifier()) == -1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:3");
			Assert::IsTrue(mov.modifier().name().get() == "Modifier:2");

			//Remove the position requirement
			mov.decr();
			Assert::IsFalse(mov.modifier().target().isAssigned(obj.get()));

			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == 2);
			Assert::IsTrue(obj->modifiers().find(mov.modifier()) == -1);
			Assert::IsTrue(upd.modifier().name().get() == "Modifier:2");

			//Remove the update requirement
			upd.decr();
			Assert::IsFalse(upd.modifier().target().isAssigned(obj.get()));
			Assert::IsTrue(obj->modifiers().find(upd.modifier()) == -1);
		}

		TEST_METHOD(Load_regular)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			auto data = file.create<nif::NiPSysData>();
			Assert::IsNotNull(data.get());
			psys->data().assign(data.get());

			auto alpha = file.create<nif::NiAlphaProperty>();
			Assert::IsNotNull(alpha.get());
			psys->alphaProperty().assign(alpha.get());

			auto ctlr = file.create<nif::NiPSysUpdateCtlr>();
			Assert::IsNotNull(ctlr.get());
			psys->controllers().insert(0, *ctlr);

			auto mod0 = file.create<nif::NiPSysAgeDeathModifier>();
			Assert::IsNotNull(mod0.get());
			psys->modifiers().insert(-1, *mod0);

			auto mod1 = file.create<nif::NiPSysPositionModifier>();
			Assert::IsNotNull(mod1.get());
			psys->modifiers().insert(-1, *mod1);

			auto mod2 = file.create<nif::NiPSysBoundUpdateModifier>();
			Assert::IsNotNull(mod2.get());
			psys->modifiers().insert(-1, *mod2);

			Constructor c(file);
			c.makeRoot(&root->getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), *psys);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, psys_node->getField(ParticleSystem::PARENT)->connector));

			//Are names and ordering as expected?
			Assert::IsTrue(psys_node->ageDeathMod().name().get() == "Modifier:0");
			Assert::IsTrue(psys_node->ageDeathMod().order().get() == 0);
			Assert::IsTrue(psys_node->positionMod().name().get() == "Modifier:1");
			Assert::IsTrue(psys_node->positionMod().order().get() == 1);
			Assert::IsTrue(psys_node->boundUpdateMod().name().get() == "Modifier:2");
			Assert::IsTrue(psys_node->boundUpdateMod().order().get() == 2);

			//Did we mess up the backend?
			Assert::IsTrue(psys_node->object().getNative().GetData() == &data->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetBSProperty(1) == &alpha->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetControllers().size() == 1);
			Assert::IsTrue(psys_node->object().getNative().GetControllers().front() == &ctlr->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 3);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[0] == &mod0->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod1->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod2->getNative());

			//Was a warning generated?
			Assert::IsFalse(!c.warnings().empty());
		}

		//If critical components are missing, they should be added
		TEST_METHOD(Load_incomplete)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			Constructor c(file);
			c.makeRoot(&root->getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), *psys);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, psys_node->getField(ParticleSystem::PARENT)->connector));

			//Are names and ordering as expected?
			Assert::IsTrue(psys_node->ageDeathMod().name().get() == "Modifier:0");
			Assert::IsTrue(psys_node->ageDeathMod().order().get() == 0);
			Assert::IsTrue(psys_node->positionMod().name().get() == "Modifier:1");
			Assert::IsTrue(psys_node->positionMod().order().get() == 1);
			Assert::IsTrue(psys_node->boundUpdateMod().name().get() == "Modifier:2");
			Assert::IsTrue(psys_node->boundUpdateMod().order().get() == 2);

			//Have the missing components been added?
			Assert::IsTrue(psys_node->object().getNative().GetData() != nullptr);
			Assert::IsTrue(psys_node->object().getNative().GetBSProperty(1) != nullptr);
			Assert::IsTrue(psys_node->object().getNative().GetControllers().size() == 1);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 3);//should have received AgeDeath, Position, BoundUpdate

			//Was a warning generated?
			Assert::IsTrue(!c.warnings().empty());
		}

		//If the critical components are in an unexpected order, they should silently be reordered
		TEST_METHOD(Load_irregular)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			auto data = file.create<nif::NiPSysData>();
			Assert::IsNotNull(data.get());
			psys->data().assign(data.get());

			auto alpha = file.create<nif::NiAlphaProperty>();
			Assert::IsNotNull(alpha.get());
			psys->alphaProperty().assign(alpha.get());

			auto ctlr = file.create<nif::NiPSysUpdateCtlr>();
			Assert::IsNotNull(ctlr.get());
			psys->controllers().insert(0, *ctlr);

			//Add the fundamental modifiers in an order we don't expect
			auto mod1 = file.create<nif::NiPSysAgeDeathModifier>();
			Assert::IsNotNull(mod1.get());
			psys->modifiers().insert(0, *mod1);

			auto mod2 = file.create<nif::NiPSysPositionModifier>();
			Assert::IsNotNull(mod2.get());
			psys->modifiers().insert(0, *mod2);

			auto mod3 = file.create<nif::NiPSysBoundUpdateModifier>();
			Assert::IsNotNull(mod3.get());
			psys->modifiers().insert(0, *mod3);

			Constructor c(file);
			c.makeRoot(&root->getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), *psys);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, psys_node->getField(ParticleSystem::PARENT)->connector));

			//Are names and ordering as expected?
			Assert::IsTrue(psys_node->ageDeathMod().name().get() == "Modifier:0");
			Assert::IsTrue(psys_node->ageDeathMod().order().get() == 0);
			Assert::IsTrue(psys_node->positionMod().name().get() == "Modifier:1");
			Assert::IsTrue(psys_node->positionMod().order().get() == 1);
			Assert::IsTrue(psys_node->boundUpdateMod().name().get() == "Modifier:2");
			Assert::IsTrue(psys_node->boundUpdateMod().order().get() == 2);

			//Have the modifiers been reordered?
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 3);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[0] == &mod1->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod2->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod3->getNative());

			//Was a warning generated? We expect not, but maybe we should?
			Assert::IsFalse(!c.warnings().empty());
		}
	};
}
