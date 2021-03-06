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
#include "NiObject.h"

namespace nif
{
	//I don't think none/both are valid, but we might run into it
	enum class AlphaMode : std::uint_fast16_t
	{
		NONE = 0,
		BLEND = 1,
		TEST = 0x200,
		BOTH = 0X201,
	};

	enum class BlendFunction : std::uint_fast16_t
	{
		ONE,
		ZERO,
		SRC_COLOUR,
		ONE_MINUS_SRC_COLOUR,
		DST_COLOUR,
		ONE_MINUS_DST_COLOUR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		SRC_ALPHA_SATURATE,
	};

	enum class TestFunction : std::uint_fast16_t
	{
		ALWAYS,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NEQUAL,
		GEQUAL,
		NEVER,
	};

	struct NiProperty : NiTraversable<NiProperty, NiObjectNET> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiAlphaProperty : NiTraversable<NiAlphaProperty, NiProperty>
	{
		Property<AlphaMode> mode;
		Property<BlendFunction> srcFcn;
		Property<BlendFunction> dstFcn;
		Property<TestFunction> testFcn;
		Property<bool> noSorting;
		Property<std::uint_fast8_t> threshold;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct BSShaderProperty : NiTraversable<BSShaderProperty, NiProperty>
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct BSEffectShaderProperty : NiTraversable<BSEffectShaderProperty, BSShaderProperty>
	{
		Property<ColRGBA> emissiveCol;
		Property<float> emissiveMult;
		Property<std::string> sourceTex;
		Property<std::string> greyscaleTex;
		FlagSet<ShaderFlags> shaderFlags1;
		FlagSet<ShaderFlags> shaderFlags2;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}
