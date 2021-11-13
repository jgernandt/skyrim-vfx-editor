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

		Assignable<NiInterpolator>& visIplr() { return m_visIplr; }

	private:
		AssignableFcn<NiInterpolator, NiPSysEmitterCtlr> m_visIplr;
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

		Property<ColRGBA>& colour() { return m_colour; }

		Property<float>& lifeSpan() { return m_lifeSpan; }
		Property<float>& lifeSpanVar() { return m_lifeSpanVar; }

		Property<float>& size() { return m_size; }
		Property<float>& sizeVar() { return m_sizeVar; }

		Property<float>& speed() { return m_speed; }
		Property<float>& speedVar() { return m_speedVar; }

		Property<float>& azimuth() { return m_azimuth; }
		Property<float>& azimuthVar() { return m_azimuthVar; }

		Property<float>& elevation() { return m_elevation; }
		Property<float>& elevationVar() { return m_elevationVar; }

	private:

		PropertyFcn<ColRGBA, NiPSysEmitter, native::ColRGBA> m_colour;
		PropertyFcn<float, NiPSysEmitter> m_lifeSpan;
		PropertyFcn<float, NiPSysEmitter> m_lifeSpanVar;
		PropertyFcn<float, NiPSysEmitter> m_size;
		PropertyFcn<float, NiPSysEmitter> m_sizeVar;
		PropertyFcn<float, NiPSysEmitter> m_speed;
		PropertyFcn<float, NiPSysEmitter> m_speedVar;

		struct EmitterAzimuth : PropertyBase<float, NiPSysEmitter>
		{
			EmitterAzimuth(NiPSysEmitter& block) : 
				PropertyBase<float, NiPSysEmitter>{ block } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		} m_azimuth;

		struct EmitterAzimuthVar : PropertyBase<float, NiPSysEmitter>
		{
			EmitterAzimuthVar(NiPSysEmitter& block) : 
				PropertyBase<float, NiPSysEmitter>{ block } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		} m_azimuthVar;

		struct EmitterElevation : PropertyBase<float, NiPSysEmitter>
		{
			EmitterElevation(NiPSysEmitter& block) : 
				PropertyBase<float, NiPSysEmitter>{ block } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

		} m_elevation;

		struct EmitterElevationVar : PropertyBase<float, NiPSysEmitter>
		{
			EmitterElevationVar(NiPSysEmitter& block) : 
				PropertyBase<float, NiPSysEmitter>{ block } {}

			virtual float get() const override;
			virtual void set(const float& f) override;

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

		Assignable<NiNode>& emitterObject() { return m_emtrObj; }

	private:
		AssignableFcn<NiNode, NiPSysVolumeEmitter> m_emtrObj;
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

		Property<float>& width() { return m_width; }
		Property<float>& height() { return m_height; }
		Property<float>& depth() { return m_depth; }

	private:
		PropertyFcn<float, NiPSysBoxEmitter> m_width;
		PropertyFcn<float, NiPSysBoxEmitter> m_height;
		PropertyFcn<float, NiPSysBoxEmitter> m_depth;
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

		Property<float>& radius() { return m_radius; }
		Property<float>& height() { return m_height; }

	private:
		PropertyFcn<float, NiPSysCylinderEmitter> m_radius;
		PropertyFcn<float, NiPSysCylinderEmitter> m_height;
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

		Property<float>& radius() { return m_radius; }

	private:
		PropertyFcn<float, NiPSysSphereEmitter> m_radius;
	};
}