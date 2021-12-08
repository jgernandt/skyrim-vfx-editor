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
#include "NiController.h"

namespace nif
{
	struct NiPSysModifier : NiTraversable<NiPSysModifier, NiObject>
	{
		Property<std::string> name;
		Property<unsigned int> order;
		Ptr<NiParticleSystem> target;
		Property<bool> active;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysAgeDeathModifier : NiTraversable<NiPSysAgeDeathModifier, NiPSysModifier>
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysBoundUpdateModifier : NiTraversable<NiPSysBoundUpdateModifier, NiPSysModifier> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysGravityModifier : NiTraversable<NiPSysGravityModifier, NiPSysModifier>
	{
		Ptr<NiNode> gravityObject;
		Property<Floats<3>> gravityAxis;
		Property<float> decay;
		Property<float> strength;
		Property<ForceType> forceType;
		Property<float> turbulence;
		Property<float> turbulenceScale;
		Property<bool> worldAligned;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysPositionModifier : NiTraversable<NiPSysPositionModifier, NiPSysModifier> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct NiPSysRotationModifier : NiTraversable<NiPSysRotationModifier, NiPSysModifier>
	{
		Property<math::degf> speed;
		Property<math::degf> speedVar;
		Property<math::degf> angle;
		Property<math::degf> angleVar;
		Property<bool> randomSign;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct BSPSysScaleModifier : NiTraversable<BSPSysScaleModifier, NiPSysModifier>
	{
		Property<std::vector<float>> scales;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};

	struct BSPSysSimpleColorModifier : NiTraversable<BSPSysSimpleColorModifier, NiPSysModifier>
	{
		struct
		{
			Property<ColRGBA> value;

			const Property<float> RGBbegin{ 0.0f };
			Property<float> RGBend;

			const Property<float> Abegin{ 0.0f };
			const Property<float> Aend{ 0.0f };
		} col1;
		struct
		{
			Property<ColRGBA> value;

			Property<float> RGBbegin;
			Property<float> RGBend;

			Property<float> Abegin;
			Property<float> Aend;
		} col2;
		struct
		{
			Property<ColRGBA> value;

			Property<float> RGBbegin;
			const Property<float> RGBend{ 1.0f };

			const Property<float> Abegin{ 1.0f };
			const Property<float> Aend{ 1.0f };
		} col3;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
}
