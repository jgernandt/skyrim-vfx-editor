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
#include "ModifiersTests.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(Emitter)
	{
		std::mt19937 m_engine;

	public:

		//Emitters should send a colour requirement if their colour is non-white
		TEST_METHOD(ColourRequirement)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysBoxEmitter>();
			MockModifiable target;
			TestRoot root;
			auto node = node::Default<node::BoxEmitter>{}.create(file, obj);
			auto mod1 = node.get();
			root.addChild(std::move(node));

			//Set to white before connecting
			obj->colour.set(COL_WHITE);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (node::Field* f1 = mod1->getField(node::Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			//No colour requirement should have been sent
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to non-white. Requirement should be sent.
			obj->colour.set(COL_RED);
			Assert::IsTrue(target.reqsAdded.size() == 1 && target.reqsAdded[0] == node::ModRequirement::COLOUR);
			target.reqsAdded.clear();
			Assert::IsTrue(target.reqsRemoved.empty());

			//Reset same. No additional requirement should be sent.
			obj->colour.set(COL_RED);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set other non-white. No additional requirement should be sent.
			obj->colour.set(COL_BLUE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to white. Requirement should be removed.
			obj->colour.set(COL_WHITE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.size() == 1 && target.reqsRemoved[0] == node::ModRequirement::COLOUR);
			target.reqsRemoved.clear();

			//Reset to white. Requirement should not be removed again.
			obj->colour.set(COL_WHITE);
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Other properties should not require colour
			obj->lifeSpan.set(1.0f);
			obj->lifeSpanVar.set(1.0f);
			obj->size.set(1.0f);
			obj->sizeVar.set(1.0f);
			obj->speed.set(1.0f);
			obj->speedVar.set(1.0f);
			obj->azimuth.set(math::degf(90.0f));
			obj->azimuthVar.set(math::degf(90.0f));
			obj->elevation.set(math::degf(90.0f));
			obj->elevationVar.set(math::degf(90.0f));
			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.empty());

			//Set to non-white and disconnect. Requirement should be removed.
			obj->colour.set(COL_RED);
			target.reqsAdded.clear();
			target.reqsRemoved.clear();

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.size() == 1 && target.reqsRemoved[0] == node::ModRequirement::COLOUR);
			target.reqsRemoved.clear();

			//Connecting with non-white should send requirement
			if (c1 && c2) {
				c1->onClick();
				c2->onRelease();
			}

			Assert::IsTrue(target.reqsAdded.size() == 1 && target.reqsAdded[0] == node::ModRequirement::COLOUR);
			target.reqsAdded.clear();
			Assert::IsTrue(target.reqsRemoved.empty());
		}

		//Birth rate should send Assignable<NiInterpolator> and receive IController<float> (single)
		TEST_METHOD(Connector_BirthRate)
		{
			class MockController : public node::IController<float>
			{
			public:
				virtual FlagSet<ControllerFlags>& flags() override { return m_flags; }
				virtual Property<float>& frequency() override { return m_frequency; }
				virtual Property<float>& phase() override { return m_phase; }
				virtual Property<float>& startTime() override { return m_startTime; }
				virtual Property<float>& stopTime() override { return m_stopTime; }

			private:
				FlagSet<ControllerFlags> m_flags;
				Property<float> m_frequency;
				Property<float> m_phase;
				Property<float> m_startTime;
				Property<float> m_stopTime;
			};

			File file{ File::Version::SKYRIM_SE };
			auto ctlr = file.create<NiPSysEmitterCtlr>();


			MockController target0;
			MockController target;
			ConnectorTester<node::Emitter> tester(node::Default<node::BoxEmitter>{}.create(file, nullptr, ctlr));

			tester.tryConnect<Ref<NiInterpolator>, node::IController<float>>(node::Emitter::BIRTH_RATE, false, &target0);
			auto ifc = tester.tryConnect<Ref<NiInterpolator>, node::IController<float>>(node::Emitter::BIRTH_RATE, false, &target);
			Assert::IsNotNull(ifc);

			//Setting the properties on target (but not target0) should set the corresponding on ctlr
			std::uniform_int_distribution<unsigned short> I;
			std::uniform_real_distribution<float> F;

			target0.flags().clear(-1);//better clear any defaults
			target0.flags().raise(I(m_engine));
			target0.frequency().set(F(m_engine));
			target0.phase().set(F(m_engine));
			target0.startTime().set(F(m_engine));
			target0.stopTime().set(F(m_engine));
			Assert::IsFalse(ctlr->flags.raised() == target0.flags().raised());
			Assert::IsFalse(ctlr->frequency.get() == target0.frequency().get());
			Assert::IsFalse(ctlr->phase.get() == target0.phase().get());
			Assert::IsFalse(ctlr->startTime.get() == target0.startTime().get());
			Assert::IsFalse(ctlr->stopTime.get() == target0.stopTime().get());

			target.flags().clear(-1);//better clear any defaults
			target.flags().raise(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			Assert::IsTrue(ctlr->flags.raised() == target.flags().raised());
			Assert::IsTrue(ctlr->frequency.get() == target.frequency().get());
			Assert::IsTrue(ctlr->phase.get() == target.phase().get());
			Assert::IsTrue(ctlr->startTime.get() == target.startTime().get());
			Assert::IsTrue(ctlr->stopTime.get() == target.stopTime().get());

			//Assigning to the interface should assign to ctlr->interpolator
			auto iplr = file.create<NiFloatInterpolator>();
			Assert::IsNotNull(iplr.get());
			ifc->assign(iplr);
			Assert::IsTrue(ctlr->interpolator.assigned() == iplr);
			ifc->assign(nullptr);
			Assert::IsTrue(ctlr->interpolator.assigned() == nullptr);

			//Make sure listeners are removed
			tester.disconnect<node::IController<float>>(&target);

			//these calls should not reach the node (defaults should have been restored)
			target.flags().clear(-1);
			target.flags().raise(I(m_engine));
			target.frequency().set(F(m_engine));
			target.phase().set(F(m_engine));
			target.startTime().set(F(m_engine));
			target.stopTime().set(F(m_engine));
			Assert::IsTrue(ctlr->flags.raised() == node::DEFAULT_CTLR_FLAGS);
			Assert::IsTrue(ctlr->frequency.get() == node::DEFAULT_FREQUENCY);
			Assert::IsTrue(ctlr->phase.get() == node::DEFAULT_PHASE);
			Assert::IsTrue(ctlr->startTime.get() == node::DEFAULT_STARTTIME);
			Assert::IsTrue(ctlr->stopTime.get() == node::DEFAULT_STOPTIME);
		}

		/*
		TEST_METHOD(Load_Emitters)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
			Assert::IsNotNull(root.get());

			auto psys = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys.get());
			root->children().add(*psys);

			//Complete emitter
			auto mod0 = file.create<nif::NiPSysBoxEmitter>();
			Assert::IsNotNull(mod0.get());
			mod0->name().set("BoxEmitter");
			mod0->emitterObject().assign(root.get());

			auto ctlr0 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr0.get());
			ctlr0->modifierName().set(mod0->name().get());

			auto iplr0 = file.create<nif::NiFloatInterpolator>();
			Assert::IsNotNull(iplr0.get());
			ctlr0->interpolator().assign(iplr0.get());

			auto viplr0 = file.create<nif::NiBoolInterpolator>();
			Assert::IsNotNull(viplr0.get());
			ctlr0->visIplr().assign(viplr0.get());
			psys->modifiers().insert(-1, *mod0);
			psys->controllers().insert(-1, *ctlr0);

			//incomplete emitters
			auto mod1 = file.create<nif::NiPSysCylinderEmitter>();
			Assert::IsNotNull(mod1.get());
			mod1->name().set("CylEmitter");

			auto ctlr1 = file.create<nif::NiPSysEmitterCtlr>();
			Assert::IsNotNull(ctlr1.get());
			ctlr1->modifierName().set(mod1->name().get());
			psys->modifiers().insert(-1, *mod1);
			psys->controllers().insert(-1, *ctlr1);

			auto mod2 = file.create<nif::NiPSysSphereEmitter>();
			Assert::IsNotNull(mod2.get());
			psys->modifiers().insert(-1, *mod2);

			Constructor c(file);
			c.makeRoot();

			Assert::IsTrue(c.size() == 5);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), *root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), *psys);
			BoxEmitter* mod0_node = findNode<BoxEmitter>(c.nodes(), *mod0);
			CylinderEmitter* mod1_node = findNode<CylinderEmitter>(c.nodes(), *mod1);
			SphereEmitter* mod2_node = findNode<SphereEmitter>(c.nodes(), *mod2);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);
			Assert::IsNotNull(mod0_node);
			Assert::IsNotNull(mod1_node);
			Assert::IsNotNull(mod2_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(psys_node->getField(ParticleSystem::MODIFIERS)->connector, mod0_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod0_node->getField(Modifier::NEXT_MODIFIER)->connector, mod1_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod1_node->getField(Modifier::NEXT_MODIFIER)->connector, mod2_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod0_node->getField(VolumeEmitter::EMITTER_OBJECT)->connector, root_node->getField(Node::OBJECT)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 6);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod0->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod1->getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[3] == &mod2->getNative());

			Assert::IsTrue(mod0->getNative().GetEmitterObject() == &root->getNative());

			Assert::IsTrue(psys_node->object().getNative().GetControllers().size() == 4);
			auto&& ctlrs = psys_node->object().getNative().GetControllers();

			auto it = ctlrs.begin();
			Niflib::NiPSysEmitterCtlr* c0 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c0);
			Assert::IsTrue(c0 == &ctlr0->getNative());
			Assert::IsTrue(c0->GetModifierName() == mod0->name().get());
			Assert::IsTrue(c0->GetInterpolator() == &iplr0->getNative());
			Assert::IsTrue(c0->GetVisibilityInterpolator() == &viplr0->getNative());

			++it;
			Niflib::NiPSysEmitterCtlr* c1 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c1);
			Assert::IsTrue(c1 == &ctlr1->getNative());
			Assert::IsTrue(c1->GetModifierName() == mod1->name().get());
			Assert::IsTrue(c1->GetInterpolator() != nullptr);
			Assert::IsTrue(c1->GetVisibilityInterpolator() != nullptr);

			++it;
			Niflib::NiPSysEmitterCtlr* c2 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c2);
			Assert::IsTrue(c2->GetModifierName() == mod2->name().get());
			Assert::IsTrue(c2->GetInterpolator() != nullptr);
			Assert::IsTrue(c2->GetVisibilityInterpolator() != nullptr);
		}

		TEST_METHOD(Load_BirthRateController)
		{
			//We want a separate controller node only if:
			//A) There is a NiFloatData attached
			// OR
			//B) We have a NiBlendFloatInterpolator
			//
			//Unless I am mistaken, the other settings don't matter if the value is const (should verify!).
			// We might want to warn about or repair crazy settings, though (0 freq, non-increasing time interval...).
			// 
			//Do we care about the keys in the FloatData? 
			// We *could* drop it if it's constant, but probably not worth the trouble.
			//
			//In case A the node should be a FloatController.
			//In case B the node should be some "multi-sequence controller" that we don't have yet. 

			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.getRoot();
			auto iplrA = file.create<nif::NiFloatInterpolator>();
			auto emitterA = file.create<nif::NiPSysBoxEmitter>();
			auto iplrB = file.create<nif::NiBlendFloatInterpolator>();
			auto emitterB = file.create<nif::NiPSysBoxEmitter>();
			auto iplr0 = file.create<nif::NiFloatInterpolator>();
			auto emitter0 = file.create<nif::NiPSysBoxEmitter>();

			Assert::IsNotNull(root.get());
			Assert::IsNotNull(iplrA.get());
			Assert::IsNotNull(emitterA.get());
			Assert::IsNotNull(iplrB.get());
			Assert::IsNotNull(emitterB.get());
			Assert::IsNotNull(iplr0.get());
			Assert::IsNotNull(emitter0.get());
			
			{//case A
				ParticleSystem psys_node(file);
				root->children().add(psys_node.object());

				emitterA->name().set("A");
				psys_node.object().modifiers().insert(0, *emitterA);
				auto ctlr = file.create<nif::NiPSysEmitterCtlr>();
				ctlr->modifierName().set("A");
				psys_node.object().controllers().insert(0, *ctlr);
				ctlr->interpolator().assign(iplrA.get());

				auto data = file.create<nif::NiFloatData>();
				iplrA->data().assign(data.get());

				//change up the controller settings
				ctlr->flags().set(75);
				ctlr->frequency().set(1.4f);
				ctlr->phase().set(0.6f);
				ctlr->startTime().set(0.1f);
				ctlr->stopTime().set(2.8f);
			}

			{//case B (placeholder)
				ParticleSystem psys_node(file);
				root->children().add(psys_node.object());

				emitterB->name().set("B");
				psys_node.object().modifiers().insert(0, *emitterB);
				auto ctlr = file.create<nif::NiPSysEmitterCtlr>();
				ctlr->modifierName().set("B");
				psys_node.object().controllers().insert(0, *ctlr);
				ctlr->interpolator().assign(iplrB.get());

				//ignore controller settings
			}

			{//case 0 (no controller node)
				ParticleSystem psys_node(file);
				root->children().add(psys_node.object());

				emitter0->name().set("0");
				psys_node.object().modifiers().insert(0, *emitter0);
				auto ctlr = file.create<nif::NiPSysEmitterCtlr>();
				ctlr->modifierName().set("0");
				psys_node.object().controllers().insert(0, *ctlr);
				ctlr->interpolator().assign(iplr0.get());

				//change up the controller settings
				ctlr->flags().set(77);
				ctlr->frequency().set(7.4f);
				ctlr->phase().set(0.2f);
				ctlr->startTime().set(0.4f);
				ctlr->stopTime().set(3.6f);
			}

			Constructor c(file);
			c.makeRoot();

			//Were the correct nodes created?
			//1 Root, 3 ParticleSystem, 3 BoxEmitter, 1 FloatController
			Assert::IsTrue(c.size() == 8);

			Root* root_node = findNode<Root>(c.nodes(), *root);
			FloatController* cA_node = findNode<FloatController>(c.nodes(), *iplrA);
			NodeBase* cB_node = findNode<NodeBase>(c.nodes(), *iplrB);
			NodeBase* c0_node = findNode<NodeBase>(c.nodes(), *iplr0);
			Emitter* eA_node = findNode<Emitter>(c.nodes(), *emitterA);
			Emitter* eB_node = findNode<Emitter>(c.nodes(), *emitterB);
			Emitter* e0_node = findNode<Emitter>(c.nodes(), *emitter0);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(cA_node);
			Assert::IsNull(cB_node);
			Assert::IsNull(c0_node);
			Assert::IsNotNull(eA_node);
			Assert::IsNotNull(eB_node);
			Assert::IsNotNull(e0_node);

			//Test connections
			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);
			Assert::IsTrue(areConnected(cA_node->getField(FloatController::TARGET)->connector, eA_node->getField(Emitter::BIRTH_RATE)->connector));
			//Assert::IsTrue(areConnected(cB_node->getField(Controller::TARGET)->connector, eB_node->getField(Emitter::BIRTH_RATE)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(eA_node->controller().getNative().GetInterpolator() == &iplrA->getNative());
			Assert::IsTrue(eB_node->controller().getNative().GetInterpolator() == &iplrB->getNative());
			Assert::IsTrue(e0_node->controller().getNative().GetInterpolator() == &iplr0->getNative());

			//controllerA settings should have been transferred to the node
			Assert::IsTrue(eA_node->controller().flags().get() == cA_node->flags().get());
			Assert::IsTrue(eA_node->controller().frequency().get() == cA_node->frequency().get());
			Assert::IsTrue(eA_node->controller().phase().get() == cA_node->phase().get());
			Assert::IsTrue(eA_node->controller().startTime().get() == cA_node->startTime().get());
			Assert::IsTrue(eA_node->controller().stopTime().get() == cA_node->stopTime().get());

			//controllerB settings should be ignored (they use the ControllerSequence for that)

			//leave unspecified what should be done with the settings on controller0
		}*/
	};
	
	TEST_CLASS(VolumeEmitter)
	{
		//Test connection and assignment of an emitter object
		TEST_METHOD(EmitterObject)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiPSysBoxEmitter>();
			AssignableSenderTest(node::Default<node::BoxEmitter>{}.create(file, obj), obj->emitterObject, node::VolumeEmitter::EMITTER_OBJECT, false);
		}
	};
}
