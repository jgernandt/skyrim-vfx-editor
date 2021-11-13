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
#include "NiExtraData.h"

nif::NiExtraData::NiExtraData(native_type* obj) :
	NiObject(obj), 
	m_name(*this, &getNative(), &native::NiExtraData::GetName, &native::NiExtraData::SetName)
{
}

nif::native::NiExtraData& nif::NiExtraData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiExtraData::TYPE));
	return static_cast<native::NiExtraData&>(*m_ptr);
}


nif::NiStringExtraData::NiStringExtraData() : 
	NiStringExtraData(new Niflib::NiStringExtraData)
{
	assert(m_ptr && m_ptr->GetType().IsSameType(Niflib::NiStringExtraData::TYPE));
}

nif::NiStringExtraData::NiStringExtraData(native_type* obj) :
	NiExtraData(obj), 
	m_value(*this, &getNative(), &native::NiStringExtraData::GetData, &native::NiStringExtraData::SetData)
{
}

nif::native::NiStringExtraData& nif::NiStringExtraData::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiStringExtraData::TYPE));
	return static_cast<native::NiStringExtraData&>(*m_ptr);
}
