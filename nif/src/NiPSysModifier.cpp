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
#include "File.h"

static nif::File::CreateFcn g_NiPSysModifierCtlrFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifierCtlr::TYPE), & nif::make_NiObject<nif::NiPSysModifierCtlr>);

static nif::File::CreateFcn g_NiPSysUpdateCtlrFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysUpdateCtlr::TYPE), & nif::make_NiObject<nif::NiPSysUpdateCtlr>);

static nif::File::CreateFcn g_NiPSysModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifier::TYPE), & nif::make_NiObject<nif::NiPSysModifier>);

static nif::File::CreateFcn g_NiPSysBoundUpdateModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoundUpdateModifier::TYPE), & nif::make_NiObject<nif::NiPSysBoundUpdateModifier>);

static nif::File::CreateFcn g_NiPSysAgeDeathModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysAgeDeathModifier::TYPE), & nif::make_NiObject<nif::NiPSysAgeDeathModifier>);

static nif::File::CreateFcn g_NiPSysPositionModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysPositionModifier::TYPE), & nif::make_NiObject<nif::NiPSysPositionModifier>);

static nif::File::CreateFcn g_NiPSysGravityModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysGravityModifier::TYPE), & nif::make_NiObject<nif::NiPSysGravityModifier>);

static nif::File::CreateFcn g_NiPSysRotationModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::NiPSysRotationModifier::TYPE), & nif::make_NiObject<nif::NiPSysRotationModifier>);

static nif::File::CreateFcn g_BSPSysScaleModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::BSPSysScaleModifier::TYPE), & nif::make_NiObject<nif::BSPSysScaleModifier>);

static nif::File::CreateFcn g_BSPSysSimpleColorModifierFactory =
nif::File::pushType(std::hash<const Niflib::Type*>{}(&Niflib::BSPSysSimpleColorModifier::TYPE), & nif::make_NiObject<nif::BSPSysSimpleColorModifier>);


void nif::NiSyncer<nif::NiPSysModifierCtlr>::syncReadImpl(
	File& file, NiPSysModifierCtlr* object, Niflib::NiPSysModifierCtlr* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysModifierCtlr>::syncWriteImpl(
	const File& file, NiPSysModifierCtlr* object, Niflib::NiPSysModifierCtlr* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysModifier>::syncReadImpl(
	File& file, NiPSysModifier* object, Niflib::NiPSysModifier* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysModifier>::syncWriteImpl(
	const File& file, NiPSysModifier* object, Niflib::NiPSysModifier* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysGravityModifier>::syncReadImpl(
	File& file, NiPSysGravityModifier* object, Niflib::NiPSysGravityModifier* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysGravityModifier>::syncWriteImpl(
	const File& file, NiPSysGravityModifier* object, Niflib::NiPSysGravityModifier* native) const
{
	//TODO
}

void nif::NiSyncer<nif::NiPSysRotationModifier>::syncReadImpl(
	File& file, NiPSysRotationModifier* object, Niflib::NiPSysRotationModifier* native) const
{
	//TODO
}
void nif::NiSyncer<nif::NiPSysRotationModifier>::syncWriteImpl(
	const File& file, NiPSysRotationModifier* object, Niflib::NiPSysRotationModifier* native) const
{
	//TODO
}

void nif::NiSyncer<nif::BSPSysScaleModifier>::syncReadImpl(
	File& file, BSPSysScaleModifier* object, Niflib::BSPSysScaleModifier* native) const
{
	//TODO
}
void nif::NiSyncer<nif::BSPSysScaleModifier>::syncWriteImpl(
	const File& file, BSPSysScaleModifier* object, Niflib::BSPSysScaleModifier* native) const
{
	//TODO
}

void nif::NiSyncer<nif::BSPSysSimpleColorModifier>::syncReadImpl(
	File& file, BSPSysSimpleColorModifier* object, Niflib::BSPSysSimpleColorModifier* native) const
{
	//TODO
}
void nif::NiSyncer<nif::BSPSysSimpleColorModifier>::syncWriteImpl(
	const File& file, BSPSysSimpleColorModifier* object, Niflib::BSPSysSimpleColorModifier* native) const
{
	//TODO
}



/*nif::NiPSysModifierCtlr::NiPSysModifierCtlr(native_type* obj) :
	NiSingleInterpController(obj), 
	m_modName(*this, &getNative(), &native::NiPSysModifierCtlr::GetModifierName, &native::NiPSysModifierCtlr::SetModifierName)
{}

nif::native::NiPSysModifierCtlr& nif::NiPSysModifierCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysModifierCtlr::TYPE));
	return static_cast<native::NiPSysModifierCtlr&>(*m_ptr);
}


nif::NiPSysModifier::NiPSysModifier(native_type* obj) :
	NiObject(obj),
	m_name(*this, &getNative(), &native::NiPSysModifier::GetName, &native::NiPSysModifier::SetName),
	m_order(*this, &getNative(), &native::NiPSysModifier::GetOrder, &native::NiPSysModifier::SetOrder),
	m_target(*this, &getNative(), &native::NiPSysModifier::GetTarget, &native::NiPSysModifier::SetTarget),
	m_active(*this, &getNative(), &native::NiPSysModifier::GetActive, &native::NiPSysModifier::SetActive)
{}

nif::native::NiPSysModifier& nif::NiPSysModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysModifier::TYPE));
	return static_cast<native::NiPSysModifier&>(*m_ptr);
}


nif::NiPSysUpdateCtlr::NiPSysUpdateCtlr() : NiPSysUpdateCtlr(new Niflib::NiPSysUpdateCtlr) {}
nif::NiPSysUpdateCtlr::NiPSysUpdateCtlr(native_type* obj) : NiTimeController(obj) {}

nif::native::NiPSysUpdateCtlr& nif::NiPSysUpdateCtlr::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysUpdateCtlr::TYPE));
	return static_cast<native::NiPSysUpdateCtlr&>(*m_ptr);
}


nif::NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier() :
	NiPSysBoundUpdateModifier(new Niflib::NiPSysBoundUpdateModifier)
{}

nif::NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier(native_type* obj) :
	NiPSysModifier(obj)
{}

nif::native::NiPSysBoundUpdateModifier& nif::NiPSysBoundUpdateModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysBoundUpdateModifier::TYPE));
	return static_cast<native::NiPSysBoundUpdateModifier&>(*m_ptr);
}


nif::NiPSysAgeDeathModifier::NiPSysAgeDeathModifier() : NiPSysAgeDeathModifier(new Niflib::NiPSysAgeDeathModifier) {}
nif::NiPSysAgeDeathModifier::NiPSysAgeDeathModifier(native_type* obj) : NiPSysModifier(obj) {}

nif::native::NiPSysAgeDeathModifier& nif::NiPSysAgeDeathModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysAgeDeathModifier::TYPE));
	return static_cast<native::NiPSysAgeDeathModifier&>(*m_ptr);
}


nif::NiPSysGravityModifier::NiPSysGravityModifier() : NiPSysGravityModifier(new Niflib::NiPSysGravityModifier) {}
nif::NiPSysGravityModifier::NiPSysGravityModifier(native_type* obj) :
	NiPSysModifier(obj),
	m_gravityObj(*this, &getNative(), &native::NiPSysGravityModifier::GetGravityObject, &native::NiPSysGravityModifier::SetGravityObject),
	m_gravityAxis(*this, &getNative(), &native::NiPSysGravityModifier::GetGravityAxis, &native::NiPSysGravityModifier::SetGravityAxis),
	m_decay(*this, &getNative(), &native::NiPSysGravityModifier::GetDecay, &native::NiPSysGravityModifier::SetDecay),
	m_strength(*this, &getNative(), &native::NiPSysGravityModifier::GetStrength, &native::NiPSysGravityModifier::SetStrength),
	m_forceType(*this, &getNative(), &native::NiPSysGravityModifier::GetForceType, &native::NiPSysGravityModifier::SetForceType),
	m_turbulence(*this, &getNative(), &native::NiPSysGravityModifier::GetTurbulence, &native::NiPSysGravityModifier::SetTurbulence),
	m_turbulenceScale(*this, &getNative(), &native::NiPSysGravityModifier::GetTurbulenceScale, &native::NiPSysGravityModifier::SetTurbulenceScale),
	m_worldAligned(*this, &getNative(), &native::NiPSysGravityModifier::GetWorldAligned, &native::NiPSysGravityModifier::SetWorldAligned)
{}

nif::native::NiPSysGravityModifier& nif::NiPSysGravityModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysGravityModifier::TYPE));
	return static_cast<native::NiPSysGravityModifier&>(*m_ptr);
}


nif::NiPSysPositionModifier::NiPSysPositionModifier() : NiPSysPositionModifier(new Niflib::NiPSysPositionModifier) {}
nif::NiPSysPositionModifier::NiPSysPositionModifier(native_type* obj) : NiPSysModifier(obj) {}

nif::native::NiPSysPositionModifier& nif::NiPSysPositionModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysPositionModifier::TYPE));
	return static_cast<native::NiPSysPositionModifier&>(*m_ptr);
}


nif::NiPSysRotationModifier::NiPSysRotationModifier() : NiPSysRotationModifier(new Niflib::NiPSysRotationModifier) {}
nif::NiPSysRotationModifier::NiPSysRotationModifier(native_type* obj) :
	NiPSysModifier(obj),
	m_speed(*this),
	m_speedVar(*this),
	m_angle(*this),
	m_angleVar(*this),
	//m_speed(&getNative(), &native::NiPSysRotationModifier::GetRotationSpeed, &native::NiPSysRotationModifier::SetRotationSpeed),
	//m_speedVar(&getNative(), &native::NiPSysRotationModifier::GetRotationSpeedVar, &native::NiPSysRotationModifier::SetRotationSpeedVar),
	//m_angle(&getNative(), &native::NiPSysRotationModifier::GetRotationAngle, &native::NiPSysRotationModifier::SetRotationAngle),
	//m_angleVar(&getNative(), &native::NiPSysRotationModifier::GetRotationAngleVar, &native::NiPSysRotationModifier::SetRotationAngleVar),
	m_randomSign(*this, &getNative(), &native::NiPSysRotationModifier::GetRandomSpeedSign, &native::NiPSysRotationModifier::SetRandomSpeedSign)
{}

nif::native::NiPSysRotationModifier& nif::NiPSysRotationModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiPSysRotationModifier::TYPE));
	return static_cast<native::NiPSysRotationModifier&>(*m_ptr);
}


nif::BSPSysScaleModifier::BSPSysScaleModifier() : BSPSysScaleModifier(new Niflib::BSPSysScaleModifier) {}
nif::BSPSysScaleModifier::BSPSysScaleModifier(native_type* obj) :
	NiPSysModifier(obj),
	m_scales(*this, &getNative(), &native::BSPSysScaleModifier::GetScales, &native::BSPSysScaleModifier::SetScales)
{}

nif::native::BSPSysScaleModifier& nif::BSPSysScaleModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSPSysScaleModifier::TYPE));
	return static_cast<native::BSPSysScaleModifier&>(*m_ptr);
}


nif::BSPSysSimpleColorModifier::BSPSysSimpleColorModifier() : BSPSysSimpleColorModifier(new Niflib::BSPSysSimpleColorModifier) {}
nif::BSPSysSimpleColorModifier::BSPSysSimpleColorModifier(native_type* obj) :
	NiPSysModifier(obj),
	m_fadeInEnd(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetFadeInEnd, &native::BSPSysSimpleColorModifier::SetFadeInEnd),
	m_fadeOutBegin(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetFadeOutBegin, &native::BSPSysSimpleColorModifier::SetFadeOutBegin),
	m_col1End(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetColor1End, &native::BSPSysSimpleColorModifier::SetColor1End),
	m_col2Begin(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetColor2Begin, &native::BSPSysSimpleColorModifier::SetColor2Begin),
	m_col2End(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetColor2End, &native::BSPSysSimpleColorModifier::SetColor2End),
	m_col3Begin(*this, &getNative(), &native::BSPSysSimpleColorModifier::GetColor3Begin, &native::BSPSysSimpleColorModifier::SetColor3Begin),
	m_col1(*this, [this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(0)); },
		[this](const ColRGBA& col) { getNative().SetColor(0, nif_type_conversion<native::ColRGBA>::from(col)); }),
	m_col2(*this, [this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(1)); },
		[this](const ColRGBA& col) { getNative().SetColor(1, nif_type_conversion<native::ColRGBA>::from(col)); }),
	m_col3(*this, [this]() { return nif_type_conversion<ColRGBA>::from(getNative().GetColor(2)); },
		[this](const ColRGBA& col) { getNative().SetColor(2, nif_type_conversion<native::ColRGBA>::from(col)); })
{}

nif::native::BSPSysSimpleColorModifier& nif::BSPSysSimpleColorModifier::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSPSysSimpleColorModifier::TYPE));
	return static_cast<native::BSPSysSimpleColorModifier&>(*m_ptr);
}

float nif::NiPSysRotationModifier::Speed::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetRotationSpeed())).value;
}

void nif::NiPSysRotationModifier::Speed::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetRotationSpeed(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::SpeedVar::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetRotationSpeedVar())).value;
}

void nif::NiPSysRotationModifier::SpeedVar::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetRotationSpeedVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::Angle::get() const
{
	return static_cast<math::deg>(math::rad(nativePtr()->GetRotationAngle())).value;
}

void nif::NiPSysRotationModifier::Angle::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetRotationAngle(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}

float nif::NiPSysRotationModifier::AngleVar::get() const
{
	
	return static_cast<math::deg>(math::rad(nativePtr()->GetRotationAngleVar())).value;
}

void nif::NiPSysRotationModifier::AngleVar::set(const float& f)
{
	if (f != get()) {
		nativePtr()->SetRotationAngleVar(static_cast<math::rad>(math::deg(f)).value);
		notify(f);
	}
}*/
