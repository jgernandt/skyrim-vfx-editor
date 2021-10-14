//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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

#include "pch.h"
#include "BSEffectShaderProperty.h"

nif::NiAlphaProperty::NiAlphaProperty() : NiAlphaProperty(new Niflib::NiAlphaProperty) {}
nif::NiAlphaProperty::NiAlphaProperty(native::NiAlphaProperty* obj) : 
	NiObjectNET(obj),
	m_flags(&getNative(), &native::NiAlphaProperty::GetFlags, &native::NiAlphaProperty::SetFlags)
{}

nif::native::NiAlphaProperty& nif::NiAlphaProperty::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiAlphaProperty::TYPE));
	return static_cast<native::NiAlphaProperty&>(*m_ptr);
}

nif::BSEffectShaderProperty::BSEffectShaderProperty() : BSEffectShaderProperty(new Niflib::BSEffectShaderProperty) {}

nif::BSEffectShaderProperty::BSEffectShaderProperty(native::BSEffectShaderProperty* obj) : 
	NiObjectNET(obj), 
	m_emissiveCol(&getNative(), &native::BSEffectShaderProperty::GetEmissiveColor, &native::BSEffectShaderProperty::SetEmissiveColor),
	m_emissiveMult(&getNative(), &native::BSEffectShaderProperty::GetEmissiveMultiple, &native::BSEffectShaderProperty::SetEmissiveMultiple),
	m_sourceTex(&getNative(), &native::BSEffectShaderProperty::GetSourceTexture, &native::BSEffectShaderProperty::SetSourceTexture),
	m_greyscaleTex(&getNative(), &native::BSEffectShaderProperty::GetGreyscaleTexture, &native::BSEffectShaderProperty::SetGreyscaleTexture),
	m_shaderFlags1(*this), 
	m_shaderFlags2(*this)
{}

nif::native::BSEffectShaderProperty& nif::BSEffectShaderProperty::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSEffectShaderProperty::TYPE));
	return static_cast<native::BSEffectShaderProperty&>(*m_ptr);
}

/*nif::ColRGBA nif::BSEffectShaderProperty::EmissiveColProp::get() const
{
	return nif_type_conversion<nif::ColRGBA>::from(m_super.getNative().GetEmissiveColor());
}

void nif::BSEffectShaderProperty::EmissiveColProp::set(const ColRGBA& col)
{
	m_super.getNative().SetEmissiveColor(nif_type_conversion<Niflib::Color4>::from(col));
}*/

void nif::BSEffectShaderProperty::ShaderFlag1Set::set(ShaderFlag1 flag, bool on)
{
	m_super.getNative().SetShaderFlag1(static_cast<Niflib::SkyrimShaderPropertyFlags1>(flag), on);
}

bool nif::BSEffectShaderProperty::ShaderFlag1Set::isSet(ShaderFlag1 flag) const
{
	return m_super.getNative().GetShaderFlag1(static_cast<Niflib::SkyrimShaderPropertyFlags1>(flag));
}

void nif::BSEffectShaderProperty::ShaderFlag2Set::set(ShaderFlag2 flag, bool on)
{
	m_super.getNative().SetShaderFlag2(static_cast<Niflib::SkyrimShaderPropertyFlags2>(flag), on);
}

bool nif::BSEffectShaderProperty::ShaderFlag2Set::isSet(ShaderFlag2 flag) const
{
	return m_super.getNative().GetShaderFlag2(static_cast<Niflib::SkyrimShaderPropertyFlags2>(flag));
}
