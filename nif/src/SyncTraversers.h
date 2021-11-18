#pragma once
#include "Traverser.h"

namespace nif
{
	class File;

	class NiReadSyncer final : public NiTraverser
	{
	public:
		NiReadSyncer(File& file) : m_file{ file } {}
		virtual ~NiReadSyncer() = default;
		virtual void traverse(NiObjectNET& obj) override;
		virtual void traverse(NiAVObject& obj) override;

	private:
		File& m_file;
	};

	class NiWriteSyncer final : public NiTraverser
	{
	public:
		NiWriteSyncer(const File& file) : m_file{ file } {}
		virtual ~NiWriteSyncer() = default;
		virtual void traverse(NiObjectNET& obj) override;
		virtual void traverse(NiAVObject& obj) override;

	private:
		const File& m_file;
	};
}
