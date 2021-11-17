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
#include "NiPSysEmitter.h"
#include "File.h"

static nif::File::CreateFcn g_NiPSysEmitterCtlrFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitterCtlr::TYPE), & nif::make_NiObject<nif::NiPSysEmitterCtlr>);

static nif::File::CreateFcn g_NiPSysEmitterFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitter::TYPE), & nif::make_NiObject<nif::NiPSysEmitter>);

static nif::File::CreateFcn g_NiPSysVolumeEmitterFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysVolumeEmitter::TYPE), & nif::make_NiObject<nif::NiPSysVolumeEmitter>);

static nif::File::CreateFcn g_NiPSysBoxEmitterFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoxEmitter::TYPE), & nif::make_NiObject<nif::NiPSysBoxEmitter>);

static nif::File::CreateFcn g_NiPSysCylinderEmitterFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysCylinderEmitter::TYPE), & nif::make_NiObject<nif::NiPSysCylinderEmitter>);

static nif::File::CreateFcn g_NiPSysSphereEmitterFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysSphereEmitter::TYPE), & nif::make_NiObject<nif::NiPSysSphereEmitter>);


void nif::NiSyncer<nif::NiPSysEmitterCtlr>::syncReadImpl(
	File& file, NiPSysEmitterCtlr* object, Niflib::NiPSysEmitterCtlr* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysEmitterCtlr>::syncWriteImpl(
	File& file, NiPSysEmitterCtlr* object, Niflib::NiPSysEmitterCtlr* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysEmitter>::syncReadImpl(
	File& file, NiPSysEmitter* object, Niflib::NiPSysEmitter* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysEmitter>::syncWriteImpl(
	File& file, NiPSysEmitter* object, Niflib::NiPSysEmitter* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysVolumeEmitter>::syncReadImpl(
	File& file, NiPSysVolumeEmitter* object, Niflib::NiPSysVolumeEmitter* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysVolumeEmitter>::syncWriteImpl(
	File& file, NiPSysVolumeEmitter* object, Niflib::NiPSysVolumeEmitter* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysBoxEmitter>::syncReadImpl(
	File& file, NiPSysBoxEmitter* object, Niflib::NiPSysBoxEmitter* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysBoxEmitter>::syncWriteImpl(
	File& file, NiPSysBoxEmitter* object, Niflib::NiPSysBoxEmitter* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysCylinderEmitter>::syncReadImpl(
	File& file, NiPSysCylinderEmitter* object, Niflib::NiPSysCylinderEmitter* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysCylinderEmitter>::syncWriteImpl(
	File& file, NiPSysCylinderEmitter* object, Niflib::NiPSysCylinderEmitter* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysSphereEmitter>::syncReadImpl(
	File& file, NiPSysSphereEmitter* object, Niflib::NiPSysSphereEmitter* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysSphereEmitter>::syncWriteImpl(
	File& file, NiPSysSphereEmitter* object, Niflib::NiPSysSphereEmitter* native) const
{
	//TODO
}

/*nif::NiPSysEmitterCtlr::NiPSysEmitterCtlr() : NiPSysEmitterCtlr(new Niflib::NiPSysEmitterCtlr) {}
nif::NiPSysEmitterCtlr::NiPSysEmitterCtlr(native_type* obj) :
	NiPSysModifierCtlr(obj), 
	m_visIplr(*this, &getNative(), &native::NiPSysEmitterCtlr::GetVisibilityInterpolator, &native::NiPSysEmitterCtlr::SetVisibilityInterpolator)
{}

nif::native::NiPSysEmitterCtlr& nif::NiPSysEmitterCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysEmitterCtlr::TYPE));
	return static_cast<native::NiPSysEmitterCtlr&>(*m_ptr);
}


nif::NiPSysEmitter::NiPSysEmitter(native_type* obj) :
	NiPSysModifier(obj), 
	m_colour(*this, &getNative(), &native::NiPSysEmitter::GetInitialColor, &native::NiPSysEmitter::SetInitialColor),
	m_lifeSpan(*this, &getNative(), &native::NiPSysEmitter::GetLifeSpan, &native::NiPSysEmitter::SetLifeSpan),
	m_lifeSpanVar(*this, &getNative(), &native::NiPSysEmitter::GetLifeSpanVar, &native::NiPSysEmitter::SetLifeSpanVar),
	m_size(*this, &getNative(), &native::NiPSysEmitter::GetInitialRadius, &native::NiPSysEmitter::SetInitialRadius),
	m_sizeVar(*this, &getNative(), &native::NiPSysEmitter::GetInitialRadiusVar, &native::NiPSysEmitter::SetInitialRadiusVar),
	m_speed(*this, &getNative(), &native::NiPSysEmitter::GetSpeed, &native::NiPSysEmitter::SetSpeed),
	m_speedVar(*this, &getNative(), &native::NiPSysEmitter::GetSpeedVar, &native::NiPSysEmitter::SetSpeedVar),
	m_azimuth(*this),
	m_azimuthVar(*this),
	m_elevation(*this),
	m_elevationVar(*this)
{}

nif::native::NiPSysEmitter& nif::NiPSysEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysEmitter::TYPE));
	return static_cast<native::NiPSysEmitter&>(*m_ptr);
}


nif::NiPSysVolumeEmitter::NiPSysVolumeEmitter(native_type* obj) :
	NiPSysEmitter(obj), 
	m_emtrObj(*this, &getNative(), &native::NiPSysVolumeEmitter::GetEmitterObject, &native::NiPSysVolumeEmitter::SetEmitterObject)
{}

nif::native::NiPSysVolumeEmitter& nif::NiPSysVolumeEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysVolumeEmitter::TYPE));
	return static_cast<native::NiPSysVolumeEmitter&>(*m_ptr);
}


nif::NiPSysBoxEmitter::NiPSysBoxEmitter() : NiPSysBoxEmitter(new Niflib::NiPSysBoxEmitter) {}
nif::NiPSysBoxEmitter::NiPSysBoxEmitter(native_type* obj) :
	NiPSysVolumeEmitter(obj), 
	m_width(*this, &getNative(), &native::NiPSysBoxEmitter::GetWidth, &native::NiPSysBoxEmitter::SetWidth),
	m_height(*this, &getNative(), &native::NiPSysBoxEmitter::GetHeight, &native::NiPSysBoxEmitter::SetHeight),
	m_depth(*this, &getNative(), &native::NiPSysBoxEmitter::GetDepth, &native::NiPSysBoxEmitter::SetDepth)
{}

nif::native::NiPSysBoxEmitter& nif::NiPSysBoxEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysBoxEmitter::TYPE));
	return static_cast<native::NiPSysBoxEmitter&>(*m_ptr);
}


float nif::NiPSysEmitter::EmitterAzimuth::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetPlanarAngle())).value;
}

void nif::NiPSysEmitter::EmitterAzimuth::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetPlanarAngle(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterAzimuthVar::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetPlanarAngleVar())).value;
}

void nif::NiPSysEmitter::EmitterAzimuthVar::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetPlanarAngleVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterElevation::get() const
{
	return 90.0f - static_cast<math::deg>(math::rad(nativePtr()->GetDeclination())).value;
}

void nif::NiPSysEmitter::EmitterElevation::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetDeclination(static_cast<math::rad>(math::deg(90.0f - f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterElevationVar::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetDeclinationVar())).value;
}

void nif::NiPSysEmitter::EmitterElevationVar::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetDeclinationVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}


nif::NiPSysCylinderEmitter::NiPSysCylinderEmitter() : 
	NiPSysCylinderEmitter(new Niflib::NiPSysCylinderEmitter)
{}

nif::NiPSysCylinderEmitter::NiPSysCylinderEmitter(native_type* obj) :
	NiPSysVolumeEmitter(obj),
	m_radius(*this, &getNative(), &native::NiPSysCylinderEmitter::GetRadius, &native::NiPSysCylinderEmitter::SetRadius),
	m_height(*this, &getNative(), &native::NiPSysCylinderEmitter::GetHeight, &native::NiPSysCylinderEmitter::SetHeight)
{}

nif::native::NiPSysCylinderEmitter& nif::NiPSysCylinderEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysCylinderEmitter::TYPE));
	return static_cast<native::NiPSysCylinderEmitter&>(*m_ptr);
}


nif::NiPSysSphereEmitter::NiPSysSphereEmitter() :
	NiPSysSphereEmitter(new Niflib::NiPSysSphereEmitter)
{}

nif::NiPSysSphereEmitter::NiPSysSphereEmitter(native_type* obj) :
	NiPSysVolumeEmitter(obj),
	m_radius(*this, &getNative(), &native::NiPSysSphereEmitter::GetRadius, &native::NiPSysSphereEmitter::SetRadius)
{}

nif::native::NiPSysSphereEmitter& nif::NiPSysSphereEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysSphereEmitter::TYPE));
	return static_cast<native::NiPSysSphereEmitter&>(*m_ptr);
}*/
