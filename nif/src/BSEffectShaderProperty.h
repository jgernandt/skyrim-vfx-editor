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
		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiProperty> { using type = NiProperty; };
	template<> struct type_map<NiProperty> { using type = Niflib::NiProperty; };
	template<> class NiSyncer<NiProperty> : public NiSyncer<NiObjectNET> {};

	struct NiAlphaProperty : NiTraversable<NiAlphaProperty, NiProperty>
	{
		Property<AlphaMode> mode;
		Property<BlendFunction> srcFcn;
		Property<BlendFunction> dstFcn;
		Property<TestFunction> testFcn;
		Property<std::uint_fast8_t> threshold;
		Property<bool> sorting;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiAlphaProperty> { using type = NiAlphaProperty; };
	template<> struct type_map<NiAlphaProperty> { using type = Niflib::NiAlphaProperty; };

	template<> class NiSyncer<NiAlphaProperty> : public NiSyncer<NiProperty>
	{
	public:
		void syncRead(File& file, NiAlphaProperty* object, Niflib::NiAlphaProperty* native);
		void syncWrite(const File& file, NiAlphaProperty* object, Niflib::NiAlphaProperty* native);
	};

	struct BSEffectShaderProperty : NiTraversable<BSEffectShaderProperty, NiProperty>
	{
		Property<ColRGBA> emissiveCol;
		Property<float> emissiveMult;
		Property<std::string> sourceTex;
		Property<std::string> greyscaleTex;
		FlagSet<ShaderFlag1> shaderFlags1;
		FlagSet<ShaderFlag2> shaderFlags2;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::BSEffectShaderProperty> { using type = BSEffectShaderProperty; };
	template<> struct type_map<BSEffectShaderProperty> { using type = Niflib::BSEffectShaderProperty; };

	template<> class NiSyncer<BSEffectShaderProperty> : public NiSyncer<NiProperty>
	{
	public:
		void syncRead(File& file, BSEffectShaderProperty* object, Niflib::BSEffectShaderProperty* native);
		void syncWrite(const File& file, BSEffectShaderProperty* object, Niflib::BSEffectShaderProperty* native);
	};
}
