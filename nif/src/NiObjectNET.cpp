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

#include "pch.h"
#include "NiObjectNET.h"
#include "NiController.h"

nif::NiObjectNET::NiObjectNET(native::NiObjectNET* obj) :
	NiObject(obj), 
	m_name(&getNative(), &native::NiObjectNET::GetName, &native::NiObjectNET::SetName),
	m_extraData(*this), m_controllers(*this)
{
}

nif::native::NiObjectNET& nif::NiObjectNET::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiObjectNET::TYPE));
	return static_cast<native::NiObjectNET&>(*m_ptr);
}

void nif::NiObjectNET::ExtraData::add(const NiExtraData& obj)
{
	if (!has(obj)) {
		m_super.getNative().AddExtraData(&obj.getNative());
		notifyAdd(obj);
	}
}

void nif::NiObjectNET::ExtraData::remove(const NiExtraData& obj)
{
	if (has(obj)) {
		m_super.getNative().RemoveExtraData(&obj.getNative());
		notifyRemove(obj);
	}
}

bool nif::NiObjectNET::ExtraData::has(const NiExtraData& obj) const
{
	for (auto&& data : m_super.getNative().GetExtraData())
		if (data == &obj.getNative())
			return true;

	return false;
}

size_t nif::NiObjectNET::ExtraData::size() const
{
	return m_super.getNative().GetExtraData().size();
}

size_t nif::NiObjectNET::Controllers::insert(size_t pos, const NiTimeController& obj)
{
	size_t result;
	if (size_t current = find(obj); current == -1) {
		assert(!obj.getNative().GetTarget());//should be cleared first

		//Inserting a chain wouldn't be too hard, unless at (or size) is 0. Then we'd have to get the controller list,
		//clear it, add our chain and reassign the old connectors to our target after us. Too much work for now.
		//Niflib::NiTimeController* last = &obj.getNative();
		//while (last->GetNextController())
		//	last = last->GetNextController();
		assert(!obj.getNative().GetNextController());//not necessarily an error, but let's treat it like one for now

		auto ctlrs = m_super.getNative().GetControllers();
		result = std::min(pos, ctlrs.size());

		if (result == 0)
			m_super.getNative().AddController(&obj.getNative());
		else {
			//We want to access the controller *before* our position, since it should point to us
			size_t pos_bef = std::min(pos - 1, ctlrs.size() - 1);
			auto it = ctlrs.begin();
			for (size_t i = 0; i < pos_bef; i++)
				++it;
			obj.getNative().SetNextController((*it)->GetNextController());
			(*it)->SetNextController(&obj.getNative());
		}

		obj.getNative().SetTarget(&m_super.getNative());

		notifyInsert(result);
	}
	else
		result = current;

	return result;
}

size_t nif::NiObjectNET::Controllers::erase(size_t pos)
{
	//size_t pos = find(obj);
	//if (pos != std::numeric_limits<size_t>::max())
	//	m_super.getNative().RemoveController(&obj.getNative());
	//return pos;

	auto ctlrs = m_super.getNative().GetControllers();
	assert(pos < ctlrs.size());

	if (pos == 0)
		m_super.getNative().RemoveController(ctlrs.front());
	else {
		auto it = ctlrs.begin();
		for (size_t i = 0; i < pos - 1; i++)
			++it;

		//it is the controller before the one to erase
		Niflib::NiTimeController* before = *it;
		//cut the next out of the chain
		before->SetNextController((*++it)->GetNextController());
		(*it)->SetNextController(nullptr);
		(*it)->SetTarget(nullptr);
	}

	size_t result = pos < ctlrs.size() - 1 ? pos : -1;
	notifyErase(result);

	return result;
}

size_t nif::NiObjectNET::Controllers::find(const NiTimeController& obj) const
{
	size_t result = std::numeric_limits<size_t>::max();
	auto ctlrs = m_super.getNative().GetControllers();
	auto it = ctlrs.begin();
	for (size_t i = 0; i < ctlrs.size(); i++) {
		if (*it == &obj.getNative()) {
			result = i;
			break;
		}
		++it;
	}
	return result;
}

size_t nif::NiObjectNET::Controllers::size() const
{
	return m_super.getNative().GetControllers().size();
}
