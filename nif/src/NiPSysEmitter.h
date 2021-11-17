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

namespace nif
{
	struct NiNode;

	struct NiPSysEmitterCtlr : NiPSysModifierCtlr
	{
		Assignable<NiInterpolator> visIplr;
	};
	template<> struct type_map<Niflib::NiPSysEmitterCtlr> { using type = NiPSysEmitterCtlr; };
	template<> struct type_map<NiPSysEmitterCtlr> { using type = Niflib::NiPSysEmitterCtlr; };

	template<> class NiSyncer<NiPSysEmitterCtlr> : public SyncerInherit<NiPSysEmitterCtlr, NiPSysModifierCtlr>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysEmitterCtlr* object, Niflib::NiPSysEmitterCtlr* native) const;
		void syncWriteImpl(File& file, NiPSysEmitterCtlr* object, Niflib::NiPSysEmitterCtlr* native) const;
	};

	struct NiPSysEmitter : NiPSysModifier
	{
		Property<ColRGBA> colour;

		Property<float> lifeSpan;
		Property<float> lifeSpanVar;

		Property<float> size;
		Property<float> sizeVar;

		Property<float> speed;
		Property<float> speedVar;

		Property<float> azimuth;
		Property<float> azimuthVar;

		Property<float> elevation;
		Property<float> elevationVar;
	};
	template<> struct type_map<Niflib::NiPSysEmitter> { using type = NiPSysEmitter; };
	template<> struct type_map<NiPSysEmitter> { using type = Niflib::NiPSysEmitter; };

	template<> class NiSyncer<NiPSysEmitter> : public SyncerInherit<NiPSysEmitter, NiPSysModifier>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysEmitter* object, Niflib::NiPSysEmitter* native) const;
		void syncWriteImpl(File& file, NiPSysEmitter* object, Niflib::NiPSysEmitter* native) const;
	};

	struct NiPSysVolumeEmitter : NiPSysEmitter
	{
		Assignable<NiNode> emitterObject;
	};
	template<> struct type_map<Niflib::NiPSysVolumeEmitter> { using type = NiPSysVolumeEmitter; };
	template<> struct type_map<NiPSysVolumeEmitter> { using type = Niflib::NiPSysVolumeEmitter; };

	template<> class NiSyncer<NiPSysVolumeEmitter> : public SyncerInherit<NiPSysVolumeEmitter, NiPSysEmitter>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysVolumeEmitter* object, Niflib::NiPSysVolumeEmitter* native) const;
		void syncWriteImpl(File& file, NiPSysVolumeEmitter* object, Niflib::NiPSysVolumeEmitter* native) const;
	};

	struct NiPSysBoxEmitter : NiPSysVolumeEmitter
	{
		Property<float> width;
		Property<float> height;
		Property<float> depth;
	};
	template<> struct type_map<Niflib::NiPSysBoxEmitter> { using type = NiPSysBoxEmitter; };
	template<> struct type_map<NiPSysBoxEmitter> { using type = Niflib::NiPSysBoxEmitter; };

	template<> class NiSyncer<NiPSysBoxEmitter> : public SyncerInherit<NiPSysBoxEmitter, NiPSysVolumeEmitter>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysBoxEmitter* object, Niflib::NiPSysBoxEmitter* native) const;
		void syncWriteImpl(File& file, NiPSysBoxEmitter* object, Niflib::NiPSysBoxEmitter* native) const;
	};
	
	struct NiPSysCylinderEmitter : NiPSysVolumeEmitter
	{
		Property<float> radius;
		Property<float> length;
	};
	template<> struct type_map<Niflib::NiPSysCylinderEmitter> { using type = NiPSysCylinderEmitter; };
	template<> struct type_map<NiPSysCylinderEmitter> { using type = Niflib::NiPSysCylinderEmitter; };

	template<> class NiSyncer<NiPSysCylinderEmitter> : public SyncerInherit<NiPSysCylinderEmitter, NiPSysVolumeEmitter>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysCylinderEmitter* object, Niflib::NiPSysCylinderEmitter* native) const;
		void syncWriteImpl(File& file, NiPSysCylinderEmitter* object, Niflib::NiPSysCylinderEmitter* native) const;
	};

	struct NiPSysSphereEmitter : NiPSysVolumeEmitter
	{
		Property<float> radius;
	};
	template<> struct type_map<Niflib::NiPSysSphereEmitter> { using type = NiPSysSphereEmitter; };
	template<> struct type_map<NiPSysSphereEmitter> { using type = Niflib::NiPSysSphereEmitter; };

	template<> class NiSyncer<NiPSysSphereEmitter> : public SyncerInherit<NiPSysSphereEmitter, NiPSysVolumeEmitter>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiPSysSphereEmitter* object, Niflib::NiPSysSphereEmitter* native) const;
		void syncWriteImpl(File& file, NiPSysSphereEmitter* object, Niflib::NiPSysSphereEmitter* native) const;
	};
}