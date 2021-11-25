#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes_internal.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(EffectShader)
	{
	public:

		TEST_METHOD(Connector_Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<BSEffectShaderProperty>();

			//Testing for BSShaderProperty now, should be BSEffectShaderProperty.
			//We'll have to fix inheritance for Assignable.
			AssignableReceiverTest(std::make_unique<node::EffectShader>(obj), 
				*std::static_pointer_cast<BSShaderProperty>(obj), node::EffectShader::GEOMETRY, true);
		}
	};
}
