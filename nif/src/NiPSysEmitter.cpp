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

const size_t nif::NiPSysEmitter::TYPE = std::hash<std::string>{}("NiPSysEmitter");
const size_t nif::NiPSysVolumeEmitter::TYPE = std::hash<std::string>{}("NiPSysVolumeEmitter");
const size_t nif::NiPSysBoxEmitter::TYPE = std::hash<std::string>{}("NiPSysBoxEmitter");
const size_t nif::NiPSysCylinderEmitter::TYPE = std::hash<std::string>{}("NiPSysCylinderEmitter");
const size_t nif::NiPSysSphereEmitter::TYPE = std::hash<std::string>{}("NiPSysSphereEmitter");

using namespace math;

bool nif::ReadSyncer<nif::NiPSysEmitter>::operator()(NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file)
{
	assert(native);
	object.colour.set(nif_type_conversion<ColRGBA>::from(native->GetInitialColor()));
	object.lifeSpan.set(native->GetLifeSpan());
	object.lifeSpanVar.set(native->GetLifeSpanVar());
	object.size.set(native->GetInitialRadius());
	object.sizeVar.set(native->GetInitialRadiusVar());
	object.speed.set(native->GetSpeed());
	object.speedVar.set(native->GetSpeedVar());
	object.azimuth.set(radf(native->GetPlanarAngle()));
	object.azimuthVar.set(radf(native->GetPlanarAngleVar()));
	object.elevation.set(radf(0.5f * pi<float> - native->GetDeclination()));
	object.elevationVar.set(radf(native->GetDeclinationVar()));

	return true;
}

bool nif::WriteSyncer<nif::NiPSysEmitter>::operator()(const NiPSysEmitter& object, Niflib::NiPSysEmitter* native, const File& file)
{
	assert(native);
	native->SetInitialColor(nif_type_conversion<Niflib::Color4>::from(object.colour.get()));
	native->SetLifeSpan(object.lifeSpan.get());
	native->SetLifeSpanVar(object.lifeSpanVar.get());
	native->SetInitialRadius(object.size.get());
	native->SetInitialRadiusVar(object.sizeVar.get());
	native->SetSpeed(object.speed.get());
	native->SetSpeedVar(object.speedVar.get());
	native->SetPlanarAngle(static_cast<radf>(object.azimuth.get()).value);
	native->SetPlanarAngleVar(static_cast<radf>(object.azimuthVar.get()).value);
	native->SetDeclination(static_cast<radf>(degf(90.0f) - object.elevation.get()).value);
	native->SetDeclinationVar(static_cast<radf>(object.elevationVar.get()).value);

	return true;
}


bool nif::ReadSyncer<nif::NiPSysVolumeEmitter>::operator()(NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file)
{
	assert(native);
	auto emitterObject = file.get<NiNode>(native->GetEmitterObject());
	file.keepAlive(emitterObject);
	object.emitterObject.assign(emitterObject);
	return true;
}

bool nif::WriteSyncer<nif::NiPSysVolumeEmitter>::operator()(const NiPSysVolumeEmitter& object, Niflib::NiPSysVolumeEmitter* native, const File& file)
{
	assert(native);
	native->SetEmitterObject(file.getNative<NiNode>(object.emitterObject.assigned().get()));
	return true;
}


bool nif::ReadSyncer<nif::NiPSysBoxEmitter>::operator()(NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file)
{
	assert(native);
	object.width.set(native->GetWidth());
	object.height.set(native->GetHeight());
	object.depth.set(native->GetDepth());
	return true;
}

bool nif::WriteSyncer<nif::NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter& object, Niflib::NiPSysBoxEmitter* native, const File& file)
{
	assert(native);
	native->SetWidth(object.width.get());
	native->SetHeight(object.height.get());
	native->SetDepth(object.depth.get());
	return true;
}


bool nif::ReadSyncer<nif::NiPSysCylinderEmitter>::operator()(NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file)
{
	assert(native);
	object.radius.set(native->GetRadius());
	object.length.set(native->GetHeight());
	return true;
}

bool nif::WriteSyncer<nif::NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter& object, Niflib::NiPSysCylinderEmitter* native, const File& file)
{
	assert(native);
	native->SetRadius(object.radius.get());
	native->SetHeight(object.length.get());
	return true;
}


bool nif::ReadSyncer<nif::NiPSysSphereEmitter>::operator()(NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file)
{
	assert(native);
	object.radius.set(native->GetRadius());
	return true;
}

bool nif::WriteSyncer<nif::NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter& object, Niflib::NiPSysSphereEmitter* native, const File& file)
{
	assert(native);
	native->SetRadius(object.radius.get());
	return true;
}
