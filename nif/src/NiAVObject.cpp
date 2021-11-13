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
#include "NiAVObject.h"
#include "NiNode.h"
#include "File.h"

nif::NiAVObject::NiAVObject(native_type* obj) :
	NiObjectNET(obj), m_transform(*this)
{}

nif::native::NiAVObject& nif::NiAVObject::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiAVObject::TYPE));
	return static_cast<native::NiAVObject&>(*m_ptr);
}

nif::Transformable& nif::NiAVObject::transform()
{
	return m_transform;
}

nif::NiAVObject::AVObjectTransform::AVObjectTransform(NiAVObject& super) :
	m_super{ super },
	m_T(m_super, &super.getNative(), &native::NiAVObject::GetLocalTranslation, &native::NiAVObject::SetLocalTranslation),
	m_R(m_super, &super.getNative(), &native::NiAVObject::GetLocalRotation, &native::NiAVObject::SetLocalRotation),
	m_S(m_super, &super.getNative(), &native::NiAVObject::GetLocalScale, &native::NiAVObject::SetLocalScale)
{
}

nif::Property<nif::translation_t>& nif::NiAVObject::AVObjectTransform::translation()
{
	return m_T;
}

nif::Property<nif::rotation_t>& nif::NiAVObject::AVObjectTransform::rotation()
{
	return m_R;
}

nif::Property<nif::scale_t>& nif::NiAVObject::AVObjectTransform::scale()
{
	return m_S;
}
