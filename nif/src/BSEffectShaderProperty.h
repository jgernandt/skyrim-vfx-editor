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
#include "NiObjectNET.h"

namespace nif
{
	enum class ShaderFlag1 : unsigned int
	{
		PALETTE_COLOUR	= 0x00000010,
		PALETTE_ALPHA	= 0x00000020,
		ZBUFFER_TEST	= 0x80000000,
	};

	enum class ShaderFlag2 : unsigned int
	{
		DOUBLE_SIDED	= 0x00000010,
		VERTEX_COLOUR	= 0x00000020,
	};

	class NiAlphaProperty : public NiObjectNET
	{
	public:
		NiAlphaProperty();
		NiAlphaProperty(native::NiAlphaProperty* obj);
		NiAlphaProperty(const NiAlphaProperty&) = delete;

		virtual ~NiAlphaProperty() = default;

		NiAlphaProperty& operator=(const NiAlphaProperty&) = delete;

		native::NiAlphaProperty& getNative() const;

		IProperty<unsigned short>& flags() { return m_flags; }

	private:
		Property<unsigned short> m_flags;//should be a FlagSet<unsigned short>, but I can't be bothered right now
	};

	class BSEffectShaderProperty : public NiObjectNET
	{
	public:
		BSEffectShaderProperty();
		BSEffectShaderProperty(native::BSEffectShaderProperty* obj);
		BSEffectShaderProperty(const BSEffectShaderProperty&) = delete;

		virtual ~BSEffectShaderProperty() = default;

		BSEffectShaderProperty& operator=(const BSEffectShaderProperty&) = delete;

		native::BSEffectShaderProperty& getNative() const;

		IProperty<ColRGBA>& emissiveCol() { return m_emissiveCol; }
		IProperty<float>& emissiveMult() { return m_emissiveMult; }
		IProperty<std::string>& sourceTex() { return m_sourceTex; }
		IProperty<std::string>& greyscaleTex() { return m_greyscaleTex; }
		FlagSet<ShaderFlag1>& shaderFlags1() { return m_shaderFlags1; }
		FlagSet<ShaderFlag2>& shaderFlags2() { return m_shaderFlags2; }

	private:
		Property<ColRGBA, native::ColRGBA> m_emissiveCol;
		Property<float> m_emissiveMult;
		Property<std::string> m_sourceTex;
		Property<std::string> m_greyscaleTex;

		struct ShaderFlag1Set : FlagSet<ShaderFlag1>
		{
			ShaderFlag1Set(BSEffectShaderProperty& super) : m_super{ super } {}

			virtual void set(ShaderFlag1 flag, bool on) override;
			virtual bool isSet(ShaderFlag1 flag) const override;

			BSEffectShaderProperty& m_super;

		} m_shaderFlags1;

		struct ShaderFlag2Set : FlagSet<ShaderFlag2>
		{
			ShaderFlag2Set(BSEffectShaderProperty& super) : m_super{ super } {}

			virtual void set(ShaderFlag2 flag, bool on) override;
			virtual bool isSet(ShaderFlag2 flag) const override;

			BSEffectShaderProperty& m_super;

		} m_shaderFlags2;
	};
}