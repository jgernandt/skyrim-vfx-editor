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

const size_t nif::NiBoolData::TYPE = std::hash<std::string>{}("NiBoolData");
const size_t nif::NiFloatData::TYPE = std::hash<std::string>{}("NiFloatData");
const size_t nif::NiInterpolator::TYPE = std::hash<std::string>{}("NiInterpolator");
const size_t nif::NiBoolInterpolator::TYPE = std::hash<std::string>{}("NiBoolInterpolator");
const size_t nif::NiFloatInterpolator::TYPE = std::hash<std::string>{}("NiFloatInterpolator");
const size_t nif::NiBlendInterpolator::TYPE = std::hash<std::string>{}("NiBlendInterpolator");
const size_t nif::NiBlendBoolInterpolator::TYPE = std::hash<std::string>{}("NiBlendBoolInterpolator");
const size_t nif::NiBlendFloatInterpolator::TYPE = std::hash<std::string>{}("NiBlendFloatInterpolator");
const size_t nif::NiTimeController::TYPE = std::hash<std::string>{}("NiTimeController");
const size_t nif::NiSingleInterpController::TYPE = std::hash<std::string>{}("NiSingleInterpController");


void nif::ReadSyncer<nif::NiBoolData>::operator()(NiBoolData& object, const Niflib::NiBoolData* native, File& file)
{
	assert(native);

	object.keyType.set(nif_type_conversion<KeyType>::from(native->GetKeyType()));

	object.keys.clear();
	for (auto&& key : native->GetKeys()) {
		object.keys.push_back();
		object.keys.back().time.set(key.time);
		object.keys.back().value.set(static_cast<bool>(key.data));
		object.keys.back().fwdTan.set(static_cast<bool>(key.forward_tangent));
		object.keys.back().bwdTan.set(static_cast<bool>(key.backward_tangent));
		object.keys.back().tension.set(key.tension);
		object.keys.back().bias.set(key.bias);
		object.keys.back().continuity.set(key.continuity);
	}
}

void nif::WriteSyncer<nif::NiBoolData>::operator()(const NiBoolData& object, Niflib::NiBoolData* native, const File& file)
{
	assert(native);

	native->SetKeyType(nif_type_conversion<Niflib::KeyType>::from(object.keyType.get()));

	auto&& keys = native->GetKeysRef();
	keys.clear();
	for (auto&& key : object.keys) {
		keys.push_back({ key.time.get(), key.value.get(), key.fwdTan.get(),
			key.bwdTan.get(), key.tension.get(), key.bias.get(), key.continuity.get() });
	}
}


void nif::ReadSyncer<nif::NiFloatData>::operator()(NiFloatData& object, const Niflib::NiFloatData* native, File& file)
{
	assert(native);

	object.keyType.set(nif_type_conversion<KeyType>::from(native->GetKeyType()));

	object.keys.clear();
	for (auto&& key : native->GetKeys()) {
		object.keys.push_back();
		object.keys.back().time.set(key.time);
		object.keys.back().value.set(key.data);
		object.keys.back().fwdTan.set(key.forward_tangent);
		object.keys.back().bwdTan.set(key.backward_tangent);
		object.keys.back().tension.set(key.tension);
		object.keys.back().bias.set(key.bias);
		object.keys.back().continuity.set(key.continuity);
	}
}

void nif::WriteSyncer<nif::NiFloatData>::operator()(const NiFloatData& object, Niflib::NiFloatData* native, const File& file)
{
	assert(native);

	native->SetKeyType(nif_type_conversion<Niflib::KeyType>::from(object.keyType.get()));

	auto&& keys = native->GetKeysRef();
	keys.clear();
	for (auto&& key : object.keys) {
		keys.push_back({ key.time.get(), key.value.get(), key.fwdTan.get(),
			key.bwdTan.get(), key.tension.get(), key.bias.get(), key.continuity.get() });
	}
}


void nif::Forwarder<nif::NiBoolInterpolator>::operator()(NiBoolInterpolator& object, NiTraverser& traverser)
{
	if (auto&& data = object.data.assigned())
		data->receive(traverser);
}

void nif::ReadSyncer<nif::NiBoolInterpolator>::operator()(NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file)
{
	assert(native);
	object.value.set(native->GetBoolValue());
	object.data.assign(file.get<NiBoolData>(native->GetData()));
}

void nif::WriteSyncer<nif::NiBoolInterpolator>::operator()(const NiBoolInterpolator& object, Niflib::NiBoolInterpolator* native, const File& file)
{
	assert(native);
	native->SetBoolValue(object.value.get());
	native->SetData(file.getNative<NiBoolData>(object.data.assigned().get()));
}


void nif::Forwarder<nif::NiFloatInterpolator>::operator()(NiFloatInterpolator& object, NiTraverser& traverser)
{
	if (auto&& data = object.data.assigned())
		data->receive(traverser);
}

void nif::ReadSyncer<nif::NiFloatInterpolator>::operator()(NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file)
{
	assert(native);
	object.value.set(native->GetFloatValue());
	object.data.assign(file.get<NiFloatData>(native->GetData()));
}

void nif::WriteSyncer<nif::NiFloatInterpolator>::operator()(const NiFloatInterpolator& object, Niflib::NiFloatInterpolator* native, const File& file)
{
	assert(native);
	native->SetFloatValue(object.value.get());
	native->SetData(file.getNative<NiFloatData>(object.data.assigned().get()));
}


void nif::ReadSyncer<nif::NiTimeController>::operator()(NiTimeController& object, const Niflib::NiTimeController* native, File& file)
{
	assert(native);
	object.flags.clear();
	object.flags.raise(native->GetFlags());
	object.frequency.set(native->GetFrequency());
	object.phase.set(native->GetPhase());
	object.startTime.set(native->GetStartTime());
	object.stopTime.set(native->GetStopTime());
}

void nif::WriteSyncer<nif::NiTimeController>::operator()(const NiTimeController& object, Niflib::NiTimeController* native, const File& file)
{
	assert(native);
	native->SetFlags(static_cast<unsigned short>(object.flags.raised()));
	native->SetFrequency(object.frequency.get());
	native->SetPhase(object.phase.get());
	native->SetStartTime(object.startTime.get());
	native->SetStopTime(object.stopTime.get());
}


void nif::Forwarder<nif::NiSingleInterpController>::operator()(NiSingleInterpController& object, NiTraverser& traverser)
{
	if (auto&& iplr = object.interpolator.assigned())
		iplr->receive(traverser);
}

void nif::ReadSyncer<nif::NiSingleInterpController>::operator()(NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file)
{
	assert(native);
	object.interpolator.assign(file.get<NiInterpolator>(native->GetInterpolator()));
}

void nif::WriteSyncer<nif::NiSingleInterpController>::operator()(const NiSingleInterpController& object, Niflib::NiSingleInterpController* native, const File& file)
{
	assert(native);
	native->SetInterpolator(file.getNative<NiInterpolator>(object.interpolator.assigned().get()));
}


void nif::ReadSyncer<nif::NiPSysModifierCtlr>::operator()(NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file)
{
	assert(native);
	object.modifierName.set(native->GetModifierName());
}

void nif::WriteSyncer<nif::NiPSysModifierCtlr>::operator()(const NiPSysModifierCtlr& object, Niflib::NiPSysModifierCtlr* native, const File& file)
{
	assert(native);
	native->SetModifierName(object.modifierName.get());
}


void nif::Forwarder<nif::NiPSysEmitterCtlr>::operator()(NiPSysEmitterCtlr& object, NiTraverser& traverser)
{
	if (auto&& obj = object.visIplr.assigned())
		obj->receive(traverser);
}

void nif::ReadSyncer<nif::NiPSysEmitterCtlr>::operator()(NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file)
{
	assert(native);
	object.visIplr.assign(file.get<NiInterpolator>(native->GetVisibilityInterpolator()));
}

void nif::WriteSyncer<nif::NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& object, Niflib::NiPSysEmitterCtlr* native, const File& file)
{
	assert(native);
	native->SetVisibilityInterpolator(file.getNative<NiInterpolator>(object.visIplr.assigned().get()));
}
