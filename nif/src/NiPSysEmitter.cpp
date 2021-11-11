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

nif::NiPSysEmitterCtlr::NiPSysEmitterCtlr() : NiPSysEmitterCtlr(new Niflib::NiPSysEmitterCtlr) {}
nif::NiPSysEmitterCtlr::NiPSysEmitterCtlr(native_type* obj) :
	NiPSysModifierCtlr(obj), 
	m_visIplr(&getNative(), &native::NiPSysEmitterCtlr::GetVisibilityInterpolator, &native::NiPSysEmitterCtlr::SetVisibilityInterpolator)
{}

nif::native::NiPSysEmitterCtlr& nif::NiPSysEmitterCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysEmitterCtlr::TYPE));
	return static_cast<native::NiPSysEmitterCtlr&>(*m_ptr);
}


nif::NiPSysEmitter::NiPSysEmitter(native_type* obj) :
	NiPSysModifier(obj), 
	m_colour(&getNative(), &native::NiPSysEmitter::GetInitialColor, &native::NiPSysEmitter::SetInitialColor),
	m_lifeSpan(&getNative(), &native::NiPSysEmitter::GetLifeSpan, &native::NiPSysEmitter::SetLifeSpan),
	m_lifeSpanVar(&getNative(), &native::NiPSysEmitter::GetLifeSpanVar, &native::NiPSysEmitter::SetLifeSpanVar),
	m_size(&getNative(), &native::NiPSysEmitter::GetInitialRadius, &native::NiPSysEmitter::SetInitialRadius),
	m_sizeVar(&getNative(), &native::NiPSysEmitter::GetInitialRadiusVar, &native::NiPSysEmitter::SetInitialRadiusVar),
	m_speed(&getNative(), &native::NiPSysEmitter::GetSpeed, &native::NiPSysEmitter::SetSpeed),
	m_speedVar(&getNative(), &native::NiPSysEmitter::GetSpeedVar, &native::NiPSysEmitter::SetSpeedVar),
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
	m_emtrObj(&getNative(), &native::NiPSysVolumeEmitter::GetEmitterObject, &native::NiPSysVolumeEmitter::SetEmitterObject)
{}

nif::native::NiPSysVolumeEmitter& nif::NiPSysVolumeEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysVolumeEmitter::TYPE));
	return static_cast<native::NiPSysVolumeEmitter&>(*m_ptr);
}


nif::NiPSysBoxEmitter::NiPSysBoxEmitter() : NiPSysBoxEmitter(new Niflib::NiPSysBoxEmitter) {}
nif::NiPSysBoxEmitter::NiPSysBoxEmitter(native_type* obj) :
	NiPSysVolumeEmitter(obj), 
	m_width(&getNative(), &native::NiPSysBoxEmitter::GetWidth, &native::NiPSysBoxEmitter::SetWidth),
	m_height(&getNative(), &native::NiPSysBoxEmitter::GetHeight, &native::NiPSysBoxEmitter::SetHeight),
	m_depth(&getNative(), &native::NiPSysBoxEmitter::GetDepth, &native::NiPSysBoxEmitter::SetDepth)
{}

nif::native::NiPSysBoxEmitter& nif::NiPSysBoxEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysBoxEmitter::TYPE));
	return static_cast<native::NiPSysBoxEmitter&>(*m_ptr);
}


float nif::NiPSysEmitter::EmitterAzimuth::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetPlanarAngle())).value;
}

void nif::NiPSysEmitter::EmitterAzimuth::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetPlanarAngle(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterAzimuthVar::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetPlanarAngleVar())).value;
}

void nif::NiPSysEmitter::EmitterAzimuthVar::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetPlanarAngleVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterElevation::get() const
{
	return 90.0f - static_cast<math::deg>(math::rad(m_super.getNative().GetDeclination())).value;
}

void nif::NiPSysEmitter::EmitterElevation::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetDeclination(static_cast<math::rad>(math::deg(90.0f - f)).value);
		notify(f);
	}
}

float nif::NiPSysEmitter::EmitterElevationVar::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetDeclinationVar())).value;
}

void nif::NiPSysEmitter::EmitterElevationVar::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetDeclinationVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}


nif::NiPSysCylinderEmitter::NiPSysCylinderEmitter() : 
	NiPSysCylinderEmitter(new Niflib::NiPSysCylinderEmitter)
{}

nif::NiPSysCylinderEmitter::NiPSysCylinderEmitter(native_type* obj) :
	NiPSysVolumeEmitter(obj),
	m_radius(&getNative(), &native::NiPSysCylinderEmitter::GetRadius, &native::NiPSysCylinderEmitter::SetRadius),
	m_height(&getNative(), &native::NiPSysCylinderEmitter::GetHeight, &native::NiPSysCylinderEmitter::SetHeight)
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
	m_radius(&getNative(), &native::NiPSysSphereEmitter::GetRadius, &native::NiPSysSphereEmitter::SetRadius)
{}

nif::native::NiPSysSphereEmitter& nif::NiPSysSphereEmitter::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysSphereEmitter::TYPE));
	return static_cast<native::NiPSysSphereEmitter&>(*m_ptr);
}
