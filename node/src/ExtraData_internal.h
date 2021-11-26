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
}