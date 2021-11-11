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

nif::NiInterpolator::NiInterpolator(native_type* obj) : NiObject(obj) {}

nif::native::NiInterpolator& nif::NiInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiInterpolator::TYPE));
	return static_cast<native::NiInterpolator&>(*m_ptr);
}


nif::NiBoolData::NiBoolData() : NiBoolData(new Niflib::NiBoolData) {}
nif::NiBoolData::NiBoolData(native_type* obj) :
	NiObject(obj), 
	m_keyType(obj, &native::NiBoolData::GetKeyType, &native::NiBoolData::SetKeyType)
{}

nif::native::NiBoolData& nif::NiBoolData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBoolData::TYPE));
	return static_cast<native::NiBoolData&>(*m_ptr);
}


nif::NiBoolInterpolator::NiBoolInterpolator() : NiBoolInterpolator(new Niflib::NiBoolInterpolator) {}

nif::NiBoolInterpolator::NiBoolInterpolator(native_type* obj) :
	NiInterpolator(obj),
	m_value(&getNative(), &native::NiBoolInterpolator::GetBoolValue, &native::NiBoolInterpolator::SetBoolValue),
	m_data(&getNative(), &native::NiBoolInterpolator::GetData, &native::NiBoolInterpolator::SetData)
{}

nif::native::NiBoolInterpolator& nif::NiBoolInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBoolInterpolator::TYPE));
	return static_cast<native::NiBoolInterpolator&>(*m_ptr);
}


nif::NiFloatData::NiFloatData() : NiFloatData(new Niflib::NiFloatData) {}
nif::NiFloatData::NiFloatData(native_type* obj) :
	NiObject(obj),
	m_keyType(obj, &native::NiFloatData::GetKeyType, &native::NiFloatData::SetKeyType),
	m_keys(*this)
{}

nif::native::NiFloatData& nif::NiFloatData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiFloatData::TYPE));
	return static_cast<native::NiFloatData&>(*m_ptr);
}

std::vector<nif::Key<float>> nif::NiFloatData::IplnData::Keys::get() const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	std::vector<Key<float>> result;
	result.reserve(keys.size());
	for (auto&& key : keys)
		result.push_back({ key.time, key.data });

	return result;
}

void nif::NiFloatData::IplnData::Keys::set(const std::vector<Key<float>>& keys)
{
	auto&& dest = m_super.getNative().GetKeysRef();

	//first erase excess
	for (size_t i = dest.size(); i > keys.size(); i--)
		erase(i - 1);

	assert(dest.size() <= keys.size());//sanity check

	//then set existing
	size_t i = 0;
	for (; i < dest.size(); i++) {
		if (keys[i].key != dest[i].time || keys[i].value != dest[i].data) {
			dest[i].time = keys[i].key;
			dest[i].data = keys[i].value;
			notifySet(i, keys[i]);
		}
	}

	//then insert extra
	dest.reserve(keys.size());
	for (; i < keys.size(); i++)
		insert(i, keys[i]);
}

nif::Key<float> nif::NiFloatData::IplnData::Keys::get(int i) const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	return { keys[i].time, keys[i].data };
}

void nif::NiFloatData::IplnData::Keys::set(int i, const Key<float>& key)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	if (key.key != keys[i].time || key.value != keys[i].data) {
		keys[i].time = key.key;
		keys[i].data = key.value;
		notifySet(i, key);
	}
}

int nif::NiFloatData::IplnData::Keys::insert(int i, const Key<float>& key)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0);

	std::vector<Niflib::Key<float>>::iterator it;
	if (static_cast<size_t>(i) < keys.size())
		it = keys.begin() + i;
	else {
		it = keys.end();
		i = keys.size();
	}

	keys.insert(it, Niflib::Key<float>{key.key, key.value, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });
	notifyInsert(i);
	m_super.m_keys.m_tans.notifyInsert(i);
	m_super.m_keys.m_tbcs.notifyInsert(i);

	return i;
}

int nif::NiFloatData::IplnData::Keys::erase(int i)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	keys.erase(keys.begin() + i);
	notifyErase(i);
	m_super.m_keys.m_tans.notifyErase(i);
	m_super.m_keys.m_tbcs.notifyErase(i);

	return i;
}

IVectorProperty<nif::Key<float>>::element nif::NiFloatData::IplnData::Keys::at(int i)
{
	assert(i >= 0 && static_cast<size_t>(i) < m_super.getNative().GetKeysRef().size());
	return element(*this, i);
}

std::vector<nif::Tangent<float>> nif::NiFloatData::IplnData::Tangents::get() const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	std::vector<Tangent<float>> result;
	result.reserve(keys.size());
	for (auto&& key : keys)
		result.push_back({ key.forward_tangent, key.backward_tangent });

	return result;
}

void nif::NiFloatData::IplnData::Tangents::set(const std::vector<Tangent<float>>& keys)
{
	auto&& dest = m_super.getNative().GetKeysRef();

	//first erase excess
	for (size_t i = dest.size(); i > keys.size(); i--)
		erase(i - 1);

	assert(dest.size() <= keys.size());//sanity check

	//then set existing
	size_t i = 0;
	for (; i < dest.size(); i++) {
		if (keys[i].forward != dest[i].forward_tangent || keys[i].backward != dest[i].backward_tangent) {
			dest[i].forward_tangent = keys[i].forward;
			dest[i].backward_tangent = keys[i].backward;
			notifySet(i, keys[i]);
		}
	}

	//then insert extra
	dest.reserve(keys.size());
	for (; i < keys.size(); i++)
		insert(i, keys[i]);
}

nif::Tangent<float> nif::NiFloatData::IplnData::Tangents::get(int i) const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	return { keys[i].forward_tangent, keys[i].backward_tangent };
}

void nif::NiFloatData::IplnData::Tangents::set(int i, const Tangent<float>& tan)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	if (tan.forward != keys[i].forward_tangent || tan.backward != keys[i].backward_tangent) {
		keys[i].forward_tangent = tan.forward;
		keys[i].backward_tangent = tan.backward;
		notifySet(i, tan);
	}
}

int nif::NiFloatData::IplnData::Tangents::insert(int i, const Tangent<float>& tan)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0);

	std::vector<Niflib::Key<float>>::iterator it;
	if (static_cast<size_t>(i) < keys.size())
		it = keys.begin() + i;
	else {
		it = keys.end();
		i = keys.size();
	}

	keys.insert(it, Niflib::Key<float>{0.0f, 0.0f, tan.forward, tan.backward, 0.0f, 0.0f, 0.0f });
	notifyInsert(i);
	m_super.m_keys.m_keys.notifyInsert(i);
	m_super.m_keys.m_tbcs.notifyInsert(i);

	return i;
}

int nif::NiFloatData::IplnData::Tangents::erase(int i)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	keys.erase(keys.begin() + i);
	notifyErase(i);
	m_super.m_keys.m_keys.notifyErase(i);
	m_super.m_keys.m_tbcs.notifyErase(i);

	return i;
}

IVectorProperty<nif::Tangent<float>>::element nif::NiFloatData::IplnData::Tangents::at(int i)
{
	assert(i >= 0 && static_cast<size_t>(i) < m_super.getNative().GetKeysRef().size());
	return element(*this, i);
}

std::vector<nif::TBC> nif::NiFloatData::IplnData::TBCs::get() const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	std::vector<TBC> result;
	result.reserve(keys.size());
	for (auto&& key : keys)
		result.push_back({ key.tension, key.bias, key.continuity });

	return result;
}

void nif::NiFloatData::IplnData::TBCs::set(const std::vector<TBC>& keys)
{
	auto&& dest = m_super.getNative().GetKeysRef();

	//first erase excess
	for (size_t i = dest.size(); i > keys.size(); i--)
		erase(i - 1);

	assert(dest.size() <= keys.size());//sanity check

	//then set existing
	size_t i = 0;
	for (; i < dest.size(); i++) {
		if (keys[i].tension != dest[i].tension || 
			keys[i].bias != dest[i].bias || 
			keys[i].continuity != dest[i].continuity) 
		{
			dest[i].tension = keys[i].tension;
			dest[i].bias = keys[i].bias;
			dest[i].continuity = keys[i].continuity;
			notifySet(i, keys[i]);
		}
	}

	//then insert extra
	dest.reserve(keys.size());
	for (; i < keys.size(); i++)
		insert(i, keys[i]);
}

nif::TBC nif::NiFloatData::IplnData::TBCs::get(int i) const
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	return { keys[i].tension, keys[i].bias, keys[i].continuity };
}

void nif::NiFloatData::IplnData::TBCs::set(int i, const TBC& tbc)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	if (tbc.tension != keys[i].tension || tbc.bias != keys[i].bias || tbc.continuity != keys[i].continuity) {
		keys[i].tension = tbc.tension;
		keys[i].bias = tbc.bias;
		keys[i].continuity = tbc.continuity;
		notifySet(i, tbc);
	}
}

int nif::NiFloatData::IplnData::TBCs::insert(int i, const TBC& tbc)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0);

	std::vector<Niflib::Key<float>>::iterator it;
	if (static_cast<size_t>(i) < keys.size())
		it = keys.begin() + i;
	else {
		it = keys.end();
		i = keys.size();
	}

	keys.insert(it, Niflib::Key<float>{0.0f, 0.0f, 0.0f, 0.0f, tbc.tension, tbc.bias, tbc.continuity });
	notifyInsert(i);
	m_super.m_keys.m_keys.notifyInsert(i);
	m_super.m_keys.m_tans.notifyInsert(i);

	return i;
}

int nif::NiFloatData::IplnData::TBCs::erase(int i)
{
	auto&& keys = m_super.getNative().GetKeysRef();
	assert(i >= 0 && static_cast<size_t>(i) < keys.size());

	keys.erase(keys.begin() + i);
	notifyErase(i);
	m_super.m_keys.m_keys.notifyErase(i);
	m_super.m_keys.m_tans.notifyErase(i);

	return i;
}

IVectorProperty<nif::TBC>::element nif::NiFloatData::IplnData::TBCs::at(int i)
{
	assert(i >= 0 && static_cast<size_t>(i) < m_super.getNative().GetKeysRef().size());
	return element(*this, i);
}


nif::NiFloatInterpolator::NiFloatInterpolator() : NiFloatInterpolator(new Niflib::NiFloatInterpolator) {}

nif::NiFloatInterpolator::NiFloatInterpolator(native_type* obj) :
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
nif::NiBlendBoolInterpolator::NiBlendBoolInterpolator(native_type* obj) : NiInterpolator(obj) {}

nif::native::NiBlendBoolInterpolator& nif::NiBlendBoolInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBlendBoolInterpolator::TYPE));
	return static_cast<native::NiBlendBoolInterpolator&>(*m_ptr);
}


nif::NiBlendFloatInterpolator::NiBlendFloatInterpolator() : NiBlendFloatInterpolator(new Niflib::NiBlendFloatInterpolator) {}
nif::NiBlendFloatInterpolator::NiBlendFloatInterpolator(native_type* obj) : NiInterpolator(obj) {}

nif::native::NiBlendFloatInterpolator& nif::NiBlendFloatInterpolator::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiBlendFloatInterpolator::TYPE));
	return static_cast<native::NiBlendFloatInterpolator&>(*m_ptr);
}


nif::NiTimeController::NiTimeController(native_type* obj) :
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


nif::NiSingleInterpController::NiSingleInterpController(native_type* obj) :
	NiTimeController(obj),
	m_iplr(&getNative(), &native::NiSingleInterpController::GetInterpolator, &native::NiSingleInterpController::SetInterpolator)
{
}

nif::native::NiSingleInterpController& nif::NiSingleInterpController::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiSingleInterpController::TYPE));
	return static_cast<native::NiSingleInterpController&>(*m_ptr);
}
