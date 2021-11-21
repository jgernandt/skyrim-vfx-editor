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
	struct NiPSysEmitter : NiTraversable<NiPSysEmitter, NiPSysModifier>
	{
		Property<ColRGBA> colour;

		Property<float> lifeSpan;
		Property<float> lifeSpanVar;

		Property<float> size;
		Property<float> sizeVar;

		Property<float> speed;
		Property<float> speedVar;

		Property<math::degf> azimuth;
		Property<math::degf> azimuthVar;

		Property<math::degf> elevation;
		Property<math::degf> elevationVar;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysEmitter> { using type = NiPSysEmitter; };
	template<> struct type_map<NiPSysEmitter> { using type = Niflib::NiPSysEmitter; };

	template<> struct ReadSyncer<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, ReadSyncer>
	{
		void operator() (NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, WriteSyncer>
	{
		void operator() (const NiPSysEmitter& object, Niflib::NiPSysEmitter* native, const File& file);
	};


	struct NiPSysVolumeEmitter : NiTraversable<NiPSysVolumeEmitter, NiPSysEmitter>
	{
		Assignable<NiNode> emitterObject;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysVolumeEmitter> { using type = NiPSysVolumeEmitter; };
	template<> struct type_map<NiPSysVolumeEmitter> { using type = Niflib::NiPSysVolumeEmitter; };

	template<> struct ReadSyncer<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, ReadSyncer>
	{
		void operator() (NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, WriteSyncer>
	{
		void operator() (const NiPSysVolumeEmitter& object, Niflib::NiPSysVolumeEmitter* native, const File& file);
	};


	struct NiPSysBoxEmitter : NiTraversable<NiPSysBoxEmitter, NiPSysVolumeEmitter>
	{
		Property<float> width;
		Property<float> height;
		Property<float> depth;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysBoxEmitter> { using type = NiPSysBoxEmitter; };
	template<> struct type_map<NiPSysBoxEmitter> { using type = Niflib::NiPSysBoxEmitter; };

	template<> struct ReadSyncer<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, ReadSyncer>
	{
		void operator() (NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, WriteSyncer>
	{
		void operator() (const NiPSysBoxEmitter& object, Niflib::NiPSysBoxEmitter* native, const File& file);
	};

	
	struct NiPSysCylinderEmitter : NiTraversable<NiPSysCylinderEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;
		Property<float> length;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysCylinderEmitter> { using type = NiPSysCylinderEmitter; };
	template<> struct type_map<NiPSysCylinderEmitter> { using type = Niflib::NiPSysCylinderEmitter; };

	template<> struct ReadSyncer<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, ReadSyncer>
	{
		void operator() (NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, WriteSyncer>
	{
		void operator() (const NiPSysCylinderEmitter& object, Niflib::NiPSysCylinderEmitter* native, const File& file);
	};


	struct NiPSysSphereEmitter : NiTraversable<NiPSysSphereEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysSphereEmitter> { using type = NiPSysSphereEmitter; };
	template<> struct type_map<NiPSysSphereEmitter> { using type = Niflib::NiPSysSphereEmitter; };

	template<> struct ReadSyncer<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, ReadSyncer>
	{
		void operator() (NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, WriteSyncer>
	{
		void operator() (const NiPSysSphereEmitter& object, Niflib::NiPSysSphereEmitter* native, const File& file);
	};


	struct NiPSysEmitterCtlr : NiTraversable<NiPSysEmitterCtlr, NiPSysModifierCtlr>
	{
		Assignable<NiInterpolator> visIplr;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysEmitterCtlr> { using type = NiPSysEmitterCtlr; };
	template<> struct type_map<NiPSysEmitterCtlr> { using type = Niflib::NiPSysEmitterCtlr; };

	template<> struct Forwarder<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, Forwarder>
	{
		void operator() (NiPSysEmitterCtlr& object, NiTraverser& traverser);
	};
	template<> struct ReadSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, ReadSyncer>
	{
		void operator() (NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file);
	};
	template<> struct WriteSyncer<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, WriteSyncer>
	{
		void operator() (const NiPSysEmitterCtlr& object, Niflib::NiPSysEmitterCtlr* native, const File& file);
	};
}
