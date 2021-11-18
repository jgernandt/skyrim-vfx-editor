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
	struct NiNode;
	struct NiParticleSystem;

	struct NiPSysModifierCtlr : NiTraversable<NiPSysModifierCtlr, NiSingleInterpController>
	{
		Property<std::string> modifierName;
	};
	template<> struct type_map<Niflib::NiPSysModifierCtlr> { using type = NiPSysModifierCtlr; };
	template<> struct type_map<NiPSysModifierCtlr> { using type = Niflib::NiPSysModifierCtlr; };

	template<> class NiSyncer<NiPSysModifierCtlr> : public NiSyncer<NiSingleInterpController>
	{
	public:
		void syncRead(File& file, NiPSysModifierCtlr* object, Niflib::NiPSysModifierCtlr* native);
		void syncWrite(const File& file, NiPSysModifierCtlr* object, Niflib::NiPSysModifierCtlr* native);
	};

	struct NiPSysUpdateCtlr : NiTraversable<NiPSysUpdateCtlr, NiTimeController> {};
	template<> struct type_map<Niflib::NiPSysUpdateCtlr> { using type = NiPSysUpdateCtlr; };
	template<> struct type_map<NiPSysUpdateCtlr> { using type = Niflib::NiPSysUpdateCtlr; };
	template<> class NiSyncer<NiPSysUpdateCtlr> : public NiSyncer<NiTimeController> {};

	struct NiPSysModifier : NiTraversable<NiPSysModifier, NiObject>
	{
		Property<std::string> name;
		Property<unsigned int> order;
		Assignable<NiParticleSystem> target;
		Property<bool> active;
	};
	template<> struct type_map<Niflib::NiPSysModifier> { using type = NiPSysModifier; };
	template<> struct type_map<NiPSysModifier> { using type = Niflib::NiPSysModifier; };

	template<> class NiSyncer<NiPSysModifier> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiPSysModifier* object, Niflib::NiPSysModifier* native);
		void syncWrite(const File& file, NiPSysModifier* object, Niflib::NiPSysModifier* native);
	};

	struct NiPSysBoundUpdateModifier : NiTraversable<NiPSysBoundUpdateModifier, NiPSysModifier> {};
	template<> struct type_map<Niflib::NiPSysBoundUpdateModifier> { using type = NiPSysBoundUpdateModifier; };
	template<> struct type_map<NiPSysBoundUpdateModifier> { using type = Niflib::NiPSysBoundUpdateModifier; };
	template<> class NiSyncer<NiPSysBoundUpdateModifier> : public NiSyncer<NiPSysModifier> {};

	struct NiPSysAgeDeathModifier : NiTraversable<NiPSysAgeDeathModifier, NiPSysModifier> {};
	template<> struct type_map<Niflib::NiPSysAgeDeathModifier> { using type = NiPSysAgeDeathModifier; };
	template<> struct type_map<NiPSysAgeDeathModifier> { using type = Niflib::NiPSysAgeDeathModifier; };
	template<> class NiSyncer<NiPSysAgeDeathModifier> : public NiSyncer<NiPSysModifier> {};

	struct NiPSysPositionModifier : NiTraversable<NiPSysPositionModifier, NiPSysModifier> {};
	template<> struct type_map<Niflib::NiPSysPositionModifier> { using type = NiPSysPositionModifier; };
	template<> struct type_map<NiPSysPositionModifier> { using type = Niflib::NiPSysPositionModifier; };
	template<> class NiSyncer<NiPSysPositionModifier> : public NiSyncer<NiPSysModifier> {};

	struct NiPSysGravityModifier : NiTraversable<NiPSysGravityModifier, NiPSysModifier>
	{
		Assignable<NiNode> gravityObject;
		Property<Floats<3>> gravityAxis;
		Property<float> decay;
		Property<float> strength;
		Property<ForceType> forceType;
		Property<float> turbulence;
		Property<float> turbulenceScale;
		Property<bool> worldAligned;
	};
	template<> struct type_map<Niflib::NiPSysGravityModifier> { using type = NiPSysGravityModifier; };
	template<> struct type_map<NiPSysGravityModifier> { using type = Niflib::NiPSysGravityModifier; };

	template<> class NiSyncer<NiPSysGravityModifier> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiPSysGravityModifier* object, Niflib::NiPSysGravityModifier* native);
		void syncWrite(const File& file, NiPSysGravityModifier* object, Niflib::NiPSysGravityModifier* native);
	};

	struct NiPSysRotationModifier : NiTraversable<NiPSysRotationModifier, NiPSysModifier>
	{
		Property<float> speed;
		Property<float> speedVar;
		Property<float> angle;
		Property<float> angleVar;
		Property<bool> randomSign;
	};
	template<> struct type_map<Niflib::NiPSysRotationModifier> { using type = NiPSysRotationModifier; };
	template<> struct type_map<NiPSysRotationModifier> { using type = Niflib::NiPSysRotationModifier; };

	template<> class NiSyncer<NiPSysRotationModifier> : public NiSyncer<NiPSysModifier>
	{
	public:
		void syncRead(File& file, NiPSysRotationModifier* object, Niflib::NiPSysRotationModifier* native);
		void syncWrite(const File& file, NiPSysRotationModifier* object, Niflib::NiPSysRotationModifier* native);
	};

	struct BSPSysScaleModifier : NiTraversable<BSPSysScaleModifier, NiPSysModifier>
	{
		Property<std::vector<float>> scales;
	};
	template<> struct type_map<Niflib::BSPSysScaleModifier> { using type = BSPSysScaleModifier; };
	template<> struct type_map<BSPSysScaleModifier> { using type = Niflib::BSPSysScaleModifier; };

	template<> class NiSyncer<BSPSysScaleModifier> : public NiSyncer<NiPSysModifier>
	{
	public:
		void syncRead(File& file, BSPSysScaleModifier* object, Niflib::BSPSysScaleModifier* native);
		void syncWrite(const File& file, BSPSysScaleModifier* object, Niflib::BSPSysScaleModifier* native);
	};

	struct BSPSysSimpleColorModifier : NiTraversable<BSPSysSimpleColorModifier, NiPSysModifier>
	{
		struct
		{
			Property<nif::ColRGBA> col;
			Property<float> RGB_end;
		} col1;
		struct
		{
			Property<nif::ColRGBA> col;
			Property<float> RGB_begin;
			Property<float> RGB_end;
			Property<float> alpha_begin;
			Property<float> alpha_end;
		} col2;
		struct
		{
			Property<nif::ColRGBA> col;
			Property<float> RGB_begin;
		} col3;

		//Property<float> alpha2Begin;
		//Property<float> alpha2End;
		//Property<float> rgb1End;
		//Property<float> rgb2Begin;
		//Property<float> rgb2End;
		//Property<float> rgb3Begin;
		//Property<nif::ColRGBA> col1;
		//Property<nif::ColRGBA> col2;
		//Property<nif::ColRGBA> col3;
	};
	template<> struct type_map<Niflib::BSPSysSimpleColorModifier> { using type = BSPSysSimpleColorModifier; };
	template<> struct type_map<BSPSysSimpleColorModifier> { using type = Niflib::BSPSysSimpleColorModifier; };

	template<> class NiSyncer<BSPSysSimpleColorModifier> : public NiSyncer<NiPSysModifier>
	{
	public:
		void syncRead(File& file, BSPSysSimpleColorModifier* object, Niflib::BSPSysSimpleColorModifier* native);
		void syncWrite(const File& file, BSPSysSimpleColorModifier* object, Niflib::BSPSysSimpleColorModifier* native);
	};
}
