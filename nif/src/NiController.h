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
#include "NiObjectNET.h"

namespace nif
{
	template<typename T>
	struct Key
	{
		Property<float> time;
		Property<T> value;
		Property<T> fwdTan;
		Property<T> bwdTan;
		Property<float> tension;
		Property<float> bias;
		Property<float> continuity;
	};

	//Animation data
	struct NiBoolData : NiObject
	{
		Property<KeyType> keyType;
		Vector<Key<bool>> keys;
	};
	template<> struct type_map<Niflib::NiBoolData> { using type = NiBoolData; };
	template<> struct type_map<NiBoolData> { using type = Niflib::NiBoolData; };

	template<> class NiSyncer<NiBoolData> : public SyncerInherit<NiBoolData, NiObject>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiBoolData* object, Niflib::NiBoolData* native) const;
		void syncWriteImpl(File& file, NiBoolData* object, Niflib::NiBoolData* native) const;
	};

	struct NiFloatData : NiObject
	{
		Property<KeyType> keyType;
		Vector<Key<float>> keys;
	};
	template<> struct type_map<Niflib::NiFloatData> { using type = NiFloatData; };
	template<> struct type_map<NiFloatData> { using type = Niflib::NiFloatData; };

	template<> class NiSyncer<NiFloatData> : public SyncerInherit<NiFloatData, NiObject>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiFloatData* object, Niflib::NiFloatData* native) const;
		void syncWriteImpl(File& file, NiFloatData* object, Niflib::NiFloatData* native) const;
	};

	//Interpolators
	struct NiInterpolator : NiObject {};
	template<> struct type_map<Niflib::NiInterpolator> { using type = NiInterpolator; };
	template<> struct type_map<NiInterpolator> { using type = Niflib::NiInterpolator; };
	template<> class NiSyncer<NiInterpolator> : public NiSyncer<NiObject> { public: virtual ~NiSyncer() = default; };

	struct NiBoolInterpolator : NiInterpolator
	{
		Property<bool> value;
		Assignable<NiBoolData> data;
	};
	template<> struct type_map<Niflib::NiBoolInterpolator> { using type = NiBoolInterpolator; };
	template<> struct type_map<NiBoolInterpolator> { using type = Niflib::NiBoolInterpolator; };

	template<> class NiSyncer<NiBoolInterpolator> : public SyncerInherit<NiBoolInterpolator, NiInterpolator>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiBoolInterpolator* object, Niflib::NiBoolInterpolator* native) const;
		void syncWriteImpl(File& file, NiBoolInterpolator* object, Niflib::NiBoolInterpolator* native) const;
	};

	struct NiFloatInterpolator : NiInterpolator
	{
		Property<float> value;
		Assignable<NiFloatData> data;
	};
	template<> struct type_map<Niflib::NiFloatInterpolator> { using type = NiFloatInterpolator; };
	template<> struct type_map<NiFloatInterpolator> { using type = Niflib::NiFloatInterpolator; };

	template<> class NiSyncer<NiFloatInterpolator> : public SyncerInherit<NiFloatInterpolator, NiInterpolator>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiFloatInterpolator* object, Niflib::NiFloatInterpolator* native) const;
		void syncWriteImpl(File& file, NiFloatInterpolator* object, Niflib::NiFloatInterpolator* native) const;
	};

	struct NiBlendInterpolator : NiInterpolator {};
	template<> struct type_map<Niflib::NiBlendInterpolator> { using type = NiBlendInterpolator; };
	template<> struct type_map<NiBlendInterpolator> { using type = Niflib::NiBlendInterpolator; };
	template<> class NiSyncer<NiBlendInterpolator> : public NiSyncer<NiInterpolator> { public: virtual ~NiSyncer() = default; };

	struct NiBlendBoolInterpolator : NiBlendInterpolator {};
	template<> struct type_map<Niflib::NiBlendBoolInterpolator> { using type = NiBlendBoolInterpolator; };
	template<> struct type_map<NiBlendBoolInterpolator> { using type = Niflib::NiBlendBoolInterpolator; };
	template<> class NiSyncer<NiBlendBoolInterpolator> : public NiSyncer<NiBlendInterpolator> { public: virtual ~NiSyncer() = default; };

	struct NiBlendFloatInterpolator : NiBlendInterpolator {};
	template<> struct type_map<Niflib::NiBlendFloatInterpolator> { using type = NiBlendFloatInterpolator; };
	template<> struct type_map<NiBlendFloatInterpolator> { using type = Niflib::NiBlendFloatInterpolator; };
	template<> class NiSyncer<NiBlendFloatInterpolator> : public NiSyncer<NiBlendInterpolator> { public: virtual ~NiSyncer() = default; };
	
	//Controllers
	struct NiTimeController : NiObject
	{
		//disallow access to these?
		//Assignable<NiTimeController>& nextCtlr;
		//Assignable<NiObjectNET>& target;

		Property<unsigned short> flags;
		Property<float> frequency;
		Property<float> phase;
		Property<float> startTime;
		Property<float> stopTime;
	};
	template<> struct type_map<Niflib::NiTimeController> { using type = NiTimeController; };
	template<> struct type_map<NiTimeController> { using type = Niflib::NiTimeController; };

	template<> class NiSyncer<NiTimeController> : public SyncerInherit<NiTimeController, NiObject>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiTimeController* object, Niflib::NiTimeController* native) const;
		void syncWriteImpl(File& file, NiTimeController* object, Niflib::NiTimeController* native) const;
	};

	struct NiSingleInterpController : NiTimeController
	{
		Assignable<NiInterpolator> interpolator;
	};
	template<> struct type_map<Niflib::NiSingleInterpController> { using type = NiSingleInterpController; };
	template<> struct type_map<NiSingleInterpController> { using type = Niflib::NiSingleInterpController; };

	template<> class NiSyncer<NiSingleInterpController> : public SyncerInherit<NiSingleInterpController, NiTimeController>
	{
	public:
		virtual ~NiSyncer() = default;
		void syncReadImpl(File& file, NiSingleInterpController* object, Niflib::NiSingleInterpController* native) const;
		void syncWriteImpl(File& file, NiSingleInterpController* object, Niflib::NiSingleInterpController* native) const;
	};
}