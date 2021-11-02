#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"
#include "Constructor.h"

#include "nif_backend.h"


namespace node
{

	TEST_CLASS(ConstructorTests)
	{
		//Test importing a hierarchy of NiNodes
		TEST_METHOD(Nodes)
		{
			nif::BSFadeNode root;
			nif::NiNode c1;
			root.children().add(c1);
			nif::NiNode c2;
			root.children().add(c2);
			nif::NiNode c1c1;
			c1.children().add(c1c1);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 4);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			Node* c1_node = findNode<Node>(c.nodes(), c1);
			Node* c2_node = findNode<Node>(c.nodes(), c2);
			Node* c1c1_node = findNode<Node>(c.nodes(), c1c1);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(c1_node);
			Assert::IsNotNull(c2_node);
			Assert::IsNotNull(c1c1_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, c1_node->getField(Node::PARENT)->connector));
			Assert::IsTrue(areConnected(root_node->getField(Node::CHILDREN)->connector, c2_node->getField(Node::PARENT)->connector));
			Assert::IsTrue(areConnected(c1_node->getField(Node::CHILDREN)->connector, c1c1_node->getField(Node::PARENT)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(root.getNative().GetChildren().size() == 2);
			Assert::IsTrue(c1.getNative().GetChildren().size() == 1);
			Assert::IsTrue(c2.getNative().GetChildren().size() == 0);
			Assert::IsTrue(c1c1.getNative().GetChildren().size() == 0);
			Assert::IsTrue(c1.getNative().GetParent() == &root.getNative());
			Assert::IsTrue(c2.getNative().GetParent() == &root.getNative());
			Assert::IsTrue(c1c1.getNative().GetParent() == &c1.getNative());
		}

		//Test StringData nodes (partially dependent on Nodes test)
		TEST_METHOD(StringExtraData)
		{
			nif::BSFadeNode root;
			nif::NiStringExtraData s1;
			root.extraData().add(s1);

			nif::NiNode n1;
			root.children().add(n1);
			nif::NiStringExtraData s2;
			n1.extraData().add(s2);

			//Multitarget extra data should be allowed (but not necessarily meaningful)
			nif::NiStringExtraData s3;
			root.extraData().add(s3);
			n1.extraData().add(s3);

			//Special types
			nif::NiStringExtraData weaponTypes[7];
			std::string types[7]{
				"WeaponAxe",
				"WeaponDagger",
				"WeaponMace",
				"WeaponSword",
				"WeaponBack",
				"WeaponBow",
				"SHIELD" };
			for (int i = 0; i < 7; i++) {
				weaponTypes[i].name().set("Prn");
				weaponTypes[i].value().set(types[i]);
				root.extraData().add(weaponTypes[i]);
			}

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 12);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			Node* n1_node = findNode<Node>(c.nodes(), n1);
			StringData* s1_node = findNode<StringData>(c.nodes(), s1);
			StringData* s2_node = findNode<StringData>(c.nodes(), s2);
			StringData* s3_node = findNode<StringData>(c.nodes(), s3);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(n1_node);
			Assert::IsNotNull(s1_node);
			Assert::IsNotNull(s2_node);
			Assert::IsNotNull(s3_node);

			WeaponTypeData* wtype_nodes[7];
			for (int i = 0; i < 7; i++) {
				wtype_nodes[i] = findNode<WeaponTypeData>(c.nodes(), weaponTypes[i]);
				Assert::IsNotNull(wtype_nodes[i]);
			}

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, s1_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(n1_node->getField(Node::EXTRA_DATA)->connector, s2_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, s3_node->getField(ExtraData::TARGET)->connector));
			Assert::IsTrue(areConnected(n1_node->getField(Node::EXTRA_DATA)->connector, s3_node->getField(ExtraData::TARGET)->connector));

			for (int i = 0; i < 7; i++)
				Assert::IsTrue(areConnected(root_node->getField(Node::EXTRA_DATA)->connector, wtype_nodes[i]->getField(ExtraData::TARGET)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(root.getNative().GetExtraData().size() == 9);
			Assert::IsTrue(n1.getNative().GetExtraData().size() == 2);
		}

		TEST_METHOD(ParticleSystem_regular)
		{
			nif::BSFadeNode root;

			nif::NiParticleSystem psys;
			root.children().add(psys);
			nif::NiPSysData data;
			psys.data().assign(&data);
			//seems weird to include the alpha property but not the shader property, but you'll almost never touch the alpha
			nif::NiAlphaProperty alpha;
			psys.alphaProperty().assign(&alpha);
			nif::NiPSysUpdateCtlr ctlr;
			psys.controllers().insert(0, ctlr);
			nif::NiPSysAgeDeathModifier mod0;
			psys.modifiers().insert(-1, mod0);
			nif::NiPSysPositionModifier mod1;
			psys.modifiers().insert(-1, mod1);
			nif::NiPSysBoundUpdateModifier mod2;
			psys.modifiers().insert(-1, mod2);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), psys);
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
			Assert::IsTrue(psys_node->object().getNative().GetData() == &data.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetBSProperty(1) == &alpha.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetControllers().size() == 1);
			Assert::IsTrue(psys_node->object().getNative().GetControllers().front() == &ctlr.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 3);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[0] == &mod0.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod1.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod2.getNative());

			//Was a warning generated?
			Assert::IsFalse(!c.warnings().empty());
		}

		//If critical components are missing, they should be added
		TEST_METHOD(ParticleSystem_incomplete)
		{
			nif::BSFadeNode root;
			nif::NiParticleSystem psys;
			root.children().add(psys);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), psys);
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
		TEST_METHOD(ParticleSystem_irregular)
		{
			nif::BSFadeNode root;

			nif::NiParticleSystem psys;
			root.children().add(psys);
			nif::NiPSysData data;
			psys.data().assign(&data);
			nif::NiAlphaProperty alpha;
			psys.alphaProperty().assign(&alpha);
			nif::NiPSysUpdateCtlr ctlr;
			psys.controllers().insert(0, ctlr);
			//Add the fundamental modifiers in an order we don't expect
			nif::NiPSysAgeDeathModifier mod1;
			psys.modifiers().insert(0, mod1);
			nif::NiPSysPositionModifier mod2;
			psys.modifiers().insert(0, mod2);
			nif::NiPSysBoundUpdateModifier mod3;
			psys.modifiers().insert(0, mod3);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 2);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), psys);
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
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[0] == &mod1.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod2.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod3.getNative());

			//Was a warning generated? We expect not, but maybe we should?
			Assert::IsFalse(!c.warnings().empty());
		}

		TEST_METHOD(Emitters)
		{
			nif::BSFadeNode root;
			nif::NiParticleSystem psys;
			root.children().add(psys);

			//Complete emitter
			nif::NiPSysBoxEmitter mod0;
			mod0.name().set("BoxEmitter");
			mod0.emitterObject().assign(&root);
			nif::NiPSysEmitterCtlr ctlr0;
			ctlr0.modifierName().set(mod0.name().get());
			nif::NiFloatInterpolator iplr0;
			ctlr0.interpolator().assign(&iplr0);
			nif::NiBoolInterpolator viplr0;
			ctlr0.visIplr().assign(&viplr0);
			psys.modifiers().insert(-1, mod0);
			psys.controllers().insert(-1, ctlr0);

			//incomplete emitters
			nif::NiPSysCylinderEmitter mod1;
			mod1.name().set("CylEmitter");
			nif::NiPSysEmitterCtlr ctlr1;
			ctlr1.modifierName().set(mod1.name().get());
			psys.modifiers().insert(-1, mod1);
			psys.controllers().insert(-1, ctlr1);

			nif::NiPSysSphereEmitter mod2;
			psys.modifiers().insert(-1, mod2);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 5);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), psys);
			BoxEmitter* mod0_node = findNode<BoxEmitter>(c.nodes(), mod0);
			CylinderEmitter* mod1_node = findNode<CylinderEmitter>(c.nodes(), mod1);
			SphereEmitter* mod2_node = findNode<SphereEmitter>(c.nodes(), mod2);
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
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod0.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod1.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[3] == &mod2.getNative());

			Assert::IsTrue(mod0.getNative().GetEmitterObject() == &root.getNative());

			Assert::IsTrue(psys_node->object().getNative().GetControllers().size() == 4);
			auto&& ctlrs = psys_node->object().getNative().GetControllers();

			auto it = ctlrs.begin();
			Niflib::NiPSysEmitterCtlr* c0 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c0);
			Assert::IsTrue(c0 == &ctlr0.getNative());
			Assert::IsTrue(c0->GetModifierName() == mod0.name().get());
			Assert::IsTrue(c0->GetInterpolator() == &iplr0.getNative());
			Assert::IsTrue(c0->GetVisibilityInterpolator() == &viplr0.getNative());

			++it;
			Niflib::NiPSysEmitterCtlr* c1 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c1);
			Assert::IsTrue(c1 == &ctlr1.getNative());
			Assert::IsTrue(c1->GetModifierName() == mod1.name().get());
			Assert::IsTrue(c1->GetInterpolator() != nullptr);
			Assert::IsTrue(c1->GetVisibilityInterpolator() != nullptr);

			++it;
			Niflib::NiPSysEmitterCtlr* c2 = Niflib::DynamicCast<Niflib::NiPSysEmitterCtlr>(*it);
			Assert::IsNotNull(c2);
			Assert::IsTrue(c2->GetModifierName() == mod2.name().get());
			Assert::IsTrue(c2->GetInterpolator() != nullptr);
			Assert::IsTrue(c2->GetVisibilityInterpolator() != nullptr);
		}

		TEST_METHOD(Modifiers)
		{
			nif::BSFadeNode root;
			nif::NiParticleSystem psys;
			root.children().add(psys);

			nif::NiPSysRotationModifier mod0;
			psys.modifiers().insert(-1, mod0);

			nif::NiPSysGravityModifier mod1;
			mod1.forceType().set(nif::FORCE_PLANAR);
			mod1.gravityObject().assign(&root);
			psys.modifiers().insert(-1, mod1);

			nif::NiPSysGravityModifier mod2;
			mod2.forceType().set(nif::FORCE_SPHERICAL);
			psys.modifiers().insert(-1, mod2);

			nif::BSPSysSimpleColorModifier mod3;
			psys.modifiers().insert(-1, mod3);

			nif::BSPSysScaleModifier mod4;
			psys.modifiers().insert(-1, mod4);

			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 7);

			//Were the correct nodes created?
			Root* root_node = findNode<Root>(c.nodes(), root);
			ParticleSystem* psys_node = findNode<ParticleSystem>(c.nodes(), psys);
			RotationModifier* mod0_node = findNode<RotationModifier>(c.nodes(), mod0);
			PlanarForceField* mod1_node = findNode<PlanarForceField>(c.nodes(), mod1);
			SphericalForceField* mod2_node = findNode<SphericalForceField>(c.nodes(), mod2);
			SimpleColourModifier* mod3_node = findNode<SimpleColourModifier>(c.nodes(), mod3);
			ScaleModifier* mod4_node = findNode<ScaleModifier>(c.nodes(), mod4);
			Assert::IsNotNull(root_node);
			Assert::IsNotNull(psys_node);
			Assert::IsNotNull(mod0_node);
			Assert::IsNotNull(mod1_node);
			Assert::IsNotNull(mod2_node);
			Assert::IsNotNull(mod3_node);
			Assert::IsNotNull(mod4_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(psys_node->getField(ParticleSystem::MODIFIERS)->connector, mod0_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod0_node->getField(Modifier::NEXT_MODIFIER)->connector, mod1_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod1_node->getField(Modifier::NEXT_MODIFIER)->connector, mod2_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod2_node->getField(Modifier::NEXT_MODIFIER)->connector, mod3_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod3_node->getField(Modifier::NEXT_MODIFIER)->connector, mod4_node->getField(Modifier::TARGET)->connector));
			Assert::IsTrue(areConnected(mod1_node->getField(GravityModifier::GRAVITY_OBJECT)->connector, root_node->getField(Node::OBJECT)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(psys_node->object().getNative().GetModifiers().size() == 8);
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[1] == &mod0.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[2] == &mod1.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[3] == &mod2.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[4] == &mod3.getNative());
			Assert::IsTrue(psys_node->object().getNative().GetModifiers()[5] == &mod4.getNative());

			Assert::IsTrue(mod1.getNative().GetGravityObject() == &root.getNative());
		}

		TEST_METHOD(ShaderProperties)
		{
			nif::BSFadeNode root;

			//Particle system with subtexture offsets
			nif::NiParticleSystem psys0;
			root.children().add(psys0);

			/*nif::NiPSysData data0;
			nif::SubtextureCount subtexCount0 = { 3, 2 };
			data0.subtexOffsets().set(nif::nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(subtexCount0));
			psys0.data().assign(&data0);*/

			nif::BSEffectShaderProperty shader0;
			psys0.shaderProperty().assign(&shader0);

			//Two particle systems sharing a shader property block
			nif::NiParticleSystem psys1;
			root.children().add(psys1);
			nif::NiParticleSystem psys2;
			root.children().add(psys2);

			nif::BSEffectShaderProperty shader12;
			psys1.shaderProperty().assign(&shader12);
			psys2.shaderProperty().assign(&shader12);

			//Particle system with irregular subtexture offsets
			/*nif::NiParticleSystem psys3;
			root.children().add(psys3);

			nif::NiPSysData data3;
			std::vector<nif::SubtextureOffset> offsets3{ { 0.2f, 0.3f, 0.0f, 0.5f } };
			data3.subtexOffsets().set(offsets3);
			psys3.data().assign(&data3);

			nif::BSEffectShaderProperty shader3;
			psys3.shaderProperty().assign(&shader3);*/


			Constructor c;
			c.makeRoot(&root.getNative());

			Assert::IsTrue(c.size() == 6);

			//Were the correct nodes created?
			ParticleSystem* psys0_node = findNode<ParticleSystem>(c.nodes(), psys0);
			ParticleSystem* psys1_node = findNode<ParticleSystem>(c.nodes(), psys1);
			ParticleSystem* psys2_node = findNode<ParticleSystem>(c.nodes(), psys2);
			//ParticleSystem* psys3_node = findNode<ParticleSystem>(c.nodes(), psys3);
			EffectShader* shader0_node = findNode<EffectShader>(c.nodes(), shader0);
			EffectShader* shader12_node = findNode<EffectShader>(c.nodes(), shader12);
			//EffectShader* shader3_node = findNode<EffectShader>(c.nodes(), shader3);
			Assert::IsNotNull(psys0_node);
			Assert::IsNotNull(psys1_node);
			Assert::IsNotNull(psys2_node);
			//Assert::IsNotNull(psys3_node);
			Assert::IsNotNull(shader0_node);
			Assert::IsNotNull(shader12_node);
			//Assert::IsNotNull(shader3_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(psys0_node->getField(ParticleSystem::SHADER)->connector, shader0_node->getField(EffectShader::GEOMETRY)->connector));
			Assert::IsTrue(areConnected(psys1_node->getField(ParticleSystem::SHADER)->connector, shader12_node->getField(EffectShader::GEOMETRY)->connector));
			Assert::IsTrue(areConnected(psys2_node->getField(ParticleSystem::SHADER)->connector, shader12_node->getField(EffectShader::GEOMETRY)->connector));
			//Assert::IsTrue(areConnected(psys3_node->getField(ParticleSystem::SHADER)->connector, shader3_node->getField(EffectShader::GEOMETRY)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(psys0.getNative().GetBSProperty(0) == &shader0.getNative());
			Assert::IsTrue(psys1.getNative().GetBSProperty(0) == &shader12.getNative());
			Assert::IsTrue(psys2.getNative().GetBSProperty(0) == &shader12.getNative());
			//Assert::IsTrue(psys3.getNative().GetBSProperty(0) == &shader3.getNative());

			/*Assert::IsTrue(psys0.data().isAssigned(&data0));
			Assert::IsTrue(nif::nif_type_conversion<nif::SubtextureCount>::from(data0.subtexOffsets().get()) == subtexCount0);
			Assert::IsTrue(shader0_node->subtexCount().get() == subtexCount0);

			Assert::IsTrue(psys3.data().isAssigned(&data3));
			Assert::IsTrue(data3.subtexOffsets().get() == offsets3);
			Assert::IsTrue(shader3_node->subtexCount().get() == nif::SubtextureCount{ 0, 0 });*/
		}
	};
}
