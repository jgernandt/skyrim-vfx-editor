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
#include "NiObject.h"

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
	struct NiBoolData : NiTraversable<NiBoolData, NiObject>
	{
		Property<KeyType> keyType;
		List<Key<bool>> keys;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBoolData> { using type = NiBoolData; };
	template<> struct type_map<NiBoolData> { using type = Niflib::NiBoolData; };

	template<> class NiSyncer<NiBoolData> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiBoolData* object, Niflib::NiBoolData* native);
		void syncWrite(const File& file, NiBoolData* object, Niflib::NiBoolData* native);
	};


	struct NiFloatData : NiTraversable<NiFloatData, NiObject>
	{
		Property<KeyType> keyType;
		List<Key<float>> keys;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiFloatData> { using type = NiFloatData; };
	template<> struct type_map<NiFloatData> { using type = Niflib::NiFloatData; };

	template<> class NiSyncer<NiFloatData> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiFloatData* object, Niflib::NiFloatData* native);
		void syncWrite(const File& file, NiFloatData* object, Niflib::NiFloatData* native);
	};


	//Interpolators
	struct NiInterpolator : NiTraversable<NiInterpolator, NiObject> 
	{
		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiInterpolator> { using type = NiInterpolator; };
	template<> struct type_map<NiInterpolator> { using type = Niflib::NiInterpolator; };
	template<> class NiSyncer<NiInterpolator> : public NiSyncer<NiObject> {};


	struct NiBoolInterpolator : NiTraversable<NiBoolInterpolator, NiInterpolator>
	{
		Property<bool> value;
		Assignable<NiBoolData> data;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBoolInterpolator> { using type = NiBoolInterpolator; };
	template<> struct type_map<NiBoolInterpolator> { using type = Niflib::NiBoolInterpolator; };

	template<> class NiSyncer<NiBoolInterpolator> : public NiSyncer<NiInterpolator>
	{
	public:
		void syncRead(File& file, NiBoolInterpolator* object, Niflib::NiBoolInterpolator* native);
		void syncWrite(const File& file, NiBoolInterpolator* object, Niflib::NiBoolInterpolator* native);
	};


	struct NiFloatInterpolator : NiTraversable<NiFloatInterpolator, NiInterpolator>
	{
		Property<float> value;
		Assignable<NiFloatData> data;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiFloatInterpolator> { using type = NiFloatInterpolator; };
	template<> struct type_map<NiFloatInterpolator> { using type = Niflib::NiFloatInterpolator; };

	template<> class NiSyncer<NiFloatInterpolator> : public NiSyncer<NiInterpolator>
	{
	public:
		void syncRead(File& file, NiFloatInterpolator* object, Niflib::NiFloatInterpolator* native);
		void syncWrite(const File& file, NiFloatInterpolator* object, Niflib::NiFloatInterpolator* native);
	};


	struct NiBlendInterpolator : NiTraversable<NiBlendInterpolator, NiInterpolator> 
	{
		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendInterpolator> { using type = NiBlendInterpolator; };
	template<> struct type_map<NiBlendInterpolator> { using type = Niflib::NiBlendInterpolator; };
	template<> class NiSyncer<NiBlendInterpolator> : public NiSyncer<NiInterpolator> {};


	struct NiBlendBoolInterpolator : NiTraversable<NiBlendBoolInterpolator, NiBlendInterpolator> 
	{
		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendBoolInterpolator> { using type = NiBlendBoolInterpolator; };
	template<> struct type_map<NiBlendBoolInterpolator> { using type = Niflib::NiBlendBoolInterpolator; };
	template<> class NiSyncer<NiBlendBoolInterpolator> : public NiSyncer<NiBlendInterpolator> {};


	struct NiBlendFloatInterpolator : NiTraversable<NiBlendFloatInterpolator, NiBlendInterpolator> 
	{
		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendFloatInterpolator> { using type = NiBlendFloatInterpolator; };
	template<> struct type_map<NiBlendFloatInterpolator> { using type = Niflib::NiBlendFloatInterpolator; };
	template<> class NiSyncer<NiBlendFloatInterpolator> : public NiSyncer<NiBlendInterpolator> {};
	

	//Controllers
	struct NiTimeController : NiTraversable<NiTimeController, NiObject>
	{
		//disallow access to these?
		//Assignable<NiTimeController>& nextCtlr;
		//Assignable<NiObjectNET>& target;

		Property<unsigned short> flags;
		Property<float> frequency;
		Property<float> phase;
		Property<float> startTime;
		Property<float> stopTime;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiTimeController> { using type = NiTimeController; };
	template<> struct type_map<NiTimeController> { using type = Niflib::NiTimeController; };

	template<> class NiSyncer<NiTimeController> : public NiSyncer<NiObject>
	{
	public:
		void syncRead(File& file, NiTimeController* object, Niflib::NiTimeController* native);
		void syncWrite(const File& file, NiTimeController* object, Niflib::NiTimeController* native);
	};


	struct NiSingleInterpController : NiTraversable<NiSingleInterpController, NiTimeController>
	{
		Assignable<NiInterpolator> interpolator;

		static const size_t TYPE;
		virtual size_t type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiSingleInterpController> { using type = NiSingleInterpController; };
	template<> struct type_map<NiSingleInterpController> { using type = Niflib::NiSingleInterpController; };

	template<> class NiSyncer<NiSingleInterpController> : public NiSyncer<NiTimeController>
	{
	public:
		void syncRead(File& file, NiSingleInterpController* object, Niflib::NiSingleInterpController* native);
		void syncWrite(const File& file, NiSingleInterpController* object, Niflib::NiSingleInterpController* native);
	};
}