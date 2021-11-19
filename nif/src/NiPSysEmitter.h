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

	struct NiPSysEmitterCtlr : NiTraversable<NiPSysEmitterCtlr, NiPSysModifierCtlr>
	{
		Assignable<NiInterpolator> visIplr;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysEmitterCtlr> { using type = NiPSysEmitterCtlr; };
	template<> struct type_map<NiPSysEmitterCtlr> { using type = Niflib::NiPSysEmitterCtlr; };


	struct NiPSysEmitter : NiTraversable<NiPSysEmitter, NiPSysModifier>
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

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysEmitter> { using type = NiPSysEmitter; };
	template<> struct type_map<NiPSysEmitter> { using type = Niflib::NiPSysEmitter; };


	struct NiPSysVolumeEmitter : NiTraversable<NiPSysVolumeEmitter, NiPSysEmitter>
	{
		Assignable<NiNode> emitterObject;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysVolumeEmitter> { using type = NiPSysVolumeEmitter; };
	template<> struct type_map<NiPSysVolumeEmitter> { using type = Niflib::NiPSysVolumeEmitter; };


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

	
	struct NiPSysCylinderEmitter : NiTraversable<NiPSysCylinderEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;
		Property<float> length;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysCylinderEmitter> { using type = NiPSysCylinderEmitter; };
	template<> struct type_map<NiPSysCylinderEmitter> { using type = Niflib::NiPSysCylinderEmitter; };


	struct NiPSysSphereEmitter : NiTraversable<NiPSysSphereEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiPSysSphereEmitter> { using type = NiPSysSphereEmitter; };
	template<> struct type_map<NiPSysSphereEmitter> { using type = Niflib::NiPSysSphereEmitter; };

}
