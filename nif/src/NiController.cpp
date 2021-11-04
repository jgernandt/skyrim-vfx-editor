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
#include "NiController.h"

nif::NiInterpolator::NiInterpolator(native::NiInterpolator* obj) : NiObject(obj) {}

nif::native::NiInterpolator& nif::NiInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiInterpolator::TYPE));
	return static_cast<native::NiInterpolator&>(*m_ptr);
}


nif::NiBoolData::NiBoolData() : NiBoolData(new Niflib::NiBoolData) {}
nif::NiBoolData::NiBoolData(native::NiBoolData* obj) : 
	NiObject(obj), 
	m_keyType(obj, &native::NiBoolData::GetKeyType, &native::NiBoolData::SetKeyType),
	m_keys(*this)
{}

nif::native::NiBoolData& nif::NiBoolData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBoolData::TYPE));
	return static_cast<native::NiBoolData&>(*m_ptr);
}

std::vector<nif::Key<bool>> nif::NiBoolData::Keys::get() const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	std::vector<Key<bool>> result;
	result.reserve(keys.size());
	for (auto&& key : keys) {
		result.push_back(Key<bool>{ 
			key.time, 
			static_cast<bool>(key.data), 
			static_cast<bool>(key.forward_tangent),
			static_cast<bool>(key.backward_tangent),
			key.tension, key.bias, key.continuity });
	}
	return result;
}

void nif::NiBoolData::Keys::set(const std::vector<Key<bool>>& keys)
{
	auto&& dest = m_super.getNative().GetKeysRef();
	dest.clear();
	dest.reserve(keys.size());
	for (auto&& key : keys) {
		dest.push_back(Niflib::Key<unsigned char>{
			key.time,
			key.value,
			key.forward,
			key.backward,
			key.tension, key.bias, key.continuity });
	}
}


nif::NiBoolInterpolator::NiBoolInterpolator() : NiBoolInterpolator(new Niflib::NiBoolInterpolator) {}

nif::NiBoolInterpolator::NiBoolInterpolator(native::NiBoolInterpolator* obj) :
	NiInterpolator(obj),
	m_value(&getNative(), &native::NiBoolInterpolator::GetBoolValue, &native::NiBoolInterpolator::SetBoolValue),
	m_data(&getNative(), &native::NiBoolInterpolator::GetData, &native::NiBoolInterpolator::SetData)
{
}

nif::native::NiBoolInterpolator& nif::NiBoolInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBoolInterpolator::TYPE));
	return static_cast<native::NiBoolInterpolator&>(*m_ptr);
}


nif::NiFloatData::NiFloatData() : NiFloatData(new Niflib::NiFloatData) {}
nif::NiFloatData::NiFloatData(native::NiFloatData* obj) : 
	NiObject(obj),
	m_keyType(obj, &native::NiFloatData::GetKeyType, &native::NiFloatData::SetKeyType),
	m_keys(*this)
{}

nif::native::NiFloatData& nif::NiFloatData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiFloatData::TYPE));
	return static_cast<native::NiFloatData&>(*m_ptr);
}

std::vector<nif::Key<float>> nif::NiFloatData::Keys::get() const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	std::vector<Key<float>> result;
	result.reserve(keys.size());
	for (auto&& key : keys) {
		result.push_back(Key<float>{
			key.time,
			key.data,
			key.forward_tangent,
			key.backward_tangent,
			key.tension, key.bias, key.continuity });
	}
	return result;
}

void nif::NiFloatData::Keys::set(const std::vector<Key<float>>& keys)
{
	auto&& dest = m_super.getNative().GetKeysRef();
	dest.clear();
	dest.reserve(keys.size());
	for (auto&& key : keys) {
		dest.push_back(Niflib::Key<float>{
			key.time,
			key.value,
			key.forward,
			key.backward,
			key.tension, key.bias, key.continuity });
	}
}


nif::NiFloatInterpolator::NiFloatInterpolator() : NiFloatInterpolator(new Niflib::NiFloatInterpolator) {}

nif::NiFloatInterpolator::NiFloatInterpolator(native::NiFloatInterpolator* obj) : 
	NiInterpolator(obj),
	m_value(&getNative(), &native::NiFloatInterpolator::GetFloatValue, &native::NiFloatInterpolator::SetFloatValue),
	m_data(&getNative(), &native::NiFloatInterpolator::GetData, &native::NiFloatInterpolator::SetData)
{
}

nif::native::NiFloatInterpolator& nif::NiFloatInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiFloatInterpolator::TYPE));
	return static_cast<native::NiFloatInterpolator&>(*m_ptr);
}


nif::NiBlendBoolInterpolator::NiBlendBoolInterpolator() : NiBlendBoolInterpolator(new Niflib::NiBlendBoolInterpolator) {}
nif::NiBlendBoolInterpolator::NiBlendBoolInterpolator(native::NiBlendBoolInterpolator* obj) : NiInterpolator(obj) {}

nif::native::NiBlendBoolInterpolator& nif::NiBlendBoolInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBlendBoolInterpolator::TYPE));
	return static_cast<native::NiBlendBoolInterpolator&>(*m_ptr);
}


nif::NiBlendFloatInterpolator::NiBlendFloatInterpolator() : NiBlendFloatInterpolator(new Niflib::NiBlendFloatInterpolator) {}
nif::NiBlendFloatInterpolator::NiBlendFloatInterpolator(native::NiBlendFloatInterpolator* obj) : NiInterpolator(obj) {}

nif::native::NiBlendFloatInterpolator& nif::NiBlendFloatInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBlendFloatInterpolator::TYPE));
	return static_cast<native::NiBlendFloatInterpolator&>(*m_ptr);
}


nif::NiTimeController::NiTimeController(native::NiTimeController* obj) :
	NiObject(obj),
	m_flags(&getNative(), &native::NiTimeController::GetFlags, &native::NiTimeController::SetFlags),
	m_frequency(&getNative(), &native::NiTimeController::GetFrequency, &native::NiTimeController::SetFrequency),
	m_phase(&getNative(), &native::NiTimeController::GetPhase, &native::NiTimeController::SetPhase),
	m_startTime(&getNative(), &native::NiTimeController::GetStartTime, &native::NiTimeController::SetStartTime),
	m_stopTime(&getNative(), &native::NiTimeController::GetStopTime, &native::NiTimeController::SetStopTime)
{}

nif::native::NiTimeController& nif::NiTimeController::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiTimeController::TYPE));
	return static_cast<native::NiTimeController&>(*m_ptr);
}


nif::NiSingleInterpController::NiSingleInterpController(native::NiSingleInterpController* obj) :
	NiTimeController(obj),
	m_iplr(&getNative(), &native::NiSingleInterpController::GetInterpolator, &native::NiSingleInterpController::SetInterpolator)
{
}

nif::native::NiSingleInterpController& nif::NiSingleInterpController::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiSingleInterpController::TYPE));
	return static_cast<native::NiSingleInterpController&>(*m_ptr);
}
