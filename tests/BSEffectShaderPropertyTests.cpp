#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

#include "Constructor.h"
#include "nif_backend.h"

namespace nif
{
	TEST_CLASS(BSEffectShaderPropertyTests)
	{
	public:
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(EmissiveColour)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());
			PropertyTest<nif::ColRGBA>(obj->emissiveCol(), m_engine);
		}

		TEST_METHOD(EmissiveMult)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());
			PropertyTest<float>(obj->emissiveMult(), m_engine);
		}

		TEST_METHOD(SourceTexture)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());
			StringPropertyTest(obj->sourceTex());
		}

		TEST_METHOD(GreyscaleTexture)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());
			StringPropertyTest(obj->greyscaleTex());
		}

		TEST_METHOD(ShaderFlag1)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());

			std::vector<nif::ShaderFlag1> flags{
				ShaderFlag1::PALETTE_ALPHA,
				ShaderFlag1::PALETTE_COLOUR,
			};

			FlagTest<nif::ShaderFlag1>(obj->shaderFlags1(), flags);
		}

		TEST_METHOD(ShaderFlag2)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			Assert::IsNotNull(obj.get());

			std::vector<nif::ShaderFlag2> flags{
				ShaderFlag2::DOUBLE_SIDED,
				ShaderFlag2::VERTEX_COLOUR,
			};

			FlagTest<nif::ShaderFlag2>(obj->shaderFlags2(), flags);
		}
	};

	TEST_CLASS(NiAlphaPropertyTests)
	{
	public:
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(Flags)
		{
			auto obj = file.create<NiAlphaProperty>();
			Assert::IsNotNull(obj.get());
			PropertyTest<unsigned short>(obj->flags(), m_engine);
		}
	};
}


namespace node
{
	TEST_CLASS(BSEffectShaderPropertyTests)
	{
	public:
		//Geometry should receive Assignable<BSEffectShaderProperty> and send IObservable<SubtextureCount>, multi
		//We're moving the subtexture count to ParticleSystem, so should now only receive Assignable<BSEffectShaderProperty>
		TEST_METHOD(Geometry)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto node = std::make_unique<EffectShader>(file);
			AssignableReceiverTest(EffectShader::GEOMETRY, true, std::move(node), &node->object());

			/*MockAssignable<nif::BSEffectShaderProperty> target1;
			MockAssignable<nif::BSEffectShaderProperty> target2;
			ConnectorTester tester(std::make_unique<EffectShader>());
			auto ifc = tester.tryConnect<IProperty<nif::SubtextureCount>, IAssignable<nif::BSEffectShaderProperty>>(
				EffectShader::GEOMETRY, true, &target1);
			tester.tryConnect<IProperty<nif::SubtextureCount>, IAssignable<nif::BSEffectShaderProperty>>(
				EffectShader::GEOMETRY, true, &target2);

			Assert::IsNotNull(ifc);

			//the node should have assigned its object to the target
			Assert::IsTrue(target1.isAssigned(&tester.getNode()->object()));

			//If we add a listener to the interface, it should receive calls when the subtex count on the node is changed.
			//It should also receive a call on connection.
			nif::SubtextureCount cnt{ 2, 3 };
			tester.getNode()->subtexCount().set(cnt);
			MockPropertyListener<nif::SubtextureCount> lsnr;
			ifc->addListener(lsnr);
			Assert::IsTrue(lsnr.isSignalled() && lsnr.result() == cnt);
			lsnr.reset();

			cnt = { 3, 5 };
			tester.getNode()->subtexCount().set(cnt);
			Assert::IsTrue(lsnr.isSignalled() && lsnr.result() == cnt);
			//Make sure it can be removed too
			lsnr.reset();
			ifc->removeListener(lsnr);
			tester.getNode()->subtexCount().set(cnt);
			Assert::IsFalse(lsnr.isSignalled());

			tester.disconnect<IAssignable<nif::BSEffectShaderProperty>>(&target1);

			//the object should have been unassigned
			Assert::IsTrue(target1.isAssigned(nullptr));*/
		}

		TEST_METHOD(Load)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			auto root = file.create<nif::BSFadeNode>();
			Assert::IsNotNull(root.get());

			auto psys0 = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys0.get());
			root->children().add(*psys0);

			auto shader0 = file.create<nif::BSEffectShaderProperty>();
			Assert::IsNotNull(shader0.get());
			psys0->shaderProperty().assign(shader0.get());

			//Two particle systems sharing a shader property block
			auto psys1 = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys1.get());
			root->children().add(*psys1);

			auto psys2 = file.create<nif::NiParticleSystem>();
			Assert::IsNotNull(psys2.get());
			root->children().add(*psys2);

			auto shader12 = file.create<nif::BSEffectShaderProperty>();
			Assert::IsNotNull(shader12.get());
			psys1->shaderProperty().assign(shader12.get());
			psys2->shaderProperty().assign(shader12.get());

			Constructor c(file);
			c.makeRoot(&root->getNative());

			Assert::IsTrue(c.size() == 6);

			//Were the correct nodes created?
			ParticleSystem* psys0_node = findNode<ParticleSystem>(c.nodes(), *psys0);
			ParticleSystem* psys1_node = findNode<ParticleSystem>(c.nodes(), *psys1);
			ParticleSystem* psys2_node = findNode<ParticleSystem>(c.nodes(), *psys2);
			EffectShader* shader0_node = findNode<EffectShader>(c.nodes(), *shader0);
			EffectShader* shader12_node = findNode<EffectShader>(c.nodes(), *shader12);
			Assert::IsNotNull(psys0_node);
			Assert::IsNotNull(psys1_node);
			Assert::IsNotNull(psys2_node);
			Assert::IsNotNull(shader0_node);
			Assert::IsNotNull(shader12_node);

			TestRoot nodeRoot;
			c.extractNodes(nodeRoot);

			//Were they connected?
			Assert::IsTrue(areConnected(psys0_node->getField(ParticleSystem::SHADER)->connector, shader0_node->getField(EffectShader::GEOMETRY)->connector));
			Assert::IsTrue(areConnected(psys1_node->getField(ParticleSystem::SHADER)->connector, shader12_node->getField(EffectShader::GEOMETRY)->connector));
			Assert::IsTrue(areConnected(psys2_node->getField(ParticleSystem::SHADER)->connector, shader12_node->getField(EffectShader::GEOMETRY)->connector));

			//Did we mess up the backend?
			Assert::IsTrue(psys0->getNative().GetBSProperty(0) == &shader0->getNative());
			Assert::IsTrue(psys1->getNative().GetBSProperty(0) == &shader12->getNative());
			Assert::IsTrue(psys2->getNative().GetBSProperty(0) == &shader12->getNative());
		}

	};
}
