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
		using native_type = native::NiPSysModifierCtlr;

	protected:
		friend class File;
		NiPSysModifierCtlr(native_type* obj);

	public:
		virtual ~NiPSysModifierCtlr() = default;

		native_type& getNative() const;

		Property<std::string>& modifierName() { return m_modName; }

	private:
		PropertyFcn<std::string, NiPSysModifierCtlr> m_modName;
	};

	class NiPSysModifier : public NiObject
	{
	public:
		using native_type = native::NiPSysModifier;

	protected:
		friend class File;
		NiPSysModifier(native_type* obj);

	public:
		virtual ~NiPSysModifier() = default;

		native_type& getNative() const;

		Property			<std::string>&		name()			{ return m_name; }
		const Property		<std::string>&		name() const	{ return m_name; }
		
		Property			<unsigned int>&		order()			{ return m_order; }
		const Property		<unsigned int>&		order() const	{ return m_order; }

		Assignable			<NiParticleSystem>& target()		{ return m_target; }
		const Assignable	<NiParticleSystem>& target() const	{ return m_target; }

		Property			<bool>&				active()		{ return m_active; }
		const Property		<bool>&				active() const	{ return m_active; }

	private:
		PropertyFcn<std::string, NiPSysModifier> m_name;
		PropertyFcn<unsigned int, NiPSysModifier> m_order;
		AssignableFcn<NiParticleSystem, NiPSysModifier> m_target;
		PropertyFcn<bool, NiPSysModifier> m_active;
	};

	class NiPSysUpdateCtlr : public NiTimeController
	{
	public:
		using native_type = native::NiPSysUpdateCtlr;

	protected:
		friend class File;
		NiPSysUpdateCtlr();
		NiPSysUpdateCtlr(native_type* obj);

	public:
		virtual ~NiPSysUpdateCtlr() = default;

		native_type& getNative() const;
	};

	class NiPSysBoundUpdateModifier : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysBoundUpdateModifier;

	protected:
		friend class File;
		NiPSysBoundUpdateModifier();
		NiPSysBoundUpdateModifier(native_type* obj);

	public:
		virtual ~NiPSysBoundUpdateModifier() = default;

		native_type& getNative() const;
	};


	class NiPSysAgeDeathModifier : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysAgeDeathModifier;

	protected:
		friend class File;
		NiPSysAgeDeathModifier();
		NiPSysAgeDeathModifier(native_type* obj);

	public:
		virtual ~NiPSysAgeDeathModifier() = default;

		native_type& getNative() const;
	};

	class NiPSysGravityModifier : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysGravityModifier;

	protected:
		friend class File;
		NiPSysGravityModifier();
		NiPSysGravityModifier(native_type* obj);

	public:
		virtual ~NiPSysGravityModifier() = default;

		native_type& getNative() const;

		Assignable		<NiNode>&		gravityObject()		{ return m_gravityObj; }
		Property		<Floats<3>>&	gravityAxis()		{ return m_gravityAxis; }
		Property		<float>&		decay()				{ return m_decay; }
		Property		<float>&		strength()			{ return m_strength; }
		Property		<ForceType>&	forceType()			{ return m_forceType; }
		Property		<float>&		turbulence()		{ return m_turbulence; }
		Property		<float>&		turbulenceScale()	{ return m_turbulenceScale; }
		Property		<bool>&			worldAligned()		{ return m_worldAligned; }

	private:
		AssignableFcn<NiNode, NiPSysGravityModifier> m_gravityObj;
		PropertyFcn<Floats<3>, NiPSysGravityModifier, native::float3_t> m_gravityAxis;
		PropertyFcn<float, NiPSysGravityModifier> m_decay;
		PropertyFcn<float, NiPSysGravityModifier> m_strength;
		PropertyFcn<ForceType, NiPSysGravityModifier, native::ForceType> m_forceType;
		PropertyFcn<float, NiPSysGravityModifier> m_turbulence;
		PropertyFcn<float, NiPSysGravityModifier> m_turbulenceScale;
		PropertyFcn<bool, NiPSysGravityModifier> m_worldAligned;
	};


	class NiPSysPositionModifier : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysPositionModifier;

	protected:
		friend class File;
		NiPSysPositionModifier();
		NiPSysPositionModifier(native_type* obj);

	public:
		virtual ~NiPSysPositionModifier() = default;

		native_type& getNative() const;
	};

	//Somewhat annoying to have to do this just because we cannot inject this conversion into our current Property template.
	//We should be able to fix that, but it only comes into play in a few places right now.
	//Edit: This fails the get/set tests in some release builds. Can't see an obvious reason. Ignore this design for now.
	/*template<typename T, typename ObjType>
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
	};*/

	class NiPSysRotationModifier : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysRotationModifier;

	protected:
		friend class File;
		NiPSysRotationModifier();
		NiPSysRotationModifier(native_type* obj);

	public:
		virtual ~NiPSysRotationModifier() = default;

		native_type& getNative() const;

		Property<float>& speed() { return m_speed; }
		Property<float>& speedVar() { return m_speedVar; }
		Property<float>& angle() { return m_angle; }
		Property<float>& angleVar() { return m_angleVar; }
		Property<bool>& randomSign() { return m_randomSign; }

	private:
		struct Speed : PropertyBase<float, NiPSysRotationModifier>
		{
			Speed(NiPSysRotationModifier& block) : 
				PropertyBase<float, NiPSysRotationModifier>(block) {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		};
		struct SpeedVar : PropertyBase<float, NiPSysRotationModifier>
		{
			SpeedVar(NiPSysRotationModifier& block) : 
				PropertyBase<float, NiPSysRotationModifier>(block) {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		};
		struct Angle : PropertyBase<float, NiPSysRotationModifier>
		{
			Angle(NiPSysRotationModifier& block) : 
				PropertyBase<float, NiPSysRotationModifier>(block) {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		};
		struct AngleVar : PropertyBase<float, NiPSysRotationModifier>
		{
			AngleVar(NiPSysRotationModifier& block) : 
				PropertyBase<float, NiPSysRotationModifier>(block) {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		};

		Speed m_speed;
		SpeedVar m_speedVar;
		Angle m_angle;
		AngleVar m_angleVar;

		//DegRadConverter<float, native::NiPSysRotationModifier> m_speed;
		//DegRadConverter<float, native::NiPSysRotationModifier> m_speedVar;
		//DegRadConverter<float, native::NiPSysRotationModifier> m_angle;
		//DegRadConverter<float, native::NiPSysRotationModifier> m_angleVar;
		PropertyFcn<bool, NiPSysRotationModifier> m_randomSign;
	};

	class BSPSysScaleModifier : public NiPSysModifier
	{
	public:
		using native_type = native::BSPSysScaleModifier;

	protected:
		friend class File;
		BSPSysScaleModifier();
		BSPSysScaleModifier(native_type* obj);

	public:
		virtual ~BSPSysScaleModifier() = default;

		native_type& getNative() const;

		Property<std::vector<float>>& scales() { return m_scales; }

	private:
		PropertyFcn<std::vector<float>, BSPSysScaleModifier> m_scales;
	};

	class BSPSysSimpleColorModifier : public NiPSysModifier
	{
	public:
		using native_type = native::BSPSysSimpleColorModifier;

	protected:
		friend class File;
		BSPSysSimpleColorModifier();
		BSPSysSimpleColorModifier(native_type* obj);

	public:
		virtual ~BSPSysSimpleColorModifier() = default;

		native_type& getNative() const;

		Property<float>& alpha2Begin() { return m_fadeInEnd; }
		const Property<float>& alpha2Begin() const { return m_fadeInEnd; }

		Property<float>& alpha2End() { return m_fadeOutBegin; }
		const Property<float>& alpha2End() const { return m_fadeOutBegin; }

		Property<float>& rgb1End() { return m_col1End; }
		const Property<float>& rgb1End() const { return m_col1End; }

		Property<float>& rgb2Begin() { return m_col2Begin; }
		const Property<float>& rgb2Begin() const { return m_col2Begin; }

		Property<float>& rgb2End() { return m_col2End; }
		const Property<float>& rgb2End() const { return m_col2End; }

		Property<float>& rgb3Begin() { return m_col3Begin; }
		const Property<float>& rgb3Begin() const { return m_col3Begin; }

		Property<nif::ColRGBA>& col1() { return m_col1; }
		const Property<nif::ColRGBA>& col1() const { return m_col1; }

		Property<nif::ColRGBA>& col2() { return m_col2; }
		const Property<nif::ColRGBA>& col2() const { return m_col2; }

		Property<nif::ColRGBA>& col3() { return m_col3; }
		const Property<nif::ColRGBA>& col3() const { return m_col3; }

	private:
		PropertyFcn<float, BSPSysSimpleColorModifier> m_fadeInEnd;
		PropertyFcn<float, BSPSysSimpleColorModifier> m_fadeOutBegin;
		PropertyFcn<float, BSPSysSimpleColorModifier> m_col1End;
		PropertyFcn<float, BSPSysSimpleColorModifier> m_col2Begin;
		PropertyFcn<float, BSPSysSimpleColorModifier> m_col2End;
		PropertyFcn<float, BSPSysSimpleColorModifier> m_col3Begin;
		PropertyFcn<nif::ColRGBA, BSPSysSimpleColorModifier> m_col1;
		PropertyFcn<nif::ColRGBA, BSPSysSimpleColorModifier> m_col2;
		PropertyFcn<nif::ColRGBA, BSPSysSimpleColorModifier> m_col3;
	};
}