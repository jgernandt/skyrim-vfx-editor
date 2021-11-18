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
#include "Traversal.h"

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
		virtual void traverse(NiNode& obj) override;
		virtual void traverse(BSFadeNode& obj) override;

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
		virtual void traverse(NiNode& obj) override;
		virtual void traverse(BSFadeNode& obj) override;

	private:
		const File& m_file;
	};
}
