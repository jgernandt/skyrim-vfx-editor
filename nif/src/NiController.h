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

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBoolData> { using type = NiBoolData; };
	template<> struct type_map<NiBoolData> { using type = Niflib::NiBoolData; };
	template<> struct ReadSyncer<NiBoolData> : VerticalTraverser<NiBoolData, ReadSyncer>
	{
		void operator() (NiBoolData& object, const Niflib::NiBoolData* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolData> : VerticalTraverser<NiBoolData, WriteSyncer>
	{
		void operator() (const NiBoolData& object, Niflib::NiBoolData* native, const File& file);
	};


	struct NiFloatData : NiTraversable<NiFloatData, NiObject>
	{
		Property<KeyType> keyType;
		List<Key<float>> keys;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiFloatData> { using type = NiFloatData; };
	template<> struct type_map<NiFloatData> { using type = Niflib::NiFloatData; };
	template<> struct ReadSyncer<NiFloatData> : VerticalTraverser<NiFloatData, ReadSyncer>
	{
		void operator() (NiFloatData& object, const Niflib::NiFloatData* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatData> : VerticalTraverser<NiFloatData, WriteSyncer>
	{
		void operator() (const NiFloatData& object, Niflib::NiFloatData* native, const File& file);
	};


	//Interpolators
	struct NiInterpolator : NiTraversable<NiInterpolator, NiObject> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiInterpolator> { using type = NiInterpolator; };
	template<> struct type_map<NiInterpolator> { using type = Niflib::NiInterpolator; };


	struct NiBoolInterpolator : NiTraversable<NiBoolInterpolator, NiInterpolator>
	{
		Property<bool> value;
		Assignable<NiBoolData> data;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBoolInterpolator> { using type = NiBoolInterpolator; };
	template<> struct type_map<NiBoolInterpolator> { using type = Niflib::NiBoolInterpolator; };
	template<> struct ReadSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, ReadSyncer>
	{
		void operator() (NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, WriteSyncer>
	{
		void operator() (const NiBoolInterpolator& object, Niflib::NiBoolInterpolator* native, const File& file);
	};


	struct NiFloatInterpolator : NiTraversable<NiFloatInterpolator, NiInterpolator>
	{
		Property<float> value;
		Assignable<NiFloatData> data;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiFloatInterpolator> { using type = NiFloatInterpolator; };
	template<> struct type_map<NiFloatInterpolator> { using type = Niflib::NiFloatInterpolator; };
	template<> struct ReadSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, ReadSyncer>
	{
		void operator() (NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file);
	};
	template<> struct WriteSyncer<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, WriteSyncer>
	{
		void operator() (const NiFloatInterpolator& object, Niflib::NiFloatInterpolator* native, const File& file);
	};


	struct NiBlendInterpolator : NiTraversable<NiBlendInterpolator, NiInterpolator> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendInterpolator> { using type = NiBlendInterpolator; };
	template<> struct type_map<NiBlendInterpolator> { using type = Niflib::NiBlendInterpolator; };


	struct NiBlendBoolInterpolator : NiTraversable<NiBlendBoolInterpolator, NiBlendInterpolator> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendBoolInterpolator> { using type = NiBlendBoolInterpolator; };
	template<> struct type_map<NiBlendBoolInterpolator> { using type = Niflib::NiBlendBoolInterpolator; };


	struct NiBlendFloatInterpolator : NiTraversable<NiBlendFloatInterpolator, NiBlendInterpolator> 
	{
		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiBlendFloatInterpolator> { using type = NiBlendFloatInterpolator; };
	template<> struct type_map<NiBlendFloatInterpolator> { using type = Niflib::NiBlendFloatInterpolator; };
	

	//Controllers
	struct NiTimeController : NiTraversable<NiTimeController, NiObject>
	{
		//these should only be readable? Separate type, or just const Assignable?
		//How to keep them updated, though?
		//Readable<NiTimeController>& nextCtlr;
		//Readable<NiObjectNET>& target;

		Property<unsigned short> flags;
		Property<float> frequency;
		Property<float> phase;
		Property<float> startTime;
		Property<float> stopTime;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiTimeController> { using type = NiTimeController; };
	template<> struct type_map<NiTimeController> { using type = Niflib::NiTimeController; };
	template<> struct ReadSyncer<NiTimeController> : VerticalTraverser<NiTimeController, ReadSyncer>
	{
		void operator() (NiTimeController& object, const Niflib::NiTimeController* native, File& file);
	};
	template<> struct WriteSyncer<NiTimeController> : VerticalTraverser<NiTimeController, WriteSyncer>
	{
		void operator() (const NiTimeController& object, Niflib::NiTimeController* native, const File& file);
	};


	struct NiSingleInterpController : NiTraversable<NiSingleInterpController, NiTimeController>
	{
		Assignable<NiInterpolator> interpolator;

		static const ni_type TYPE;
		virtual ni_type type() const override { return TYPE; }
	};
	template<> struct type_map<Niflib::NiSingleInterpController> { using type = NiSingleInterpController; };
	template<> struct type_map<NiSingleInterpController> { using type = Niflib::NiSingleInterpController; };
	template<> struct ReadSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, ReadSyncer>
	{
		void operator() (NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file);
	};
	template<> struct WriteSyncer<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, WriteSyncer>
	{
		void operator() (const NiSingleInterpController& object, Niflib::NiSingleInterpController* native, const File& file);
	};

}