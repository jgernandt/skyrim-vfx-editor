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

namespace Niflib
{
	NiObjectRef FindRoot(std::vector<NiObjectRef> const& objects);
}

nif::File::File(Version version) : m_version{ version }
{
	m_rootNode = create<BSFadeNode>();
	if (m_rootNode)
		m_rootNode->name().set("NewFile.nif");
}

nif::File::File(const std::filesystem::path& path)
{
	if (!path.empty()) {
		std::ifstream in(path, std::ifstream::binary);
		Niflib::NifInfo fileInfo;
		auto objects = Niflib::ReadNifList(in, &fileInfo);
		in.close();

		Niflib::NiObjectRef root = Niflib::FindRoot(objects);

		for (auto&& obj : objects)
			addToIndex(obj, {});

		if (fileInfo.version == 0x14020007 && fileInfo.userVersion == 12) {
			if (fileInfo.userVersion2 == 83)
				m_version = Version::SKYRIM;
			else if (fileInfo.userVersion2 == 100)
				m_version = Version::SKYRIM_SE;
		}

		if (Niflib::NiNode* node = Niflib::DynamicCast<Niflib::NiNode>(root))
			m_rootNode = get<NiNode>(node);
	}
}

nif::File::~File()
{
	for (auto&& item : m_index) {
		if (item.first)
			item.first->SubtractRef();
	}
}

nif::File::index_type::const_iterator nif::File::addToIndex(
	native::NiObject* obj, const std::shared_ptr<NiObject>& ptr)
{
	if (obj) {
		if (auto result = m_index.insert({ obj, ptr }); result.second) {
			obj->AddRef();
			return result.first;
		}
		//else ignore? fail? this is an error!
		else {
			assert(false);
			return m_index.end();
		}
	}
	else
		return m_index.end();
}

bool nif::File::isCompatible(Version version) const
{
	switch (version) {
	case Version::SKYRIM:
		return m_version == Version::SKYRIM;
	case Version::SKYRIM_SE:
		return m_version == Version::SKYRIM_SE || m_version == Version::SKYRIM;
	default:
		return false;
	}
}

void nif::File::write(const std::filesystem::path& path)
{
	auto root = &m_rootNode->getNative();
	if (!path.empty() && root) {
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

		ofstream out(path, ofstream::binary);
		Niflib::WriteNifTree(out, root, fileInfo);
	}
}
