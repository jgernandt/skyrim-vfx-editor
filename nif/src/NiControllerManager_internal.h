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
#include "NiControllerManager.h"
#include "NiController_internal.h"
#include "NiExtraData_internal.h"

namespace nif
{
	//NiControllerManager
	template<> struct type_map<Niflib::NiControllerManager> { using type = NiControllerManager; };
	template<> struct type_map<NiControllerManager> { using type = Niflib::NiControllerManager; };
	template<> struct ReadSyncer<NiControllerManager> : VerticalTraverser<NiControllerManager, ReadSyncer>
	{
		bool operator() (NiControllerManager& object, const Niflib::NiControllerManager* native, File& file);
	};
	template<> struct WriteSyncer<NiControllerManager> : VerticalTraverser<NiControllerManager, WriteSyncer>
	{
		bool operator() (const NiControllerManager& object, Niflib::NiControllerManager* native, const File& file);
	};

	//NiControllerSequence
	template<> struct type_map<Niflib::NiControllerSequence> { using type = NiControllerSequence; };
	template<> struct type_map<NiControllerSequence> { using type = Niflib::NiControllerSequence; };
	template<> struct ReadSyncer<NiControllerSequence> : VerticalTraverser<NiControllerSequence, ReadSyncer>
	{
		bool operator() (NiControllerSequence& object, const Niflib::NiControllerSequence* native, File& file);
	};
	template<> struct WriteSyncer<NiControllerSequence> : VerticalTraverser<NiControllerSequence, WriteSyncer>
	{
		bool operator() (const NiControllerSequence& object, Niflib::NiControllerSequence* native, const File& file);
	};

	//NiDefaultAVObjectPalette
	template<> struct type_map<Niflib::NiDefaultAVObjectPalette> { using type = NiDefaultAVObjectPalette; };
	template<> struct type_map<NiDefaultAVObjectPalette> { using type = Niflib::NiDefaultAVObjectPalette; };
	template<> struct ReadSyncer<NiDefaultAVObjectPalette> : VerticalTraverser<NiDefaultAVObjectPalette, ReadSyncer>
	{
		bool operator() (NiDefaultAVObjectPalette& object, const Niflib::NiDefaultAVObjectPalette* native, File& file);
	};
	template<> struct WriteSyncer<NiDefaultAVObjectPalette> : VerticalTraverser<NiDefaultAVObjectPalette, WriteSyncer>
	{
		bool operator() (const NiDefaultAVObjectPalette& object, Niflib::NiDefaultAVObjectPalette* native, const File& file);
	};
}
