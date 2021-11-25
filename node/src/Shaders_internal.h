#pragma once
#include "Shaders.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	template<>
	class Default<EffectShader>
	{
	public:
		std::unique_ptr<EffectShader> create(File& file)
		{
			auto obj = file.create<BSEffectShaderProperty>();
			if (!obj)
				throw std::runtime_error("Failed to create BSEffectShaderProperty");

			obj->shaderFlags1.raise(SF1_ZBUFFER_TEST);
			obj->shaderFlags2.raise(SF2_VERTEX_COLOUR);
			obj->emissiveCol.set(COL_WHITE);
			obj->emissiveMult.set(1.0f);

			return std::make_unique<EffectShader>(std::move(obj));
		}
	};
}
