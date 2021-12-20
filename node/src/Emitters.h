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

namespace node
{
	using namespace nif;

	class Emitter : 
		public Modifier, 
		public PropertyListener<ColRGBA>
	{
	public:
		class BirthRate final :
			public Field,
			public IControllable,
			public AssignableListener<NiInterpolator>
		{
		public:
			BirthRate(const std::string& name, Emitter& node,
				const ni_ptr<NiPSysEmitterCtlr>& ctlr, const ni_ptr<NiFloatInterpolator>& iplr);

			~BirthRate();

			virtual Ref<NiInterpolator>& iplr() override;
			virtual Ref<NiAVObject>& node() override;
			virtual ni_ptr<NiTimeController> ctlr() override;
			virtual std::string propertyType() override { return std::string(); }
			virtual std::string ctlrType() override;
			virtual std::string ctlrID() override { return std::string(); }
			virtual std::string iplrID() override;
			virtual ni_ptr<Property<std::string>> ctlrIDProperty() override;

			virtual void onAssign(NiInterpolator* obj) override;

			constexpr static const char* ID = "Birth rate";

		private:
			Emitter& m_node;
			const ni_ptr<NiPSysEmitterCtlr> m_ctlr;
			const ni_ptr<NiFloatInterpolator> m_iplr;

			FloatCtlrReceiver m_rcvr;
			Sender<IControllable> m_sndr;
		};

	protected:
		Emitter(
			const ni_ptr<NiPSysEmitter>& obj,
			const ni_ptr<NiPSysEmitterCtlr>& ctlr,
			const ni_ptr<NiFloatInterpolator>& iplr,
			const ni_ptr<NiBoolInterpolator>& vis_iplr);

	public:
		virtual ~Emitter();

		virtual void onSet(const nif::ColRGBA& col) override;

		BirthRate& birthRate() { return *m_birthRateField; }

	public:
		constexpr static const char* LIFE_SPAN = "Life span";
		constexpr static const char* SIZE = "Size";
		constexpr static const char* COLOUR = "Colour";
		constexpr static const char* SPEED = "Speed";
		constexpr static const char* AZIMUTH = "Azimuth";
		constexpr static const char* ELEVATION = "Elevation";

		constexpr static float WIDTH = 180.0f;

	private:
		class LifeSpanField;
		class SizeField;
		class ColourField;
		class SpeedField;
		class AzimuthField;
		class ElevationField;

		ni_ptr<NiBoolInterpolator> m_visIplr;

		bool m_colActive{ false };

		std::unique_ptr<BirthRate> m_birthRateField;
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
		VolumeEmitter(
			const ni_ptr<NiPSysVolumeEmitter>& obj,
			const ni_ptr<NiPSysEmitterCtlr>& ctlr,
			const ni_ptr<NiFloatInterpolator>& iplr,
			const ni_ptr<NiBoolInterpolator>& vis_iplr);

	public:
		virtual ~VolumeEmitter() = default;

	public:
		constexpr static const char* EMITTER_OBJECT = "Emitter object";

	protected:
		class EmitterObjectField final : public Field
		{
		public:
			EmitterObjectField(const std::string& name, NodeBase& node, 
				ni_ptr<Ptr<NiNode>>&& emitterObject);

		private:
			Receiver<void> m_rvr;
			Sender<Ptr<NiNode>> m_sdr;
		};
		class EmitterMetricField final : public Field
		{
		public:
			EmitterMetricField(const std::string& name, NodeBase& node, 
				ni_ptr<Property<float>>&& prop);
		};

		std::unique_ptr<Field> m_emitterObjField;
	};

	class BoxEmitter final : public VolumeEmitter
	{
	public:
		BoxEmitter(
			const ni_ptr<NiPSysBoxEmitter>& obj,
			const ni_ptr<NiPSysEmitterCtlr>& ctlr,
			const ni_ptr<NiFloatInterpolator>& iplr,
			const ni_ptr<NiBoolInterpolator>& vis_iplr);
		~BoxEmitter();

	public:
		constexpr static const char* BOX_WIDTH = "Width (X)";
		constexpr static const char* BOX_HEIGHT = "Height (Y)";
		constexpr static const char* BOX_DEPTH = "Depth (Z)";

		constexpr static float HEIGHT = 385.0f;

	private:
		std::unique_ptr<Field> m_widthField;
		std::unique_ptr<Field> m_heightField;
		std::unique_ptr<Field> m_depthField;
	};

	class CylinderEmitter final : public VolumeEmitter
	{
	public:
		CylinderEmitter(
			const ni_ptr<NiPSysCylinderEmitter>& obj,
			const ni_ptr<NiPSysEmitterCtlr>& ctlr,
			const ni_ptr<NiFloatInterpolator>& iplr,
			const ni_ptr<NiBoolInterpolator>& vis_iplr);
		~CylinderEmitter();

	public:
		constexpr static const char* CYL_RADIUS = "Radius (XY)";
		constexpr static const char* CYL_LENGTH = "Length (Z)";

		constexpr static float HEIGHT = 365.0f;

	private:
		std::unique_ptr<Field> m_radiusField;
		std::unique_ptr<Field> m_lengthField;
	};

	class SphereEmitter final : public VolumeEmitter
	{
	public:
		SphereEmitter(
			const ni_ptr<NiPSysSphereEmitter>& obj,
			const ni_ptr<NiPSysEmitterCtlr>& ctlr,
			const ni_ptr<NiFloatInterpolator>& iplr,
			const ni_ptr<NiBoolInterpolator>& vis_iplr);
		~SphereEmitter();

	public:
		constexpr static const char* SPH_RADIUS = "Radius";

		constexpr static float HEIGHT = 345.0f;

	private:
		std::unique_ptr<Field> m_radiusField;
	};
}