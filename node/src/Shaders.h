//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "Controls.h"
#include "BSEffectShaderProperty.h"

namespace node
{
	class EffectShader : public NodeBase
	{
	public:
		EffectShader();
		EffectShader(std::unique_ptr<nif::BSEffectShaderProperty>&& obj);

	public:
		virtual nif::BSEffectShaderProperty& object() override;
		//IProperty<nif::SubtextureCount>& subtexCount();

	public:
		constexpr static const char* GEOMETRY = "Targets";
		constexpr static const char* SHADER_FLAGS_1 = "ShaderFlags1";
		constexpr static const char* SHADER_FLAGS_2 = "ShaderFlags2";
		constexpr static const char* EMISSIVE_COLOUR = "EmissiveColour";
		constexpr static const char* EMISSIVE_MULTIPLE = "EmissiveMultiple";
		constexpr static const char* SOURCE_TEXTURE = "SourceTexture";
		constexpr static const char* PALETTE_TEXTURE = "PaletteTexture";
		constexpr static const char* SUBTEXTURES = "Subtextures";

	private:
		class GeometryField;
		class ShaderFlagsField1;
		class ShaderFlagsField2;
		class SourceTexField;
		class SubtexturesField;
		class EmissiveColourField;
		class EmissiveMultipleField;
		class PaletteTexField;

		//GeometryField* m_geomField{ nullptr };
	};
}