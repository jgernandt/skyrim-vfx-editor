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

	struct NiPSysVolumeEmitter : NiTraversable<NiPSysVolumeEmitter, NiPSysEmitter>
	{
		Ptr<NiNode> emitterObject;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysBoxEmitter : NiTraversable<NiPSysBoxEmitter, NiPSysVolumeEmitter>
	{
		Property<float> width;
		Property<float> height;
		Property<float> depth;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	
	struct NiPSysCylinderEmitter : NiTraversable<NiPSysCylinderEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;
		Property<float> length;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysSphereEmitter : NiTraversable<NiPSysSphereEmitter, NiPSysVolumeEmitter>
	{
		Property<float> radius;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}
