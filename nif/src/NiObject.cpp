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

const size_t nif::NiObject::TYPE = std::hash<std::string>{}("NiObject");
const size_t nif::NiObjectNET::TYPE = std::hash<std::string>{}("NiObjectNET");
const size_t nif::NiAVObject::TYPE = std::hash<std::string>{}("NiAVObject");

#ifdef _DEBUG
int g_currentNiObjects = 0;
#endif

nif::NiObject::NiObject()
{
#ifdef _DEBUG
	g_currentNiObjects++;
#endif
}

nif::NiObject::~NiObject()
{
#ifdef _DEBUG
	g_currentNiObjects--;
#endif
}


bool nif::Forwarder<nif::NiObjectNET>::operator()(NiObjectNET& object, NiTraverser& traverser)
{
	for (auto&& data : object.extraData) {
		assert(data);
		data->receive(traverser);
	}

	for (auto&& controller : object.controllers) {
		assert(controller);
		controller->receive(traverser);
	}

	return true;
}

bool nif::ReadSyncer<nif::NiObjectNET>::operator()(NiObjectNET& object, const Niflib::NiObjectNET* native, File& file)
{
	assert(native);

	object.name.set(const_cast<Niflib::NiObjectNET*>(native)->GetName());//this function should be const!

	object.extraData.clear();
	for (auto&& data : native->GetExtraData())
		if (data)
			object.extraData.add(file.get<NiExtraData>(data));

	object.controllers.clear();
	for (auto&& ctlr : native->GetControllers())
		if (ctlr)
			object.controllers.insert(object.controllers.size(), file.get<NiTimeController>(ctlr));

	return true;
}

bool nif::WriteSyncer<nif::NiObjectNET>::operator()(const NiObjectNET& object, Niflib::NiObjectNET* native, const File& file)
{
	assert(native);

	native->SetName(object.name.get());

	native->ClearExtraData();
	for (auto&& data : object.extraData)
		native->AddExtraData(file.getNative<NiExtraData>(data.get()), Niflib::VER_20_2_0_7);

	//Niflib adds to the front, so we reverse iterate here
	native->ClearControllers();
	for (auto rit = object.controllers.rbegin(); rit != object.controllers.rend(); ++rit)
		native->AddController(file.getNative<NiTimeController>(rit->get()));

	return true;
}

bool nif::ReadSyncer<nif::NiAVObject>::operator()(NiAVObject& object, const Niflib::NiAVObject* native, File& file)
{
	assert(native);

	object.flags.clear(-1);
	object.flags.raise(native->GetFlags());
	object.transform.translation.set(nif_type_conversion<translation_t>::from(native->GetLocalTranslation()));
	object.transform.rotation.set(nif_type_conversion<rotation_t>::from(native->GetLocalRotation()));
	object.transform.scale.set(native->GetLocalScale());

	return true;
}

bool nif::WriteSyncer<nif::NiAVObject>::operator()(const NiAVObject& object, Niflib::NiAVObject* native, const File& file)
{
	assert(native);

	native->SetFlags(object.flags.raised());
	native->SetLocalTranslation(nif_type_conversion<Niflib::Vector3>::from(object.transform.translation.get()));
	native->SetLocalRotation(nif_type_conversion<Niflib::Matrix33>::from(object.transform.rotation.get()));
	native->SetLocalScale(object.transform.scale.get());

	return true;
}

