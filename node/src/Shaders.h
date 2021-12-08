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
#include "NodeBase.h"

namespace node
{
	using namespace nif;

	class EffectShader : public NodeBase
	{
	public:
		EffectShader(const ni_ptr<BSEffectShaderProperty>& obj);
		~EffectShader();

	public:
		constexpr static const char* GEOMETRY = "Targets";
		constexpr static const char* SHADER_FLAGS_1 = "ShaderFlags1";
		constexpr static const char* SHADER_FLAGS_2 = "ShaderFlags2";
		constexpr static const char* EMISSIVE_COLOUR = "EmissiveColour";
		constexpr static const char* EMISSIVE_MULTIPLE = "EmissiveMultiple";
		constexpr static const char* SOURCE_TEXTURE = "SourceTexture";
		constexpr static const char* PALETTE_TEXTURE = "PaletteTexture";
		constexpr static const char* SUBTEXTURES = "Subtextures";

		constexpr static float WIDTH = 170.0f;
		constexpr static float HEIGHT = 250.0f;

	private:
		std::unique_ptr<Field> m_targetField;
		std::unique_ptr<Field> m_colField;
		std::unique_ptr<Field> m_multField;
		std::unique_ptr<Field> m_texField;
		std::unique_ptr<Field> m_paletteField;
	};
}