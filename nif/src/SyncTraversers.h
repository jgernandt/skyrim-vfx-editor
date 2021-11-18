#pragma once
#include "Traverser.h"

namespace nif
{
	class File;

	class NiReadSyncer : public NiTraverser
	{
	public:
		NiReadSyncer(File& file) : m_file{ file } {}
		virtual ~NiReadSyncer() = default;
		virtual void traverse(NiObjectNET& obj);

	private:
		File& m_file;
	};

	class NiWriteSyncer : public NiTraverser
	{
	public:
		NiWriteSyncer(const File& file) : m_file{ file } {}
		virtual ~NiWriteSyncer() = default;
		virtual void traverse(NiObjectNET& obj);

	private:
		const File& m_file;
	};
}
