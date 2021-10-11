#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(BSEffectShaderPropertyTests)
	{
	public:
		nif::BSEffectShaderProperty obj;
		std::mt19937 m_engine;

		TEST_METHOD(EmissiveColour)
		{
			PropertyTest<nif::ColRGBA>(obj.emissiveCol(), m_engine);
		}

		TEST_METHOD(EmissiveMult)
		{
			PropertyTest<float>(obj.emissiveMult(), m_engine);
		}

		TEST_METHOD(SourceTexture)
		{
			StringPropertyTest(obj.sourceTex());
		}

		TEST_METHOD(GreyscaleTexture)
		{
			StringPropertyTest(obj.greyscaleTex());
		}

		TEST_METHOD(ShaderFlag1)
		{
			std::vector<nif::ShaderFlag1> flags{
				ShaderFlag1::PALETTE_ALPHA,
				ShaderFlag1::PALETTE_COLOUR,
			};
			FlagTest<nif::ShaderFlag1>(obj.shaderFlags1(), flags);
		}

		TEST_METHOD(ShaderFlag2)
		{
			std::vector<nif::ShaderFlag2> flags{
				ShaderFlag2::DOUBLE_SIDED,
				ShaderFlag2::VERTEX_COLOUR,
			};
			FlagTest<nif::ShaderFlag2>(obj.shaderFlags2(), flags);
		}
	};

	TEST_CLASS(NiAlphaPropertyTests)
	{
	public:
		nif::NiAlphaProperty obj;
		std::mt19937 m_engine;

		TEST_METHOD(Flags)
		{
			PropertyTest<unsigned short>(obj.flags(), m_engine);
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
			auto node = std::make_unique<EffectShader>();
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
	};
}
