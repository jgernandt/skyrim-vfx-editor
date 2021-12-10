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

const size_t nif::NiPSysModifier::TYPE = std::hash<std::string>{}("NiPSysModifier");
const size_t nif::NiPSysAgeDeathModifier::TYPE = std::hash<std::string>{}("NiPSysAgeDeathModifier");
const size_t nif::NiPSysBoundUpdateModifier::TYPE = std::hash<std::string>{}("NiPSysBoundUpdateModifier");
const size_t nif::NiPSysGravityModifier::TYPE = std::hash<std::string>{}("NiPSysGravityModifier");
const size_t nif::NiPSysPositionModifier::TYPE = std::hash<std::string>{}("NiPSysPositionModifier");
const size_t nif::NiPSysRotationModifier::TYPE = std::hash<std::string>{}("NiPSysRotationModifier");
const size_t nif::BSPSysScaleModifier::TYPE = std::hash<std::string>{}("BSPSysScaleModifier");
const size_t nif::BSPSysSimpleColorModifier::TYPE = std::hash<std::string>{}("BSPSysSimpleColorModifier");

using namespace math;

bool nif::ReadSyncer<nif::NiPSysModifier>::operator()(NiPSysModifier& object, const Niflib::NiPSysModifier* native, File& file)
{
	assert(native);
	object.name.set(native->GetName());
	object.order.set(native->GetOrder());
	auto target = file.get<NiParticleSystem>(native->GetTarget());
	file.keepAlive(target);
	object.target.assign(target);
	object.active.set(native->GetActive());

	return true;
}

bool nif::WriteSyncer<nif::NiPSysModifier>::operator()(const NiPSysModifier& object, Niflib::NiPSysModifier* native, const File& file)
{
	assert(native);
	native->SetName(object.name.get());
	native->SetOrder(object.order.get());
	native->SetTarget(file.getNative<NiParticleSystem>(object.target.assigned().get()));
	native->SetActive(object.active.get());

	return true;
}


bool nif::ReadSyncer<nif::NiPSysGravityModifier>::operator()(NiPSysGravityModifier& object, const Niflib::NiPSysGravityModifier* native, File& file)
{
	assert(native);
	auto gravityObject = file.get<NiNode>(native->GetGravityObject());
	file.keepAlive(gravityObject);
	object.gravityObject.assign(gravityObject);
	object.gravityAxis.set(nif_type_conversion<Floats<3>>::from(native->GetGravityAxis()));
	object.decay.set(native->GetDecay());
	object.strength.set(native->GetStrength());
	object.forceType.set(native->GetForceType());
	object.turbulence.set(native->GetTurbulence());
	object.turbulenceScale.set(native->GetTurbulenceScale());
	object.worldAligned.set(native->GetWorldAligned());

	return true;
}

bool nif::WriteSyncer<nif::NiPSysGravityModifier>::operator()(const NiPSysGravityModifier& object, Niflib::NiPSysGravityModifier* native, const File& file)
{
	assert(native);
	native->SetGravityObject(file.getNative<NiNode>(object.gravityObject.assigned().get()));
	native->SetGravityAxis(nif_type_conversion<Niflib::Vector3>::from(object.gravityAxis.get()));
	native->SetDecay(object.decay.get());
	native->SetStrength(object.strength.get());
	native->SetForceType(nif_type_conversion<Niflib::ForceType>::from(object.forceType.get()));
	native->SetTurbulence(object.turbulence.get());
	native->SetTurbulenceScale(object.turbulenceScale.get());
	native->SetWorldAligned(object.worldAligned.get());

	return true;
}


bool nif::ReadSyncer<nif::NiPSysRotationModifier>::operator()(NiPSysRotationModifier& object, const Niflib::NiPSysRotationModifier* native, File& file)
{
	assert(native);
	object.speed.set(static_cast<radf>(native->GetRotationSpeed()));
	object.speedVar.set(static_cast<radf>(native->GetRotationSpeedVar()));
	object.angle.set(static_cast<radf>(native->GetRotationAngle()));
	object.angleVar.set(static_cast<radf>(native->GetRotationAngleVar()));
	object.randomSign.set(native->GetRandomSpeedSign());

	return true;
}

bool nif::WriteSyncer<nif::NiPSysRotationModifier>::operator()(const NiPSysRotationModifier& object, Niflib::NiPSysRotationModifier* native, const File& file)
{
	assert(native);
	native->SetRotationSpeed(static_cast<radf>(object.speed.get()).value);
	native->SetRotationSpeedVar(static_cast<radf>(object.speedVar.get()).value);
	native->SetRotationAngle(static_cast<radf>(object.angle.get()).value);
	native->SetRotationAngleVar(static_cast<radf>(object.angleVar.get()).value);
	native->SetRandomSpeedSign(object.randomSign.get());

	return true;
}


bool nif::ReadSyncer<nif::BSPSysScaleModifier>::operator()(BSPSysScaleModifier& object, const Niflib::BSPSysScaleModifier* native, File& file)
{
	assert(native);
	object.scales.set(native->GetScales());

	return true;
}

bool nif::WriteSyncer<nif::BSPSysScaleModifier>::operator()(const BSPSysScaleModifier& object, Niflib::BSPSysScaleModifier* native, const File& file)
{
	assert(native);
	native->SetScales(object.scales.get());

	return true;
}


bool nif::ReadSyncer<nif::BSPSysSimpleColorModifier>::operator()(BSPSysSimpleColorModifier& object, const Niflib::BSPSysSimpleColorModifier* native, File& file)
{
	assert(native);
	object.col1.value.set(nif_type_conversion<ColRGBA>::from(native->GetColor(0)));
	object.col1.RGBend.set(native->GetColor1End());

	object.col2.value.set(nif_type_conversion<ColRGBA>::from(native->GetColor(1)));
	object.col2.RGBbegin.set(native->GetColor2Begin());
	object.col2.RGBend.set(native->GetColor2End());
	object.col2.Abegin.set(native->GetFadeInEnd());
	object.col2.Aend.set(native->GetFadeOutBegin());

	object.col3.value.set(nif_type_conversion<ColRGBA>::from(native->GetColor(2)));
	object.col3.RGBbegin.set(native->GetColor3Begin());

	return true;
}

bool nif::WriteSyncer<nif::BSPSysSimpleColorModifier>::operator()(const BSPSysSimpleColorModifier& object, Niflib::BSPSysSimpleColorModifier* native, const File& file)
{
	assert(native);
	native->SetColor(0, nif_type_conversion<Niflib::Color4>::from(object.col1.value.get()));
	native->SetColor1End(object.col1.RGBend.get());

	native->SetColor(1, nif_type_conversion<Niflib::Color4>::from(object.col2.value.get()));
	native->SetColor2Begin(object.col2.RGBbegin.get());
	native->SetColor2End(object.col2.RGBend.get());
	native->SetFadeInEnd(object.col2.Abegin.get());
	native->SetFadeOutBegin(object.col2.Aend.get());

	native->SetColor(2, nif_type_conversion<Niflib::Color4>::from(object.col3.value.get()));
	native->SetColor3Begin(object.col3.RGBbegin.get());

	return true;
}
