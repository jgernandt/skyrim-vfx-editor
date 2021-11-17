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
#include "NiObjectNET.h"

namespace nif
{
	enum class ShaderFlag1 : std::uint_fast32_t
	{
		PALETTE_COLOUR	= 0x00000010,
		PALETTE_ALPHA	= 0x00000020,
		ZBUFFER_TEST	= 0x80000000,
	};

	enum class ShaderFlag2 : std::uint_fast32_t
	{
		DOUBLE_SIDED	= 0x00000010,
		VERTEX_COLOUR	= 0x00000020,
	};

	struct NiProperty : NiObjectNET {};
	template<> struct type_map<Niflib::NiProperty> { using type = NiProperty; };
	template<> struct type_map<NiProperty> { using type = Niflib::NiProperty; };

	struct NiAlphaProperty : NiProperty
	{
		Property<std::uint_fast16_t> flags;
		Property<std::uint_fast8_t> threshold;
	};
	template<> struct type_map<Niflib::NiAlphaProperty> { using type = NiAlphaProperty; };
	template<> struct type_map<NiAlphaProperty> { using type = Niflib::NiAlphaProperty; };

	struct BSEffectShaderProperty : NiProperty
	{
		Property<ColRGBA> emissiveCol;
		Property<float> emissiveMult;
		Property<std::string> sourceTex;
		Property<std::string> greyscaleTex;
		FlagSet<ShaderFlag1> shaderFlags1;
		FlagSet<ShaderFlag2> shaderFlags2;
	};
	template<> struct type_map<Niflib::BSEffectShaderProperty> { using type = BSEffectShaderProperty; };
	template<> struct type_map<BSEffectShaderProperty> { using type = Niflib::BSEffectShaderProperty; };
}
