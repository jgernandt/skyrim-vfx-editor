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
#include "NiPSysModifier.h"

nif::NiPSysModifierCtlr::NiPSysModifierCtlr(native::NiPSysModifierCtlr* obj) : 
	NiSingleInterpController(obj), 
	m_modName(&getNative(), &native::NiPSysModifierCtlr::GetModifierName, &native::NiPSysModifierCtlr::SetModifierName)
{}

nif::native::NiPSysModifierCtlr& nif::NiPSysModifierCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysModifierCtlr::TYPE));
	return static_cast<native::NiPSysModifierCtlr&>(*m_ptr);
}


nif::NiPSysModifier::NiPSysModifier(native::NiPSysModifier* obj) : 
	NiObject(obj),
	m_name(&getNative(), &native::NiPSysModifier::GetName, &native::NiPSysModifier::SetName),
	m_order(&getNative(), &native::NiPSysModifier::GetOrder, &native::NiPSysModifier::SetOrder),
	m_target(&getNative(), &native::NiPSysModifier::GetTarget, &native::NiPSysModifier::SetTarget),
	m_active(&getNative(), &native::NiPSysModifier::GetActive, &native::NiPSysModifier::SetActive)
{}

nif::native::NiPSysModifier& nif::NiPSysModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysModifier::TYPE));
	return static_cast<native::NiPSysModifier&>(*m_ptr);
}


nif::NiPSysUpdateCtlr::NiPSysUpdateCtlr() : NiPSysUpdateCtlr(new Niflib::NiPSysUpdateCtlr) {}
nif::NiPSysUpdateCtlr::NiPSysUpdateCtlr(native::NiPSysUpdateCtlr* obj) : NiTimeController(obj) {}

nif::native::NiPSysUpdateCtlr& nif::NiPSysUpdateCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysUpdateCtlr::TYPE));
	return static_cast<native::NiPSysUpdateCtlr&>(*m_ptr);
}


nif::NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier() :
	NiPSysBoundUpdateModifier(new Niflib::NiPSysBoundUpdateModifier)
{}

nif::NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier(native::NiPSysBoundUpdateModifier* obj) :
	NiPSysModifier(obj)
{}

nif::native::NiPSysBoundUpdateModifier& nif::NiPSysBoundUpdateModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysBoundUpdateModifier::TYPE));
	return static_cast<native::NiPSysBoundUpdateModifier&>(*m_ptr);
}


nif::NiPSysAgeDeathModifier::NiPSysAgeDeathModifier() : NiPSysAgeDeathModifier(new Niflib::NiPSysAgeDeathModifier) {}
nif::NiPSysAgeDeathModifier::NiPSysAgeDeathModifier(native::NiPSysAgeDeathModifier* obj) : NiPSysModifier(obj) {}

nif::native::NiPSysAgeDeathModifier& nif::NiPSysAgeDeathModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysAgeDeathModifier::TYPE));
	return static_cast<native::NiPSysAgeDeathModifier&>(*m_ptr);
}


nif::NiPSysGravityModifier::NiPSysGravityModifier() : NiPSysGravityModifier(new Niflib::NiPSysGravityModifier) {}
nif::NiPSysGravityModifier::NiPSysGravityModifier(native::NiPSysGravityModifier* obj) : 
	NiPSysModifier(obj),
	m_gravityObj(&getNative(), &native::NiPSysGravityModifier::GetGravityObject, &native::NiPSysGravityModifier::SetGravityObject),
	m_gravityAxis(&getNative(), &native::NiPSysGravityModifier::GetGravityAxis, &native::NiPSysGravityModifier::SetGravityAxis),
	m_decay(&getNative(), &native::NiPSysGravityModifier::GetDecay, &native::NiPSysGravityModifier::SetDecay),
	m_strength(&getNative(), &native::NiPSysGravityModifier::GetStrength, &native::NiPSysGravityModifier::SetStrength),
	m_forceType(&getNative(), &native::NiPSysGravityModifier::GetForceType, &native::NiPSysGravityModifier::SetForceType),
	m_turbulence(&getNative(), &native::NiPSysGravityModifier::GetTurbulence, &native::NiPSysGravityModifier::SetTurbulence),
	m_turbulenceScale(&getNative(), &native::NiPSysGravityModifier::GetTurbulenceScale, &native::NiPSysGravityModifier::SetTurbulenceScale),
	m_worldAligned(&getNative(), &native::NiPSysGravityModifier::GetWorldAligned, &native::NiPSysGravityModifier::SetWorldAligned)
{}

nif::native::NiPSysGravityModifier& nif::NiPSysGravityModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysGravityModifier::TYPE));
	return static_cast<native::NiPSysGravityModifier&>(*m_ptr);
}


nif::NiPSysPositionModifier::NiPSysPositionModifier() : NiPSysPositionModifier(new Niflib::NiPSysPositionModifier) {}
nif::NiPSysPositionModifier::NiPSysPositionModifier(native::NiPSysPositionModifier* obj) : NiPSysModifier(obj) {}

nif::native::NiPSysPositionModifier& nif::NiPSysPositionModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysPositionModifier::TYPE));
	return static_cast<native::NiPSysPositionModifier&>(*m_ptr);
}


nif::NiPSysRotationModifier::NiPSysRotationModifier() : NiPSysRotationModifier(new Niflib::NiPSysRotationModifier) {}
nif::NiPSysRotationModifier::NiPSysRotationModifier(native::NiPSysRotationModifier* obj) : 
	NiPSysModifier(obj),
	m_speed(*this),
	m_speedVar(*this),
	m_angle(*this),
	m_angleVar(*this),
	//m_speed(&getNative(), &native::NiPSysRotationModifier::GetRotationSpeed, &native::NiPSysRotationModifier::SetRotationSpeed),
	//m_speedVar(&getNative(), &native::NiPSysRotationModifier::GetRotationSpeedVar, &native::NiPSysRotationModifier::SetRotationSpeedVar),
	//m_angle(&getNative(), &native::NiPSysRotationModifier::GetRotationAngle, &native::NiPSysRotationModifier::SetRotationAngle),
	//m_angleVar(&getNative(), &native::NiPSysRotationModifier::GetRotationAngleVar, &native::NiPSysRotationModifier::SetRotationAngleVar),
	m_randomSign(&getNative(), &native::NiPSysRotationModifier::GetRandomSpeedSign, &native::NiPSysRotationModifier::SetRandomSpeedSign)
{}

nif::native::NiPSysRotationModifier& nif::NiPSysRotationModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysRotationModifier::TYPE));
	return static_cast<native::NiPSysRotationModifier&>(*m_ptr);
}


nif::BSPSysScaleModifier::BSPSysScaleModifier() : BSPSysScaleModifier(new Niflib::BSPSysScaleModifier) {}
nif::BSPSysScaleModifier::BSPSysScaleModifier(native::BSPSysScaleModifier* obj) :
	NiPSysModifier(obj),
	m_scales(&getNative(), &native::BSPSysScaleModifier::GetScales, &native::BSPSysScaleModifier::SetScales)
{}

nif::native::BSPSysScaleModifier& nif::BSPSysScaleModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSPSysScaleModifier::TYPE));
	return static_cast<native::BSPSysScaleModifier&>(*m_ptr);
}


nif::BSPSysSimpleColorModifier::BSPSysSimpleColorModifier() : BSPSysSimpleColorModifier(new Niflib::BSPSysSimpleColorModifier) {}
nif::BSPSysSimpleColorModifier::BSPSysSimpleColorModifier(native::BSPSysSimpleColorModifier* obj) :
	NiPSysModifier(obj),
	m_fadeInEnd(&getNative(), &native::BSPSysSimpleColorModifier::GetFadeInEnd, &native::BSPSysSimpleColorModifier::SetFadeInEnd),
	m_fadeOutBegin(&getNative(), &native::BSPSysSimpleColorModifier::GetFadeOutBegin, &native::BSPSysSimpleColorModifier::SetFadeOutBegin),
	m_col1End(&getNative(), &native::BSPSysSimpleColorModifier::GetColor1End, &native::BSPSysSimpleColorModifier::SetColor1End),
	m_col2Begin(&getNative(), &native::BSPSysSimpleColorModifier::GetColor2Begin, &native::BSPSysSimpleColorModifier::SetColor2Begin),
	m_col2End(&getNative(), &native::BSPSysSimpleColorModifier::GetColor2End, &native::BSPSysSimpleColorModifier::SetColor2End),
	m_col3Begin(&getNative(), &native::BSPSysSimpleColorModifier::GetColor3Begin, &native::BSPSysSimpleColorModifier::SetColor3Begin),
	m_col1([this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(0)); },
		[this](const ColRGBA& col) { getNative().SetColor(0, nif_type_conversion<native::ColRGBA>::from(col)); }),
	m_col2([this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(1)); },
		[this](const ColRGBA& col) { getNative().SetColor(1, nif_type_conversion<native::ColRGBA>::from(col)); }),
	m_col3([this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(2)); },
		[this](const ColRGBA& col) { getNative().SetColor(2, nif_type_conversion<native::ColRGBA>::from(col)); })
{}

nif::native::BSPSysSimpleColorModifier& nif::BSPSysSimpleColorModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSPSysSimpleColorModifier::TYPE));
	return static_cast<native::BSPSysSimpleColorModifier&>(*m_ptr);
}

float nif::NiPSysRotationModifier::Speed::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetRotationSpeed())).value;
}

void nif::NiPSysRotationModifier::Speed::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetRotationSpeed(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::SpeedVar::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetRotationSpeedVar())).value;
}

void nif::NiPSysRotationModifier::SpeedVar::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetRotationSpeedVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::Angle::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetRotationAngle())).value;
}

void nif::NiPSysRotationModifier::Angle::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetRotationAngle(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::AngleVar::get() const
{
	return static_cast<math::deg>(math::rad(m_super.getNative().GetRotationAngleVar())).value;
}

void nif::NiPSysRotationModifier::AngleVar::set(const float& f)
{
	if (f != get()) {
		m_super.getNative().SetRotationAngleVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}
