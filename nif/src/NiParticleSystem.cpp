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

#include "pch.h"
#include "NiParticleSystem.h"
#include "NiPSysModifier.h"
#include "File.h"

static nif::File::CreateFcn g_NiPSysDataFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysData::TYPE), & nif::make_NiObject<nif::NiPSysData>);

static nif::File::CreateFcn g_NiParticleSystemFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiParticleSystem::TYPE), & nif::make_NiObject<nif::NiParticleSystem>);


void nif::NiSyncer<nif::NiPSysData>::syncReadImpl(
	File& file, NiPSysData* object, Niflib::NiPSysData* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysData>::syncWriteImpl(
	File& file, NiPSysData* object, Niflib::NiPSysData* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiParticleSystem>::syncReadImpl(
	File& file, NiParticleSystem* object, Niflib::NiParticleSystem* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiParticleSystem>::syncWriteImpl(
	File& file, NiParticleSystem* object, Niflib::NiParticleSystem* native) const
{
	//TODO
}

/*nif::NiPSysData::NiPSysData() : NiPSysData(new Niflib::NiPSysData) {}
nif::NiPSysData::NiPSysData(native_type* obj) :
	NiObject(obj), 
	m_maxCount(*this, &getNative(), &native::NiPSysData::GetBSMaxVertices, &native::NiPSysData::SetBSMaxVertices),
	m_subtexOffsets(*this),
	m_hasColour(*this, &getNative(), &native::NiPSysData::GetHasVertexColors, &native::NiPSysData::SetHasVertexColors),
	m_hasRotationAngles(*this, &getNative(), &native::NiPSysData::GetHasRotationAngles, &native::NiPSysData::SetHasRotationAngles),
	m_hasRotationSpeeds(*this, &getNative(), &native::NiPSysData::GetHasRotationSpeeds, &native::NiPSysData::SetHasRotationSpeeds)
{}

nif::native::NiPSysData& nif::NiPSysData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysData::TYPE));
	return static_cast<native::NiPSysData&>(*m_ptr);
}

nif::NiParticleSystem::NiParticleSystem() : NiParticleSystem(new Niflib::NiParticleSystem)
{
	static int count = 0;
	getNative().SetName("ParticleSystem" + std::to_string(++count));
	getNative().SetFlags(14U);
	getNative().GetVertexDescriptor().bitfield = 0x840200004000051;//BS use this for psys'
	//Corresponding to this:
	//getNative().GetVertexDescriptor().SetVertexDataSize(1);
	//getNative().GetVertexDescriptor().SetDynamicVertexSize(5);
	//getNative().GetVertexDescriptor().SetColorOffset(4);
	//getNative().GetVertexDescriptor().SetVertexAttributes(Niflib::VF_UVS | Niflib::VF_FULL_PRECISION);
	//getNative().GetVertexDescriptor().SetUnknown02(8);//unclear if this does anything
}

nif::NiParticleSystem::NiParticleSystem(native_type* obj) :
	NiAVObject(obj), 
	m_data(*this),
	m_modifiers(*this), 
	m_shader(*this), 
	m_alpha(*this), 
	m_worldSpace(*this, &getNative(), &native::NiParticleSystem::GetWorldSpace, &native::NiParticleSystem::SetWorldSpace)
{
}

nif::native::NiParticleSystem& nif::NiParticleSystem::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiParticleSystem::TYPE));
	return static_cast<native::NiParticleSystem&>(*m_ptr);
}

void nif::NiParticleSystem::Data::assign(NiPSysData* data)
{
	nativePtr()->SetData(data ? &data->getNative() : nullptr);
	notify(data);
}

bool nif::NiParticleSystem::Data::isAssigned(NiPSysData* shader) const
{
	return nativePtr()->GetData() == (shader ? &shader->getNative() : nullptr);
}

size_t nif::NiParticleSystem::Modifiers::insert(size_t pos, const NiPSysModifier& mod)
{
	size_t result;
	if (size_t current = find(mod); current == -1) {
		assert(mod.target().isAssigned(nullptr));//we cannot take someone else's modifier

		auto&& mods = nativePtr()->GetModifiers();
		result = std::min(pos, mods.size());

		if (result == pos)
			mods.insert(mods.begin() + pos, &mod.getNative());
		else
			mods.push_back(&mod.getNative());

		//A bit dodgy to set target on a const ref. Maybe this should be done at a higher level?
		mod.getNative().SetTarget(nativePtr());

		notifyInsert(result);
	}
	else
		result = current;

	return result;
}

size_t nif::NiParticleSystem::Modifiers::erase(size_t pos)
{
	//size_t pos = std::numeric_limits<size_t>::max();

	//auto&& mods = m_super.getNative().GetModifiers();
	//for (size_t i = 0; i < mods.size(); i++)
	//	if (mods[i] == &mod.getNative()) {
	//		mods.erase(mods.begin() + i);
	//		pos = i;
	//	}

	//return pos;

	auto&& mods = nativePtr()->GetModifiers();
	assert(pos < mods.size());

	mods[pos]->SetTarget(nullptr);
	mods.erase(mods.begin() + pos);

	size_t result = pos < mods.size() ? pos : -1;
	notifyErase(result);

	return result;
}

size_t nif::NiParticleSystem::Modifiers::find(const NiPSysModifier& mod) const
{
	size_t result = std::numeric_limits<size_t>::max();

	auto&& mods = nativePtr()->GetModifiers();
	for (size_t i = 0; i < mods.size(); i++)
		if (mods[i] == &mod.getNative()) {
			result = i;
			break;
		}

	return result;
}

size_t nif::NiParticleSystem::Modifiers::size() const
{
	return nativePtr()->GetModifiers().size();
}

void nif::NiParticleSystem::ShaderProperty::assign(BSEffectShaderProperty* shader)
{
	nativePtr()->SetShaderProperty(shader ? &shader->getNative() : nullptr);
	notify(shader);
}

bool nif::NiParticleSystem::ShaderProperty::isAssigned(BSEffectShaderProperty* shader) const
{
	return nativePtr()->GetShaderProperty() == (shader ? &shader->getNative() : nullptr);
}

void nif::NiParticleSystem::AlphaProperty::assign(NiAlphaProperty* alpha)
{
	nativePtr()->SetAlphaProperty(alpha ? &alpha->getNative() : nullptr);
	notify(alpha);
}

bool nif::NiParticleSystem::AlphaProperty::isAssigned(NiAlphaProperty* alpha) const
{
	return nativePtr()->GetAlphaProperty() == (alpha ? &alpha->getNative() : nullptr);
}

std::vector<nif::SubtextureOffset> nif::NiPSysData::SubtexOffsets::get() const
{
	std::vector<SubtextureOffset> result;
	for (auto&& offs : nativePtr()->GetSubtextureOffsets())
		result.push_back(nif_type_conversion<SubtextureOffset>::from(offs));
	return result;
}

void nif::NiPSysData::SubtexOffsets::set(const std::vector<SubtextureOffset>& offsets)
{
	nativePtr()->SetHasSubtextures(!offsets.empty());

	std::vector<Niflib::Vector4>& target = nativePtr()->GetSubtextureOffsets();
	target.resize(offsets.size());
	for (size_t i = 0; i < target.size(); i++)
		target[i] = nif_type_conversion<Niflib::Vector4>::from(offsets[i]);

	notify(offsets);
}*/
