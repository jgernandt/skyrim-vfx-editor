//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "File.h"

nif::File::File(const std::filesystem::path& path)
{
	if (!path.empty()) {
		std::ifstream in(path, std::ifstream::binary);
		Niflib::NifInfo fileInfo;
		Niflib::NiObjectRef root = Niflib::ReadNifTree(in, &fileInfo);
		in.close();

		if (fileInfo.version == 0x14020007 && fileInfo.userVersion == 12) {
			if (fileInfo.userVersion2 == 83)
				m_version = Version::SKYRIM;
			else if (fileInfo.userVersion2 == 100)
				m_version = Version::SKYRIM_SE;
		}

		m_root.reset(static_cast<Niflib::NiObject*>(root));
	}
}

void nif::File::addFadeNode(native::NiNode* parent)
{
	auto ptr = new Niflib::BSFadeNode;
	ptr->SetName("NewFile.nif");
	m_root.reset(ptr);
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
	write(m_root.get(), m_version, path);
}

void nif::File::write(native::NiObject* root, Version version, const std::filesystem::path& path)
{
	if (!path.empty() && root) {
		Niflib::NifInfo fileInfo;
		switch (version) {
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

		fileInfo.exportInfo1 = "VFX Editor";
		fileInfo.exportInfo2 = "Niflib";

		ofstream out(path, ofstream::binary);
		Niflib::WriteNifTree(out, root, fileInfo);
	}
}
