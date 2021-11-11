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
		using native_type = native::NiPSysEmitterCtlr;

	protected:
		friend class File;
		NiPSysEmitterCtlr();
		NiPSysEmitterCtlr(native_type* obj);

	public:
		virtual ~NiPSysEmitterCtlr() = default;

		native_type& getNative() const;

		IAssignable<NiInterpolator>& visIplr() { return m_visIplr; }

	private:
		Assignable<NiInterpolator> m_visIplr;
	};

	class NiPSysEmitter : public NiPSysModifier
	{
	public:
		using native_type = native::NiPSysEmitter;

	protected:
		friend class File;
		NiPSysEmitter(native_type* obj);

	public:
		virtual ~NiPSysEmitter() = default;

		native_type& getNative() const;

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
		using native_type = native::NiPSysVolumeEmitter;

	protected:
		friend class File;
		NiPSysVolumeEmitter(native_type* obj);

	public:
		virtual ~NiPSysVolumeEmitter() = default;

		native_type& getNative() const;

		IAssignable<NiNode>& emitterObject() { return m_emtrObj; }

	private:
		Assignable<NiNode> m_emtrObj;
	};

	class NiPSysBoxEmitter : public NiPSysVolumeEmitter
	{
	public:
		using native_type = native::NiPSysBoxEmitter;

	protected:
		friend class File;
		NiPSysBoxEmitter();
		NiPSysBoxEmitter(native_type* obj);

	public:
		virtual ~NiPSysBoxEmitter() = default;

		native_type& getNative() const;

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
		using native_type = native::NiPSysCylinderEmitter;

	protected:
		friend class File;
		NiPSysCylinderEmitter();
		NiPSysCylinderEmitter(native_type* obj);

	public:
		virtual ~NiPSysCylinderEmitter() = default;

		native_type& getNative() const;

		IProperty<float>& radius() { return m_radius; }
		IProperty<float>& height() { return m_height; }

	private:
		Property<float> m_radius;
		Property<float> m_height;
	};

	class NiPSysSphereEmitter : public NiPSysVolumeEmitter
	{
	public:
		using native_type = native::NiPSysSphereEmitter;

	protected:
		friend class File;
		NiPSysSphereEmitter();
		NiPSysSphereEmitter(native_type* obj);

	public:
		virtual ~NiPSysSphereEmitter() = default;

		native_type& getNative() const;

		IProperty<float>& radius() { return m_radius; }

	private:
		Property<float> m_radius;
	};
}