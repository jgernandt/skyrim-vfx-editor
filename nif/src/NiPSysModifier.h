//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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

#pragma once
#include "NiObjectNET.h"
#include "NiController.h"
#include "NiParticleSystem.h"
#include "NiNode.h"

namespace nif
{
	class NiNode;
	class NiParticleSystem;

	class NiPSysModifierCtlr : public NiSingleInterpController
	{
	public:
		NiPSysModifierCtlr(native::NiPSysModifierCtlr* obj);
		NiPSysModifierCtlr(const NiPSysModifierCtlr&) = delete;

		virtual ~NiPSysModifierCtlr() = default;

		NiPSysModifierCtlr& operator=(const NiPSysModifierCtlr&) = delete;

		native::NiPSysModifierCtlr& getNative() const;

		IProperty<std::string>& modifierName() { return m_modName; }

	private:
		Property<std::string> m_modName;
	};

	class NiPSysModifier : public NiObject
	{
	public:
		NiPSysModifier(native::NiPSysModifier* obj);
		NiPSysModifier(const NiPSysModifier&) = delete;

		virtual ~NiPSysModifier() = default;

		NiPSysModifier& operator=(const NiPSysModifier&) = delete;

		native::NiPSysModifier& getNative() const;

		IProperty<std::string>& name() { return m_name; }
		const IProperty<std::string>& name() const { return m_name; }
		
		IProperty<unsigned int>& order() { return m_order; }
		const IProperty<unsigned int>& order() const { return m_order; }

		IAssignable<NiParticleSystem>& target() { return m_target; }
		const IAssignable<NiParticleSystem>& target() const { return m_target; }

		IProperty<bool>& active() { return m_active; }
		const IProperty<bool>& active() const { return m_active; }

	private:
		Property<std::string> m_name;
		Property<unsigned int> m_order;
		Assignable<NiParticleSystem> m_target;
		Property<bool> m_active;
	};

	class NiPSysUpdateCtlr : public NiTimeController
	{
	public:
		NiPSysUpdateCtlr();
		NiPSysUpdateCtlr(native::NiPSysUpdateCtlr* obj);
		NiPSysUpdateCtlr(const NiPSysUpdateCtlr&) = delete;

		virtual ~NiPSysUpdateCtlr() = default;

		NiPSysUpdateCtlr& operator=(const NiPSysUpdateCtlr&) = delete;

		native::NiPSysUpdateCtlr& getNative() const;
	};

	class NiPSysBoundUpdateModifier : public NiPSysModifier
	{
	public:
		NiPSysBoundUpdateModifier();
		NiPSysBoundUpdateModifier(native::NiPSysBoundUpdateModifier* obj);
		NiPSysBoundUpdateModifier(const NiPSysBoundUpdateModifier&) = delete;

		virtual ~NiPSysBoundUpdateModifier() = default;

		NiPSysBoundUpdateModifier& operator=(const NiPSysBoundUpdateModifier&) = delete;

		native::NiPSysBoundUpdateModifier& getNative() const;
	};


	class NiPSysAgeDeathModifier : public NiPSysModifier
	{
	public:
		NiPSysAgeDeathModifier();
		NiPSysAgeDeathModifier(native::NiPSysAgeDeathModifier* obj);
		NiPSysAgeDeathModifier(const NiPSysAgeDeathModifier&) = delete;

		virtual ~NiPSysAgeDeathModifier() = default;

		NiPSysAgeDeathModifier& operator=(const NiPSysAgeDeathModifier&) = delete;

		native::NiPSysAgeDeathModifier& getNative() const;
	};

	class NiPSysGravityModifier : public NiPSysModifier
	{
	public:
		NiPSysGravityModifier();
		NiPSysGravityModifier(native::NiPSysGravityModifier* obj);
		NiPSysGravityModifier(const NiPSysGravityModifier&) = delete;

		virtual ~NiPSysGravityModifier() = default;

		NiPSysGravityModifier& operator=(const NiPSysGravityModifier&) = delete;

		native::NiPSysGravityModifier& getNative() const;

		IAssignable<nif::NiNode>& gravityObject() { return m_gravityObj; }
		IProperty<Floats<3>>& gravityAxis() { return m_gravityAxis; }
		IProperty<float>& decay() { return m_decay; }
		IProperty<float>& strength() { return m_strength; }
		IProperty<ForceType>& forceType() { return m_forceType; }
		IProperty<float>& turbulence() { return m_turbulence; }
		IProperty<float>& turbulenceScale() { return m_turbulenceScale; }
		IProperty<bool>& worldAligned() { return m_worldAligned; }

	private:
		Assignable<nif::NiNode> m_gravityObj;
		Property<Floats<3>, native::float3_t> m_gravityAxis;
		Property<float> m_decay;
		Property<float> m_strength;
		Property<ForceType, native::ForceType> m_forceType;
		Property<float> m_turbulence;
		Property<float> m_turbulenceScale; 
		Property<bool> m_worldAligned;
	};


	class NiPSysPositionModifier : public NiPSysModifier
	{
	public:
		NiPSysPositionModifier();
		NiPSysPositionModifier(native::NiPSysPositionModifier* obj);
		NiPSysPositionModifier(const NiPSysPositionModifier&) = delete;

		virtual ~NiPSysPositionModifier() = default;

		NiPSysPositionModifier& operator=(const NiPSysPositionModifier&) = delete;

		native::NiPSysPositionModifier& getNative() const;
	};

	//Somewhat annoying to have to do this just because we cannot inject this conversion into our current Property template.
	//We should be able to fix that, but it only comes into play in a few places right now.
	template<typename T, typename ObjType>
	class DegRadConverter final : public PropertyBase<T>
	{
	public:
		template<typename ObjType, typename BaseType>
		DegRadConverter(ObjType* obj, T(BaseType::* g)() const, void(BaseType::* s)(T)) :
			m_get{ std::bind(g, obj) },
			m_set{ std::bind(s, obj, std::placeholders::_1) }
		{
			static_assert(std::is_base_of<BaseType, ObjType>::value);
			assert(obj && g && s);
		}

		virtual float get() const override
		{
			assert(m_get);
			return static_cast<math::degrees<T>>(math::radians<T>(m_get())).value;
		}
		virtual void set(const float& f) override
		{
			assert(m_set);
			if (f != get()) {
				m_set(static_cast<math::radians<T>>(math::degrees<T>(f)).value);
				this->notify(f);
			}
		}

	private:
		std::function<T()> m_get;
		std::function<void(const T&)> m_set;
	};

	class NiPSysRotationModifier : public NiPSysModifier
	{
	public:
		NiPSysRotationModifier();
		NiPSysRotationModifier(native::NiPSysRotationModifier* obj);
		NiPSysRotationModifier(const NiPSysRotationModifier&) = delete;

		virtual ~NiPSysRotationModifier() = default;

		NiPSysRotationModifier& operator=(const NiPSysRotationModifier&) = delete;

		native::NiPSysRotationModifier& getNative() const;

		IProperty<float>& speed() { return m_speed; }
		IProperty<float>& speedVar() { return m_speedVar; }
		IProperty<float>& angle() { return m_angle; }
		IProperty<float>& angleVar() { return m_angleVar; }
		IProperty<bool>& randomSign() { return m_randomSign; }

	private:
		DegRadConverter<float, native::NiPSysRotationModifier> m_speed;
		DegRadConverter<float, native::NiPSysRotationModifier> m_speedVar;
		DegRadConverter<float, native::NiPSysRotationModifier> m_angle;
		DegRadConverter<float, native::NiPSysRotationModifier> m_angleVar;
		Property<bool> m_randomSign;
	};

	class BSPSysScaleModifier : public NiPSysModifier
	{
	public:
		BSPSysScaleModifier();
		BSPSysScaleModifier(native::BSPSysScaleModifier* obj);
		BSPSysScaleModifier(const BSPSysScaleModifier&) = delete;

		virtual ~BSPSysScaleModifier() = default;

		BSPSysScaleModifier& operator=(const BSPSysScaleModifier&) = delete;

		native::BSPSysScaleModifier& getNative() const;

		IProperty<std::vector<float>>& scales() { return m_scales; }

	private:
		Property<std::vector<float>> m_scales;
	};

	class BSPSysSimpleColorModifier : public NiPSysModifier
	{
	public:
		BSPSysSimpleColorModifier();
		BSPSysSimpleColorModifier(native::BSPSysSimpleColorModifier* obj);
		BSPSysSimpleColorModifier(const BSPSysSimpleColorModifier&) = delete;

		virtual ~BSPSysSimpleColorModifier() = default;

		BSPSysSimpleColorModifier& operator=(const BSPSysSimpleColorModifier&) = delete;

		native::BSPSysSimpleColorModifier& getNative() const;

		IProperty<float>& alpha2Begin() { return m_fadeInEnd; }
		const IProperty<float>& alpha2Begin() const { return m_fadeInEnd; }
		IProperty<float>& alpha2End() { return m_fadeOutBegin; }
		const IProperty<float>& alpha2End() const { return m_fadeOutBegin; }
		IProperty<float>& rgb1End() { return m_col1End; }
		const IProperty<float>& rgb1End() const { return m_col1End; }
		IProperty<float>& rgb2Begin() { return m_col2Begin; }
		const IProperty<float>& rgb2Begin() const { return m_col2Begin; }
		IProperty<float>& rgb2End() { return m_col2End; }
		const IProperty<float>& rgb2End() const { return m_col2End; }
		IProperty<float>& rgb3Begin() { return m_col3Begin; }
		const IProperty<float>& rgb3Begin() const { return m_col3Begin; }
		IProperty<nif::ColRGBA>& col1() { return m_col1; }
		const IProperty<nif::ColRGBA>& col1() const { return m_col1; }
		IProperty<nif::ColRGBA>& col2() { return m_col2; }
		const IProperty<nif::ColRGBA>& col2() const { return m_col2; }
		IProperty<nif::ColRGBA>& col3() { return m_col3; }
		const IProperty<nif::ColRGBA>& col3() const { return m_col3; }

	private:
		Property<float> m_fadeInEnd;
		Property<float> m_fadeOutBegin;
		Property<float> m_col1End;
		Property<float> m_col2Begin;
		Property<float> m_col2End;
		Property<float> m_col3Begin;
		Property<nif::ColRGBA> m_col1;
		Property<nif::ColRGBA> m_col2;
		Property<nif::ColRGBA> m_col3;
	};
}