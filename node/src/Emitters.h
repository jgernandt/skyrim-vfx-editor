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
#include "Modifier.h"
#include "NiPSysEmitter.h"

namespace node
{
	class Emitter : public Modifier
	{
	protected:
		Emitter(std::unique_ptr<nif::NiPSysEmitter>&& obj,
			std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
			std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
			std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr);

	public:
		virtual ~Emitter();

		virtual nif::NiPSysEmitter& object() override;
		nif::NiPSysEmitterCtlr& controller();
		//nif::NiFloatInterpolator& brIplr();//our default interpolator
		//nif::NiBoolInterpolator& visIplr();//our default interpolator

	public:
		constexpr static const char* BIRTH_RATE = "BirthRate";
		constexpr static const char* LIFE_SPAN = "LifeSpan";
		constexpr static const char* SIZE = "Size";
		constexpr static const char* COLOUR = "Colour";
		constexpr static const char* SPEED = "Speed";
		constexpr static const char* AZIMUTH = "Azimuth";
		constexpr static const char* ELEVATION = "Elevation";

	private:

		//We want to set the colour and lifetime requirement dynamically. Because we can.
		class Device : 
			public Modifier::Device
		{
		public:
			Device(Emitter& node);
			virtual ~Device() = default;

			virtual void onConnect(IModifiable & ifc) override;
			virtual void onDisconnect(IModifiable & ifc) override;

		private:
			class ColourActivator final : public IPropertyListener<nif::ColRGBA>
			{
			public:
				ColourActivator(IProperty<nif::ColRGBA>& prop) : m_prop{ prop } {}

				virtual void onSet(const nif::ColRGBA& f) override;

				void activate(ISet<Requirement>* target);
				void deactivate();

			private:
				IProperty<nif::ColRGBA>& m_prop;
				ISet<Requirement>* m_target{ nullptr };
				bool m_active{ false };
			};

			class LifeMoveActivator final : public IPropertyListener<float>
			{
			public:
				LifeMoveActivator(IProperty<float>& prop, IProperty<float>& var, Requirement req) :
					m_prop{ prop }, m_var{ var }, m_req{ req } {}

				virtual void onSet(const float& f) override;

				void activate(ISet<Requirement>* target);
				void deactivate();

			private:
				IProperty<float>& m_prop;
				IProperty<float>& m_var;
				Requirement m_req;
				ISet<Requirement>* m_target{ nullptr };
				bool m_active{ false };
			};

			ColourActivator m_col;
			LifeMoveActivator m_life;
			LifeMoveActivator m_move;
		};

		class BirthRateField;
		class LifeSpanField;
		class SizeField;
		class ColourField;
		class SpeedField;
		class AzimuthField;
		class ElevationField;

		nif::NiPSysEmitterCtlr* m_ctlr{ nullptr };
		std::unique_ptr<ModifierNameListener> m_modNameLsnr;

		//until we have a field to manage this:
		std::unique_ptr<nif::NiBoolInterpolator> m_visIplr;
	};

	class VolumeEmitter : public Emitter
	{
	protected:
		VolumeEmitter(std::unique_ptr<nif::NiPSysVolumeEmitter>&& obj,
			std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
			std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
			std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr);

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
	};

	class BoxEmitter final : public VolumeEmitter
	{
	public:
		BoxEmitter();

		virtual nif::NiPSysBoxEmitter& object() override;
		BoxEmitter(std::unique_ptr<nif::NiPSysBoxEmitter>&& obj,
			std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr = std::unique_ptr<nif::NiPSysEmitterCtlr>(),
			std::unique_ptr<nif::NiFloatInterpolator>&& iplr = std::unique_ptr<nif::NiFloatInterpolator>(),
			std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr = std::unique_ptr<nif::NiBoolInterpolator>());

	public:
		constexpr static const char* WIDTH = "Width (X)";
		constexpr static const char* HEIGHT = "Height (Y)";
		constexpr static const char* DEPTH = "Depth (Z)";

	};

	class CylinderEmitter final : public VolumeEmitter
	{
	public:
		CylinderEmitter();
		CylinderEmitter(std::unique_ptr<nif::NiPSysCylinderEmitter>&& obj,
			std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr = std::unique_ptr<nif::NiPSysEmitterCtlr>(),
			std::unique_ptr<nif::NiFloatInterpolator>&& iplr = std::unique_ptr<nif::NiFloatInterpolator>(),
			std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr = std::unique_ptr<nif::NiBoolInterpolator>());

	public:
		virtual nif::NiPSysCylinderEmitter& object() override;

	public:
		constexpr static const char* RADIUS = "Radius (XY)";
		constexpr static const char* HEIGHT = "Length (Z)";

	};

	class SphereEmitter final : public VolumeEmitter
	{
	public:
		SphereEmitter();
		SphereEmitter(std::unique_ptr<nif::NiPSysSphereEmitter>&& obj,
			std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr = std::unique_ptr<nif::NiPSysEmitterCtlr>(),
			std::unique_ptr<nif::NiFloatInterpolator>&& iplr = std::unique_ptr<nif::NiFloatInterpolator>(),
			std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr = std::unique_ptr<nif::NiBoolInterpolator>());

	public:
		virtual nif::NiPSysSphereEmitter& object() override;

	public:
		constexpr static const char* RADIUS = "Radius";

	};
}