#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes_internal.h"

namespace conversions
{
	TEST_CLASS(TextureAtlasLayout)
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
				auto offsets = node_conversion<std::vector<nif::SubtextureOffset>>::from(cnt);
				Assert::IsTrue(cnt[0] * cnt[1] == offsets.size());
				Assert::IsTrue(node_conversion<nif::SubtextureCount>::from(offsets) == cnt);
			}

			//Special cases
			Assert::IsTrue(node_conversion<std::vector<nif::SubtextureOffset>>::from(
				nif::SubtextureCount{ 1, 1 }).empty());
			Assert::IsTrue(node_conversion<nif::SubtextureCount>::from(
				std::vector<nif::SubtextureOffset>()) == nif::SubtextureCount{ 1, 1 });

			Assert::IsTrue(node_conversion<std::vector<nif::SubtextureOffset>>::from(
				nif::SubtextureCount{ 0, 0 }).empty());
			std::uniform_real_distribution<float> Df;
			std::vector<nif::SubtextureOffset> irregular{
				{ Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) },
				{ Df(m_engine), Df(m_engine), Df(m_engine), Df(m_engine) } };
			Assert::IsTrue(node_conversion<nif::SubtextureCount>::from(irregular) == nif::SubtextureCount{ 0, 0 });
		}
	};
}

namespace connectors
{
	using namespace nif;
	using namespace node;

	TEST_CLASS(ParticleSystemTests)
	{
	public:
		TEST_METHOD(Shader)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiParticleSystem>();
			AssignableSenderTest(Default<ParticleSystem>{}.create(file, ni_ptr<NiParticleSystem>(obj)), obj->shaderProperty, ParticleSystem::SHADER, false);
		}

		//Modifiers should send Modifiable, single connector.
		//Split into smaller tests! We're testing *everything* here.
		TEST_METHOD(Modifiers)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiParticleSystem>();
			auto data = file.create<NiPSysData>();
			auto adm = file.create<NiPSysAgeDeathModifier>();
			auto bum = file.create<NiPSysBoundUpdateModifier>();
			auto pm = file.create<NiPSysPositionModifier>();
			auto puc = file.create<NiPSysUpdateCtlr>();

			ConnectorTester tester(Default<ParticleSystem>{}.create(file, ni_ptr<NiParticleSystem>(obj), 
				ni_ptr<NiPSysData>(data), nullptr, ni_ptr<NiPSysAgeDeathModifier>(adm), ni_ptr<NiPSysBoundUpdateModifier>(bum), 
				ni_ptr<NiPSysPositionModifier>(pm), ni_ptr<NiPSysUpdateCtlr>(puc)));
			tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);
			IModifiable* ifc = tester.tryConnect<IModifiable, void>(ParticleSystem::MODIFIERS, false, nullptr);

			Assert::IsNotNull(ifc);

			//Test the effect of interacting with the interface

			//Adding a modifier should place it in the native sequence, in increasing order.
			//Target and order should be set.
			//Static modifiers should not move.
			Assert::IsTrue(obj->modifiers.find(adm.get()) == 0);
			Assert::IsTrue(adm->order.get() == 0);
			Assert::IsTrue(adm->name.get() == "Modifier:0");
			Assert::IsTrue(obj->modifiers.find(pm.get()) == 1);
			Assert::IsTrue(pm->order.get() == 1);
			Assert::IsTrue(pm->name.get() == "Modifier:1");
			Assert::IsTrue(obj->modifiers.find(bum.get()) == 2);
			Assert::IsTrue(bum->order.get() == 2);
			Assert::IsTrue(bum->name.get() == "Modifier:2");

			//Add a mod
			auto mod1 = file.create<NiPSysRotationModifier>();
			Assert::IsNotNull(mod1.get());
			ifc->addModifier(mod1);
			int order1 = obj->modifiers.find(mod1.get());
			Assert::IsTrue(order1 >= 0 && order1 == mod1->order.get());
			Assert::IsTrue(mod1->target.assigned() == obj);

			Assert::IsTrue(obj->modifiers.find(adm.get()) == 0);
			Assert::IsTrue(adm->order.get() == 0);
			Assert::IsTrue(adm->name.get() == "Modifier:0");
			Assert::IsTrue(adm->target.assigned() == obj);
			Assert::IsTrue(obj->modifiers.find(pm.get()) == 2);
			Assert::IsTrue(pm->order.get() == 2);
			Assert::IsTrue(pm->name.get() == "Modifier:2");
			Assert::IsTrue(pm->target.assigned() == obj);
			Assert::IsTrue(obj->modifiers.find(bum.get()) == 3);
			Assert::IsTrue(bum->order.get() == 3);
			Assert::IsTrue(bum->name.get() == "Modifier:3");
			Assert::IsTrue(bum->target.assigned() == obj);

			//Add another
			auto mod2 = file.create<nif::NiPSysRotationModifier>();
			Assert::IsNotNull(mod2.get());
			ifc->addModifier(mod2);
			int order2 = obj->modifiers.find(mod2.get());
			Assert::IsTrue(order1 == obj->modifiers.find(mod1.get()));
			Assert::IsTrue(order2 >= 0 && order2 == mod2->order.get());
			Assert::IsTrue(order2 == order1 + 1);
			Assert::IsTrue(mod2->target.assigned() == obj);

			Assert::IsTrue(obj->modifiers.find(adm.get()) == 0);
			Assert::IsTrue(adm->order.get() == 0);
			Assert::IsTrue(adm->name.get() == "Modifier:0");
			Assert::IsTrue(obj->modifiers.find(pm.get()) == 3);
			Assert::IsTrue(pm->order.get() == 3);
			Assert::IsTrue(pm->name.get() == "Modifier:3");
			Assert::IsTrue(obj->modifiers.find(bum.get()) == 4);
			Assert::IsTrue(bum->order.get() == 4);
			Assert::IsTrue(bum->name.get() == "Modifier:4");

			//Removing must also affect the native sequence, and set target and order.
			ifc->removeModifier(mod1.get());
			Assert::IsTrue(obj->modifiers.find(mod1.get()) == -1);
			Assert::IsTrue(!mod1->target.assigned());
			Assert::IsTrue(mod1->order.get() == -1);

			Assert::IsTrue(obj->modifiers.find(adm.get()) == 0);
			Assert::IsTrue(adm->order.get() == 0);
			Assert::IsTrue(adm->name.get() == "Modifier:0");
			Assert::IsTrue(obj->modifiers.find(pm.get()) == 2);
			Assert::IsTrue(pm->order.get() == 2);
			Assert::IsTrue(pm->name.get() == "Modifier:2");
			Assert::IsTrue(obj->modifiers.find(bum.get()) == 3);
			Assert::IsTrue(bum->order.get() == 3);
			Assert::IsTrue(bum->name.get() == "Modifier:3");

			ifc->removeModifier(mod2.get());
			Assert::IsTrue(obj->modifiers.find(mod2.get()) == -1);
			Assert::IsTrue(!mod2->target.assigned());
			Assert::IsTrue(mod2->order.get() == -1);

			Assert::IsTrue(obj->modifiers.find(adm.get()) == 0);
			Assert::IsTrue(adm->order.get() == 0);
			Assert::IsTrue(adm->name.get() == "Modifier:0");
			Assert::IsTrue(obj->modifiers.find(pm.get()) == 1);
			Assert::IsTrue(pm->order.get() == 1);
			Assert::IsTrue(pm->name.get() == "Modifier:1");
			Assert::IsTrue(obj->modifiers.find(bum.get()) == 2);
			Assert::IsTrue(bum->order.get() == 2);
			Assert::IsTrue(bum->name.get() == "Modifier:2");

			//Similary with controllers
			Assert::IsTrue(obj->controllers.find(puc.get()) == 0);
			Assert::IsTrue(puc->target.assigned() == obj);

			auto ctlr1 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr1.get());
			ifc->addController(ctlr1);
			Assert::IsTrue(obj->controllers.find(ctlr1.get()) >= 0);
			Assert::IsTrue(ctlr1->target.assigned() == obj);

			Assert::IsTrue(obj->controllers.find(puc.get()) == 1);

			auto ctlr2 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr2.get());
			ifc->addController(ctlr2);
			Assert::IsTrue(obj->controllers.find(ctlr2.get()) >= 0);
			Assert::IsTrue(obj->controllers.find(ctlr2.get()) == obj->controllers.find(ctlr1.get()) + 1);

			Assert::IsTrue(obj->controllers.find(puc.get()) == 2);

			ifc->removeController(ctlr1.get());
			Assert::IsTrue(obj->controllers.find(ctlr1.get()) == -1);
			Assert::IsTrue(!ctlr1->target.assigned());

			Assert::IsTrue(obj->controllers.find(puc.get()) == 1);

			ifc->removeController(ctlr2.get());
			Assert::IsTrue(obj->controllers.find(ctlr2.get()) == -1);
			Assert::IsTrue(!ctlr2->target.assigned());

			Assert::IsTrue(obj->controllers.find(puc.get()) == 0);

			//Adding a requirement should set the appropriate flag
			Assert::IsFalse(data->hasColour.get());

			ifc->addRequirement(ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->addRequirement(ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->removeRequirement(ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->removeRequirement(ModRequirement::COLOUR);
			Assert::IsFalse(data->hasColour.get());

			Assert::IsFalse(data->hasRotationAngles.get());
			Assert::IsFalse(data->hasRotationSpeeds.get());

			ifc->addRequirement(ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());
			ifc->addRequirement(ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());

			ifc->removeRequirement(ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());
			ifc->removeRequirement(ModRequirement::ROTATION);
			Assert::IsFalse(data->hasRotationAngles.get());
			Assert::IsFalse(data->hasRotationSpeeds.get());
		}
	};
}

namespace node
{
	using namespace nif;

	TEST_CLASS(ParticleSystemTests)
	{
	public:
		std::mt19937 m_engine;

		//Editing the texure atlas layout widget should set subtexture offsets in the PSysData
		TEST_METHOD(TextureAtlasLayout)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };
			auto data = file.create<NiPSysData>();
			auto node = Default<ParticleSystem>{}.create(file, nullptr, ni_ptr<NiPSysData>(data));

			Assert::IsTrue(node->subtexCount().get() == SubtextureCount{ 1, 1 });

			SubtextureCount cnt{ 3, 5 };
			node->subtexCount().set(cnt);
			Assert::IsTrue(node->subtexCount().get() == cnt);

			Assert::IsTrue(node_conversion<SubtextureCount>::from(data->subtexOffsets.get()) == cnt);
		}

		/* These should be tests of Default, not Constructor
		TEST_METHOD(Load_regular)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
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
			c.makeRoot();

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
			Assert::IsTrue(mod0->name().get() == "Modifier:0");
			Assert::IsTrue(mod0->order().get() == 0);
			Assert::IsTrue(mod1->name().get() == "Modifier:1");
			Assert::IsTrue(mod1->order().get() == 1);
			Assert::IsTrue(mod2->name().get() == "Modifier:2");
			Assert::IsTrue(mod2->order().get() == 2);

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

			auto root = file.getRoot();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			Constructor c(file);
			c.makeRoot();

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
			auto&& mods = psys_node->object().getNative().GetModifiers();
			Assert::IsTrue(mods.size() == 3);
			Niflib::NiPSysModifier* mod0 = mods[0];
			Assert::IsTrue(mod0->IsSameType(Niflib::NiPSysAgeDeathModifier::TYPE));
			Assert::IsTrue(mod0->GetName() == "Modifier:0");
			Assert::IsTrue(mod0->GetOrder() == 0);

			Niflib::NiPSysModifier* mod1 = mods[1];
			Assert::IsTrue(mod1->IsSameType(Niflib::NiPSysPositionModifier::TYPE));
			Assert::IsTrue(mod1->GetName() == "Modifier:1");
			Assert::IsTrue(mod1->GetOrder() == 1);

			Niflib::NiPSysModifier* mod2 = mods[2];
			Assert::IsTrue(mod2->IsSameType(Niflib::NiPSysBoundUpdateModifier::TYPE));
			Assert::IsTrue(mod2->GetName() == "Modifier:2");
			Assert::IsTrue(mod2->GetOrder() == 2);

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

			auto root = file.getRoot();
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
			c.makeRoot();

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
			Assert::IsTrue(mod1->name().get() == "Modifier:0");
			Assert::IsTrue(mod1->order().get() == 0);
			Assert::IsTrue(mod2->name().get() == "Modifier:1");
			Assert::IsTrue(mod2->order().get() == 1);
			Assert::IsTrue(mod3->name().get() == "Modifier:2");
			Assert::IsTrue(mod3->order().get() == 2);

			//Have the modifiers been reordered?
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 3);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[0] == &mod1->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod2->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod3->getNative());

			//Was a warning generated? We expect not, but maybe we should?
			Assert::IsFalse(!c.warnings().empty());
		}
		*/
	};
}
