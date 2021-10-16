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
#include "NiPSysModifier.h"
#include "NiNode.h"

namespace nif
{
	class NiPSysEmitterCtlr : public NiPSysModifierCtlr
	{
	public:
		NiPSysEmitterCtlr();
		NiPSysEmitterCtlr(native::NiPSysEmitterCtlr* obj);
		NiPSysEmitterCtlr(const NiPSysEmitterCtlr&) = delete;

		virtual ~NiPSysEmitterCtlr() = default;

		NiPSysEmitterCtlr& operator=(const NiPSysEmitterCtlr&) = delete;

		native::NiPSysEmitterCtlr& getNative() const;

		//NiPSysEmitterCtlr
		IAssignable<NiInterpolator>& visIplr() { return m_visIplr; }

	private:
		Assignable<NiInterpolator> m_visIplr;
	};

	class NiPSysEmitter : public NiPSysModifier
	{
	public:
		NiPSysEmitter(native::NiPSysEmitter* obj);
		NiPSysEmitter(const NiPSysEmitter&) = delete;

		virtual ~NiPSysEmitter() = default;

		NiPSysEmitter& operator=(const NiPSysEmitter&) = delete;

		native::NiPSysEmitter& getNative() const;

		IProperty<ColRGBA>& colour() { return m_colour; }

		IProperty<float>& lifeSpan() { return m_lifeSpan; }
		IProperty<float>& lifeSpanVar() { return m_lifeSpanVar; }

		IProperty<float>& size() { return m_size; }
		IProperty<float>& sizeVar() { return m_sizeVar; }

		IProperty<float>& speed() { return m_speed; }
		IProperty<float>& speedVar() { return m_speedVar; }

		IProperty<float>& azimuth() { return m_azimuth; }
		IProperty<float>& azimuthVar() { return m_azimuthVar; }

		IProperty<float>& elevation() { return m_elevation; }
		IProperty<float>& elevationVar() { return m_elevationVar; }

	private:

		Property<ColRGBA, native::ColRGBA> m_colour;
		Property<float> m_lifeSpan;
		Property<float> m_lifeSpanVar;
		Property<float> m_size;
		Property<float> m_sizeVar;
		Property<float> m_speed;
		Property<float> m_speedVar;

		struct EmitterAzimuth : PropertyBase<float>
		{
			EmitterAzimuth(NiPSysEmitter& super) : m_super{ super } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

			NiPSysEmitter& m_super;

		} m_azimuth;

		struct EmitterAzimuthVar : PropertyBase<float>
		{
			EmitterAzimuthVar(NiPSysEmitter& super) : m_super{ super } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

			NiPSysEmitter& m_super;

		} m_azimuthVar;

		struct EmitterElevation : PropertyBase<float>
		{
			EmitterElevation(NiPSysEmitter& super) : m_super{ super } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

			NiPSysEmitter& m_super;

		} m_elevation;

		struct EmitterElevationVar : PropertyBase<float>
		{
			EmitterElevationVar(NiPSysEmitter& super) : m_super{ super } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

			NiPSysEmitter& m_super;

		} m_elevationVar;
	};

	class NiNode;

	class NiPSysVolumeEmitter : public NiPSysEmitter
	{
	public:
		NiPSysVolumeEmitter(native::NiPSysVolumeEmitter* obj);
		NiPSysVolumeEmitter(const NiPSysVolumeEmitter&) = delete;

		virtual ~NiPSysVolumeEmitter() = default;

		NiPSysVolumeEmitter& operator=(const NiPSysVolumeEmitter&) = delete;

		native::NiPSysVolumeEmitter& getNative() const;

		IAssignable<NiNode>& emitterObject() { return m_emtrObj; }

	private:
		Assignable<NiNode> m_emtrObj;
	};

	class NiPSysBoxEmitter : public NiPSysVolumeEmitter
	{
	public:
		NiPSysBoxEmitter();
		NiPSysBoxEmitter(native::NiPSysBoxEmitter* obj);
		NiPSysBoxEmitter(const NiPSysBoxEmitter&) = delete;

		virtual ~NiPSysBoxEmitter() = default;

		NiPSysBoxEmitter& operator=(const NiPSysBoxEmitter&) = delete;

		native::NiPSysBoxEmitter& getNative() const;

		IProperty<float>& width() { return m_width; }
		IProperty<float>& height() { return m_height; }
		IProperty<float>& depth() { return m_depth; }

	private:
		Property<float> m_width;
		Property<float> m_height;
		Property<float> m_depth;
	};
	
	class NiPSysCylinderEmitter : public NiPSysVolumeEmitter
	{
	public:
		NiPSysCylinderEmitter();
		NiPSysCylinderEmitter(native::NiPSysCylinderEmitter* obj);
		NiPSysCylinderEmitter(const NiPSysCylinderEmitter&) = delete;

		virtual ~NiPSysCylinderEmitter() = default;

		NiPSysCylinderEmitter& operator=(const NiPSysCylinderEmitter&) = delete;

		native::NiPSysCylinderEmitter& getNative() const;

		IProperty<float>& radius() { return m_radius; }
		IProperty<float>& height() { return m_height; }

	private:
		Property<float> m_radius;
		Property<float> m_height;
	};

	class NiPSysSphereEmitter : public NiPSysVolumeEmitter
	{
	public:
		NiPSysSphereEmitter();
		NiPSysSphereEmitter(native::NiPSysSphereEmitter* obj);
		NiPSysSphereEmitter(const NiPSysSphereEmitter&) = delete;

		virtual ~NiPSysSphereEmitter() = default;

		NiPSysSphereEmitter& operator=(const NiPSysSphereEmitter&) = delete;

		native::NiPSysSphereEmitter& getNative() const;

		IProperty<float>& radius() { return m_radius; }

	private:
		Property<float> m_radius;
	};
}