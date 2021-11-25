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
#include "nif_internal.h"

const size_t nif::NiParticleSystem::TYPE = std::hash<std::string>{}("NiParticleSystem");
const size_t nif::NiPSysData::TYPE = std::hash<std::string>{}("NiPSysData");


bool nif::Forwarder<nif::NiParticleSystem>::operator()(NiParticleSystem& object, NiTraverser& traverser)
{
	if (auto&& obj = object.data.assigned())
		obj->receive(traverser);

	for (auto&& obj : object.modifiers) {
		assert(obj);
		obj->receive(traverser);
	}

	if (auto&& obj = object.shaderProperty.assigned())
		obj->receive(traverser);

	if (auto&& obj = object.alphaProperty.assigned())
		obj->receive(traverser);

	return true;
}

bool nif::ReadSyncer<nif::NiParticleSystem>::operator()(NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file)
{
	assert(native);
	object.data.assign(file.get<NiPSysData>(native->GetData()));

	object.modifiers.clear();
	for (auto&& mod : native->GetModifiers())
		object.modifiers.insert(object.modifiers.size(), file.get<NiPSysModifier>(mod));

	object.shaderProperty.assign(file.get<BSShaderProperty>(native->GetShaderProperty()));
	object.alphaProperty.assign(file.get<NiAlphaProperty>(native->GetAlphaProperty()));
	object.worldSpace.set(native->GetWorldSpace());

	return true;
}

bool nif::WriteSyncer<nif::NiParticleSystem>::operator()(const NiParticleSystem& object, Niflib::NiParticleSystem* native, const File& file)
{
	assert(native);
	native->SetData(file.getNative<NiPSysData>(object.data.assigned().get()));

	native->ClearModifiers();
	for (auto&& mod : object.modifiers)
		native->AddModifier(file.getNative<NiPSysModifier>(mod.get()));

	native->SetShaderProperty(file.getNative<BSShaderProperty>(object.shaderProperty.assigned().get()));
	native->SetAlphaProperty(file.getNative<NiAlphaProperty>(object.alphaProperty.assigned().get()));
	native->SetWorldSpace(object.worldSpace.get());

	//Ideally, this would be done at construction of the native. We never change it.
	native->GetVertexDescriptor().bitfield = 0x840200004000051;//BS use this for psys'
	//Corresponding to this:
	//getNative().GetVertexDescriptor().SetVertexDataSize(1);
	//getNative().GetVertexDescriptor().SetDynamicVertexSize(5);
	//getNative().GetVertexDescriptor().SetColorOffset(4);
	//getNative().GetVertexDescriptor().SetVertexAttributes(Niflib::VF_UVS | Niflib::VF_FULL_PRECISION);
	//getNative().GetVertexDescriptor().SetUnknown02(8);//unclear if this does anything

	return true;
}

bool nif::ReadSyncer<nif::NiPSysData>::operator()(NiPSysData& object, const Niflib::NiPSysData* native, File& file)
{
	assert(native);
	object.maxCount.set(native->GetBSMaxVertices());

	std::vector<SubtextureOffset> offsets;
	offsets.reserve(native->GetSubtextureOffsets().size());
	for (auto&& offset : native->GetSubtextureOffsets())
		offsets.push_back(nif_type_conversion<SubtextureOffset>::from(offset));
	object.subtexOffsets.set(std::move(offsets));

	object.hasColour.set(native->GetHasVertexColors());
	object.hasRotationAngles.set(native->GetHasRotationAngles());
	object.hasRotationSpeeds.set(native->GetHasRotationSpeeds());

	return true;
}

bool nif::WriteSyncer<nif::NiPSysData>::operator()(const NiPSysData& object, Niflib::NiPSysData* native, const File& file)
{
	assert(native);
	native->SetBSMaxVertices(object.maxCount.get());

	auto&& offsets1 = object.subtexOffsets.get();//Copy. Should we add a const ref getter to Property?
	auto&& offsets2 = native->GetSubtextureOffsets();
	offsets2.resize(offsets1.size());
	for (size_t i = 0; i < offsets1.size(); i++)
		offsets2[i] = nif_type_conversion<Niflib::Vector4>::from(offsets1[i]);

	native->SetHasVertexColors(object.hasColour.get());
	native->SetHasRotationAngles(object.hasRotationAngles.get());
	native->SetHasRotationSpeeds(object.hasRotationSpeeds.get());

	return true;
}
