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

static nif::File::CreateFcn g_NiAVObjectFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiAVObject::TYPE), & nif::make_NiObject<nif::NiAVObject>);


void nif::NiSyncer<nif::NiAVObject>::syncReadImpl(File& file, NiAVObject* object, Niflib::NiAVObject* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiAVObject>::syncWriteImpl(File& file, NiAVObject* object, Niflib::NiAVObject* native) const
{
	//TODO
}


/*nif::NiAVObject::NiAVObject(File& file, native_type* obj) :
	NiObjectNET(file, obj),
	transform{ nif_type_conversion<translation_t>::from(obj->GetLocalTranslation()),
		nif_type_conversion<rotation_t>::from(obj->GetLocalRotation()),
		obj->GetLocalScale() }
{}

void nif::NiAVObject::sync(const File& file)
{
	native().SetLocalTranslation(nif_type_conversion<native::translation_t>::from(transform.translation.get()));
	native().SetLocalRotation(nif_type_conversion<native::rotation_t>::from(transform.rotation.get()));
	native().SetLocalScale(transform.scale.get());
}

nif::native::NiAVObject& nif::NiAVObject::native() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiAVObject::TYPE));
	return static_cast<native::NiAVObject&>(*m_ptr);
}*/
