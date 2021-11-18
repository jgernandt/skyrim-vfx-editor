#pragma once
#include "ni_objects.h"

namespace nif
{
	class File;

	class NiTraverser
	{
	public:
		virtual ~NiTraverser() = default;
		virtual void traverse(NiObject&) {}
		virtual void traverse(NiObjectNET&) {}

		virtual void traverse(NiAVObject&) {}
		virtual void traverse(NiNode&) {}

		//etc.
	};

	class NiReadSyncer : public NiTraverser
	{
	public:
		NiReadSyncer(File& file) {}
		virtual ~NiReadSyncer() = default;
		virtual void traverse(NiObject&) {}
	};

	class NiWriteSyncer : public NiTraverser
	{
	public:
		NiWriteSyncer(const File& file) {}
		virtual ~NiWriteSyncer() = default;
		virtual void traverse(NiObject&) {}
	};

	template<typename T, typename Base>
	void nif::NiTraversable<T, Base>::receive(NiTraverser& t)
	{
		t.traverse(static_cast<T&>(*this));
	}

	template<typename T>
	void nif::NiTraversable<T, void>::receive(NiTraverser& t)
	{
		t.traverse(static_cast<T&>(*this));
	}
}