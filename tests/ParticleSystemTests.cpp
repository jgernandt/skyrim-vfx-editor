//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor, a program for creating visual effects
//in the NetImmerse format.
//
//SVFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SVFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SVFX Editor. If not, see <https://www.gnu.org/licenses/>.

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
		
	};
}

namespace nodes
{
	using namespace nif;

	TEST_CLASS(ParticleSystem)
	{
	public:
		std::mt19937 m_engine;

		//Editing the texure atlas layout widget should set subtexture offsets in the PSysData
		TEST_METHOD(TextureAtlasLayout)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };
			auto data = file.create<NiPSysData>();
			auto node = node::Default<node::ParticleSystem>{}.create(file, nullptr, data);

			Assert::IsTrue(node->subtexCount().get() == SubtextureCount{ 1, 1 });

			SubtextureCount cnt{ 3, 5 };
			node->subtexCount().set(cnt);
			Assert::IsTrue(node->subtexCount().get() == cnt);

			Assert::IsTrue(node_conversion<SubtextureCount>::from(data->subtexOffsets.get()) == cnt);
		}

		
		TEST_METHOD(Default_complete)
		{
			//Set up a particle system the way we will make them
			File file{ File::Version::SKYRIM_SE };
			auto psys = file.create<NiParticleSystem>();
			auto data = file.create<NiPSysData>();
			auto alpha = file.create<NiAlphaProperty>();
			auto mod0 = file.create<NiPSysAgeDeathModifier>();
			auto mod1 = file.create<NiPSysGravityModifier>();
			auto mod2 = file.create<NiPSysRotationModifier>();
			auto mod3 = file.create<NiPSysPositionModifier>();
			auto mod4 = file.create<NiPSysBoundUpdateModifier>();
			auto ctlr = file.create<NiTimeController>();
			auto puc = file.create<NiPSysUpdateCtlr>();

			psys->data.assign(data);
			psys->alphaProperty.assign(alpha);

			ni_ptr<NiPSysModifier> mods[5]{ mod0, mod1, mod2, mod3, mod4 };
			for (int i = 0; i < 5; i++) {
				psys->modifiers.insert(psys->modifiers.size(), mods[i]);
				mods[i]->target.assign(psys);
				mods[i]->order.set(i);
				mods[i]->name.set(std::string("Modifier:") + std::to_string(i));
			}
			ni_ptr<NiTimeController> ctlrs[2]{ ctlr, puc };
			for (int i = 0; i < 2; i++) {
				psys->controllers.insert(psys->controllers.size(), ctlrs[i]);
				ctlrs[i]->target.assign(psys);
			}

			//Construct a node from it
			auto node = node::Default<node::ParticleSystem>{}.create(file, psys, data, alpha, mod0, mod4, mod3, puc);

			//Make sure we didn't break it
			Assert::IsTrue(psys->data.assigned() == data);
			Assert::IsTrue(psys->alphaProperty.assigned() == alpha);

			for (int i = 0; i < 5; i++) {
				Assert::IsTrue(psys->modifiers.at(i) == mods[i]);
				Assert::IsTrue(mods[i]->order.get() == i);
				Assert::IsTrue(mods[i]->name.get() == std::string("Modifier:") + std::to_string(i));
			}
			for (int i = 0; i < 2; i++)
				Assert::IsTrue(psys->controllers.at(i) == ctlrs[i]);
		}

		//Existing psys with modifiers in unexpected order
		TEST_METHOD(Default_disorder)
		{
			//Set up a particle system with modifiers in the wrong order
			File file{ File::Version::SKYRIM_SE };
			auto psys = file.create<NiParticleSystem>();
			auto mod0 = file.create<NiPSysGravityModifier>();
			auto mod1 = file.create<NiPSysBoundUpdateModifier>();
			auto mod2 = file.create<NiPSysPositionModifier>();
			auto mod3 = file.create<NiPSysRotationModifier>();
			auto mod4 = file.create<NiPSysAgeDeathModifier>();

			//We expect that Factory will have sorted the modifiers in order
			ni_ptr<NiPSysModifier> mods[5]{ mod0, mod1, mod2, mod3, mod4 };
			for (int i = 0; i < 5; i++) {
				psys->modifiers.insert(psys->modifiers.size(), mods[i]);
				mods[i]->order.set(i);
				mods[i]->target.assign(psys);
			}

			//Construct a node from it
			auto node = node::Default<node::ParticleSystem>{}.create(file, psys, nullptr, nullptr, mod4, mod1, mod2);

			//Make sure it has been reordered
			ni_ptr<NiPSysModifier> expected[5]{ mod4, mod0, mod3, mod2, mod1 };
			std::string names[5]{ "Modifier:0", "", "", "Modifier:3", "Modifier:4" };
			for (int i = 0; i < 5; i++) {
				Assert::IsTrue(psys->modifiers.at(i) == expected[i]);
				Assert::IsTrue(expected[i]->order.get() == i);
				Assert::IsTrue(expected[i]->name.get() == names[i]);
			}
		}

		//Existing psys with critical components missing
		TEST_METHOD(Default_incomplete)
		{
			File file{ File::Version::SKYRIM_SE };
			auto psys = file.create<NiParticleSystem>();
			auto mod0 = file.create<NiPSysGravityModifier>();
			auto mod1 = file.create<NiPSysRotationModifier>();
			auto ctlr = file.create<NiTimeController>();

			psys->controllers.insert(0, ctlr);

			//We expect that Factory will have sorted the modifiers in order
			ni_ptr<NiPSysModifier> mods[2]{ mod0, mod1 };
			for (int i = 0; i < 2; i++) {
				psys->modifiers.insert(psys->modifiers.size(), mods[i]);
				mods[i]->order.set(i);
			}

			//Construct a node from it
			auto node = node::Default<node::ParticleSystem>{}.create(file, psys);

			//Make sure it has been completed
			Assert::IsTrue(psys->data.assigned() != nullptr);
			Assert::IsTrue(psys->alphaProperty.assigned() != nullptr);

			Assert::IsTrue(psys->modifiers.size() == 5);

			Assert::IsTrue(psys->modifiers.at(0)->type() == NiPSysAgeDeathModifier::TYPE);
			Assert::IsTrue(psys->modifiers.at(0)->order.get() == 0);
			Assert::IsTrue(psys->modifiers.at(0)->name.get() == "Modifier:0");

			Assert::IsTrue(psys->modifiers.at(3)->type() == NiPSysPositionModifier::TYPE);
			Assert::IsTrue(psys->modifiers.at(3)->order.get() == 3);
			Assert::IsTrue(psys->modifiers.at(3)->name.get() == "Modifier:3");

			Assert::IsTrue(psys->modifiers.at(4)->type() == NiPSysBoundUpdateModifier::TYPE);
			Assert::IsTrue(psys->modifiers.at(4)->order.get() == 4);
			Assert::IsTrue(psys->modifiers.at(4)->name.get() == "Modifier:4");

			Assert::IsTrue(psys->controllers.size() == 2);
			Assert::IsTrue(psys->controllers.at(1)->type() == NiPSysUpdateCtlr::TYPE);
		}

		TEST_METHOD(Connector_Shader)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiParticleSystem>();
			AssignableSenderTest(node::Default<node::ParticleSystem>{}.create(file, obj), obj->shaderProperty, node::ParticleSystem::SHADER, false);
		}

		//Modifiers should send Modifiable, single connector.
		//Split into smaller tests! We're testing *everything* here.
		TEST_METHOD(Connector_Modifiers)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiParticleSystem>();
			auto data = file.create<NiPSysData>();
			auto adm = file.create<NiPSysAgeDeathModifier>();
			auto bum = file.create<NiPSysBoundUpdateModifier>();
			auto pm = file.create<NiPSysPositionModifier>();
			auto puc = file.create<NiPSysUpdateCtlr>();

			ConnectorTester tester(node::Default<node::ParticleSystem>{}.create(file, obj, data, nullptr, adm, bum, pm, puc));
			tester.tryConnect<node::IModifiable, void>(node::ParticleSystem::MODIFIERS, false, nullptr);
			node::IModifiable* ifc = tester.tryConnect<node::IModifiable, void>(node::ParticleSystem::MODIFIERS, false, nullptr);

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
			auto mod2 = file.create<NiPSysRotationModifier>();
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

			ifc->addRequirement(node::ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->addRequirement(node::ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->removeRequirement(node::ModRequirement::COLOUR);
			Assert::IsTrue(data->hasColour.get());
			ifc->removeRequirement(node::ModRequirement::COLOUR);
			Assert::IsFalse(data->hasColour.get());

			Assert::IsFalse(data->hasRotationAngles.get());
			Assert::IsFalse(data->hasRotationSpeeds.get());

			ifc->addRequirement(node::ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());
			ifc->addRequirement(node::ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());

			ifc->removeRequirement(node::ModRequirement::ROTATION);
			Assert::IsTrue(data->hasRotationAngles.get());
			Assert::IsTrue(data->hasRotationSpeeds.get());
			ifc->removeRequirement(node::ModRequirement::ROTATION);
			Assert::IsFalse(data->hasRotationAngles.get());
			Assert::IsFalse(data->hasRotationSpeeds.get());
		}
	};
}
