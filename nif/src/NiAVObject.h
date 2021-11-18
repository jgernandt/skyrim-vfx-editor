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
	struct Transform
	{
		Property<translation_t> translation;
		Property<rotation_t> rotation;
		Property<scale_t> scale;
	};

	struct NiAVObject : NiTraversable<NiAVObject, NiObjectNET>
	{
		FlagSet<std::uint_fast32_t> flags;
		Transform transform;
	};
	template<> struct type_map<Niflib::NiAVObject> { using type = NiAVObject; };
	template<> struct type_map<NiAVObject> { using type = Niflib::NiAVObject; };

	template<> class NiSyncer<NiAVObject> : public NiSyncer<NiObjectNET>
	{
	public:
		void syncRead(File& file, NiAVObject* object, Niflib::NiAVObject* native);
		void syncWrite(const File& file, NiAVObject* object, Niflib::NiAVObject* native);
	};
}
