#pragma once
#include "ni_objects.h"

namespace nif
{
	class NiTraverser
	{
	public:
		virtual ~NiTraverser() = default;
		virtual void traverse(NiObject& obj);
		virtual void traverse(NiObjectNET& obj);
		virtual void traverse(NiAVObject& obj);
		virtual void traverse(NiNode& obj);

		//etc.
	};

	template<typename T, typename Base>
	void nif::NiTraversable<T, Base>::receive(NiTraverser& t)
	{
		t.traverse(static_cast<T&>(*this));
	}
}