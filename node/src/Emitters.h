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
#include "Modifier.h"
#include "NiPSysEmitter.h"

namespace node
{
	class Emitter : 
		public Modifier, 
		public nif::PropertyListener<nif::ColRGBA>
	{
	protected:
		Emitter(nif::File& file, 
			ni_ptr<nif::NiPSysEmitter>&& obj,
			ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
			ni_ptr<nif::NiFloatInterpolator>&& iplr,
			ni_ptr<nif::NiBoolInterpolator>&& vis_iplr);

	public:
		virtual ~Emitter();

		virtual nif::NiPSysEmitter& object() override;
		nif::NiPSysEmitterCtlr& controller();
		//nif::NiFloatInterpolator& brIplr();//our default interpolator
		//nif::NiBoolInterpolator& visIplr();//our default interpolator

		virtual void onSet(const nif::ColRGBA& col) override;

	public:
		constexpr static const char* BIRTH_RATE = "Birth rate";
		constexpr static const char* LIFE_SPAN = "Life span";
		constexpr static const char* SIZE = "Size";
		constexpr static const char* COLOUR = "Colour";
		constexpr static const char* SPEED = "Speed";
		constexpr static const char* AZIMUTH = "Azimuth";
		constexpr static const char* ELEVATION = "Elevation";

	private:
		class BirthRateField;
		class LifeSpanField;
		class SizeField;
		class ColourField;
		class SpeedField;
		class AzimuthField;
		class ElevationField;

		ni_ptr<nif::NiPSysEmitterCtlr> m_ctlr;

		//until we have a field to manage this:
		ni_ptr<nif::NiBoolInterpolator> m_visIplr;
		bool m_colActive{ false };

		std::unique_ptr<Field> m_birthRateField;
		std::unique_ptr<Field> m_lifeSpanField;
		std::unique_ptr<Field> m_sizeField;
		std::unique_ptr<Field> m_colField;
		std::unique_ptr<Field> m_speedField;
		std::unique_ptr<Field> m_azimField;
		std::unique_ptr<Field> m_elevField;
	};

	class VolumeEmitter : public Emitter
	{
	protected:
		VolumeEmitter(nif::File& file, 
			ni_ptr<nif::NiPSysVolumeEmitter>&& obj,
			ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
			ni_ptr<nif::NiFloatInterpolator>&& iplr,
			ni_ptr<nif::NiBoolInterpolator>&& vis_iplr);

	public:
		virtual ~VolumeEmitter() = default;
		virtual nif::NiPSysVolumeEmitter& object() override;

	public:
		constexpr static const char* EMITTER_OBJECT = "Emitter object";

	protected:
		class EmitterObjectField final : public Field
		{
		public:
			EmitterObjectField(const std::string& name, VolumeEmitter& node);

		private:
			Receiver<void> m_rvr;
			Sender<IAssignable<nif::NiNode>> m_sdr;
		};
		class EmitterMetricField final : public Field
		{
		public:
			EmitterMetricField(const std::string& name, VolumeEmitter& node, IProperty<float>& prop);
		};

		std::unique_ptr<Field> m_emitterObjField;
	};

	class BoxEmitter final : public VolumeEmitter
	{
	public:
		BoxEmitter(nif::File& file);

		BoxEmitter(nif::File& file,
			ni_ptr<nif::NiPSysBoxEmitter>&& obj,
			ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr = ni_ptr<nif::NiPSysEmitterCtlr>(),
			ni_ptr<nif::NiFloatInterpolator>&& iplr = ni_ptr<nif::NiFloatInterpolator>(),
			ni_ptr<nif::NiBoolInterpolator>&& vis_iplr = ni_ptr<nif::NiBoolInterpolator>());
		~BoxEmitter();

		virtual nif::NiPSysBoxEmitter& object() override;

	public:
		constexpr static const char* BOX_WIDTH = "Width (X)";
		constexpr static const char* BOX_HEIGHT = "Height (Y)";
		constexpr static const char* BOX_DEPTH = "Depth (Z)";

		constexpr static float WIDTH = 180.0f;
		constexpr static float HEIGHT = 385.0f;

	private:
		std::unique_ptr<Field> m_widthField;
		std::unique_ptr<Field> m_heightField;
		std::unique_ptr<Field> m_depthField;
	};

	class CylinderEmitter final : public VolumeEmitter
	{
	public:
		CylinderEmitter(nif::File& file);
		CylinderEmitter(nif::File& file, 
			ni_ptr<nif::NiPSysCylinderEmitter>&& obj,
			ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr = ni_ptr<nif::NiPSysEmitterCtlr>(),
			ni_ptr<nif::NiFloatInterpolator>&& iplr = ni_ptr<nif::NiFloatInterpolator>(),
			ni_ptr<nif::NiBoolInterpolator>&& vis_iplr = ni_ptr<nif::NiBoolInterpolator>());
		~CylinderEmitter();

	public:
		virtual nif::NiPSysCylinderEmitter& object() override;

	public:
		constexpr static const char* CYL_RADIUS = "Radius (XY)";
		constexpr static const char* CYL_LENGTH = "Length (Z)";

		constexpr static float WIDTH = 180.0f;
		constexpr static float HEIGHT = 365.0f;

	private:
		std::unique_ptr<Field> m_radiusField;
		std::unique_ptr<Field> m_lengthField;
	};

	class SphereEmitter final : public VolumeEmitter
	{
	public:
		SphereEmitter(nif::File& file);
		SphereEmitter(nif::File& file,
			ni_ptr<nif::NiPSysSphereEmitter>&& obj,
			ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr = ni_ptr<nif::NiPSysEmitterCtlr>(),
			ni_ptr<nif::NiFloatInterpolator>&& iplr = ni_ptr<nif::NiFloatInterpolator>(),
			ni_ptr<nif::NiBoolInterpolator>&& vis_iplr = ni_ptr<nif::NiBoolInterpolator>());
		~SphereEmitter();

	public:
		virtual nif::NiPSysSphereEmitter& object() override;

	public:
		constexpr static const char* SPH_RADIUS = "Radius";

		constexpr static float WIDTH = 180.0f;
		constexpr static float HEIGHT = 345.0f;

	private:
		std::unique_ptr<Field> m_radiusField;
	};
}