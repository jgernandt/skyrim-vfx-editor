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
		std::unique_ptr<EffectShader> create(File& file,
			const ni_ptr<BSEffectShaderProperty>& obj = ni_ptr<BSEffectShaderProperty>())
		{
			if (obj)
				return std::make_unique<EffectShader>(obj);
			else {
				auto new_obj = file.create<BSEffectShaderProperty>();
				if (!new_obj)
					throw std::runtime_error("Failed to create BSEffectShaderProperty");

				new_obj->shaderFlags1.raise(SF1_ZBUFFER_TEST);
				new_obj->shaderFlags2.raise(SF2_VERTEX_COLOUR);
				new_obj->emissiveCol.set(COL_WHITE);
				new_obj->emissiveMult.set(1.0f);

				return std::make_unique<EffectShader>(new_obj);
			}
		}
	};

	//No Connector specialisation

	template<>
	class Factory<BSEffectShaderProperty> : public VerticalTraverser<BSEffectShaderProperty, Factory>
	{
	public:
		template<typename C>
		bool operator() (BSEffectShaderProperty& obj, C& ctor)
		{
			if (auto ptr = std::static_pointer_cast<BSEffectShaderProperty>(ctor.getObject()); ptr.get() == &obj) {
				ctor.addNode(&obj, Default<EffectShader>{}.create(ctor.getFile(), ptr));
			}
			return false;
		}
	};

	//No Forwarder specialisation
}
