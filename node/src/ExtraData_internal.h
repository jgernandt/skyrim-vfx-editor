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
#include "ExtraData.h"
#include "Constructor.h"
#include "AVObject_internal.h"

namespace node
{
	using namespace nif;

	//ExtraData/////

	//No Default specialisation
	//No Connector specialisation
	
	template<>
	class Factory<NiExtraData> : public VerticalTraverser<NiExtraData, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiExtraData& obj, C& ctor)
		{
			if (auto&& ptr = ctor.getObject(); ptr.get() == &obj) {
				auto node = std::make_unique<DummyExtraData>(std::static_pointer_cast<NiExtraData>(ptr));
				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};
	
	//No Forwarder specialisation


	//StringExtraData/////

	//No Default specialisation
	//No Connector specialisation

	template<>
	class Factory<NiStringExtraData> : public VerticalTraverser<NiStringExtraData, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiStringExtraData& obj, C& ctor)
		{
			if (ni_ptr<NiStringExtraData> ptr = std::static_pointer_cast<NiStringExtraData>(ctor.getObject()); ptr.get() == &obj) {
				if (ptr->name.get() == "Prn")
					ctor.addNode(&obj, std::make_unique<WeaponTypeData>(ptr));
				else
					ctor.addNode(&obj, std::make_unique<StringData>(ptr));
			}
			return false;
		}
	};

	//No Forwarder specialisation


	//StringsExtraData/////

	//No Default specialisation
	//No Connector specialisation

	template<>
	class Factory<NiStringsExtraData> : public VerticalTraverser<NiStringsExtraData, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiStringsExtraData& obj, C& ctor)
		{
			if (obj.name.get() == "AttachT") {
				if (obj.strings.size() > 0 && obj.strings.at(0).get() != "MultiTechnique") {
					if (ni_ptr<NiStringsExtraData> ptr = std::static_pointer_cast<NiStringsExtraData>(ctor.getObject()); ptr.get() == &obj)
						ctor.addNode(&obj, Default<AttachPointData>{}.create(ctor.getFile(), ptr));
				}
				return false;
			}
			else
				//fall through to default extra data
				return true;
		}
	};

	//No Forwarder specialisation
}