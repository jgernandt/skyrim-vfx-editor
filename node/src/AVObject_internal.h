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
#include "AVObject.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	template<>
	class Connector<NiObjectNET> : public VerticalTraverser<NiObjectNET, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiObjectNET& obj, C& ctor)
		{
			//Register a connection with each extra data and move on
			for (auto&& data : obj.extraData)
				ctor.addConnection(ConnectionInfo{ &obj, data.get(), ObjectNET::EXTRA_DATA, ExtraData::TARGET });

			//(do not connect to controllers, let them do it)

			return true;
		}
	};

	//No Factory specialisation

	template<>
	class Forwarder<NiObjectNET> : public VerticalTraverser<NiObjectNET, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiObjectNET& obj, C& ctor)
		{
			//Forward to extra data, then controllers
			for (auto&& data : obj.extraData) {
				assert(data);
				ctor.pushObject(data);
				data->receive(ctor);
				ctor.popObject();
			}

			for (auto&& ctlr : obj.controllers) {
				assert(ctlr);
				ctor.pushObject(ctlr);
				ctlr->receive(ctor);
				ctor.popObject();
			}

			return true;
		}
	};


	template<>
	class Default<AVObject>
	{
	public:
		void setDefaults(NiAVObject& obj)
		{
			obj.flags.raise(14);
		}
	};

	//No Connector specialisation

	template<>
	class Factory<NiAVObject> : public VerticalTraverser<NiAVObject, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiAVObject& obj, C& ctor)
		{
			if (auto&& ptr = ctor.getObject(); ptr.get() == &obj) {
				auto node = std::make_unique<DummyAVObject>(std::static_pointer_cast<NiAVObject>(ptr));
				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation
}
