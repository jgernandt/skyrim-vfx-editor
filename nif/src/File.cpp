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

#include "pch.h"
#include "File.h"

#include "NiNode.h"

std::map<size_t, nif::File::CreateFcn>* nif::File::s_typeRegistry;
nif::File::CreateFcn nif::File::pushType(size_t type, CreateFcn fcn)
{
	static bool firstRun = true;
	if (firstRun) {
		s_typeRegistry = new std::map<size_t, nif::File::CreateFcn>;
		firstRun = false;
	}
	return (*s_typeRegistry)[type] = fcn;
}

namespace Niflib
{
	NiObjectRef FindRoot(std::vector<NiObjectRef> const& objects);
}

nif::File::File(Version version) : m_version{ version }
{
	makeRoot(new Niflib::BSFadeNode());
	if (m_rootNode)
		m_rootNode->name.set("NewFile.nif");
}

nif::File::File(const std::filesystem::path& path)
{
	if (!path.empty()) {
		std::ifstream in(path, std::ifstream::binary);
		Niflib::NifInfo fileInfo;
		auto objects = Niflib::ReadNifList(in, &fileInfo);
		in.close();

		if (fileInfo.version == 0x14020007 && fileInfo.userVersion == 12) {
			if (fileInfo.userVersion2 == 83)
				m_version = Version::SKYRIM;
			else if (fileInfo.userVersion2 == 100)
				m_version = Version::SKYRIM_SE;
		}

		makeRoot(Niflib::DynamicCast<Niflib::NiNode>(Niflib::FindRoot(objects)));
	}
}

nif::File::~File()
{
}

void nif::File::makeRoot(const Niflib::Ref<Niflib::NiNode>& node)
{
	if (node) {
		auto block = make_ni<NiNode>(node);

		assert(block);//make_ni should throw on allocation or ctor failure

		if (auto res = m_objectIndex.insert({ block->object, block }); !res.second) {
			//unexpected: the object has already been created
			throw std::runtime_error("Unexpected recreation of object");
		}
		if (auto res = m_nativeIndex.insert({ block->native, block }); !res.second) {
			//same here
			throw std::runtime_error("Unexpected recreation of object");
		}

		//make_ni<T> must guarantee that the created object is actually of type T (or derived).
		//(I don't like how these assumptions keep spilling out of the functions that make them)
		m_rootNode = std::shared_ptr<NiNode>(block, static_cast<NiNode*>(block->object));
		m_rootBlock = block;

		//will get any referenced blocks, adding them to our index
		syncRead();
	}
}

void nif::File::write(const std::filesystem::path& path)
{
	if (!path.empty() && syncWrite()) {
		assert(m_rootBlock);//or syncWrite should have failed
		
		Niflib::NifInfo fileInfo;
		switch (m_version) {
		case Version::SKYRIM:
			fileInfo.version = 0x14020007;
			fileInfo.userVersion = 12;
			fileInfo.userVersion2 = 83;
			break;
		case Version::SKYRIM_SE:
			fileInfo.version = 0x14020007;
			fileInfo.userVersion = 12;
			fileInfo.userVersion2 = 100;
			break;
		}
		fileInfo.exportInfo1 = "SVFX Editor";
		fileInfo.exportInfo2 = "Niflib";

		std::ofstream out(path, std::ofstream::binary);
		Niflib::WriteNifTree(out, m_rootBlock->native, fileInfo);
	}
}

bool nif::File::syncRead()
{
	if (m_rootBlock) {
		assert(m_rootBlock->syncer);
		m_rootBlock->syncer->syncRead(*this, m_rootBlock->object, m_rootBlock->native);
		return true;
	}
	else
		return false;
}

bool nif::File::syncWrite() const
{
	if (m_rootBlock) {
		assert(m_rootBlock->syncer);
		m_rootBlock->syncer->syncWrite(*this, m_rootBlock->object, m_rootBlock->native);
		return true;
	}
	else
		return false;
}
