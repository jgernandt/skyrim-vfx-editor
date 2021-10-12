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

#pragma once
#include <filesystem>
#include <vector>
#include "nif_types.h"

namespace nif
{
	class File
	{
	public:
		//using Version = unsigned int;
		//constexpr static Version SKYRIM = 0x14020007;

		enum class Version
		{
			UNKNOWN,
			SKYRIM,
			SKYRIM_SE,
		};

	public:
		File(Version version = Version::UNKNOWN) : m_version{ version } {}
		File(const std::filesystem::path& path);

		//Add a new node under the given one (is this really how we want to do it?)
		void addFadeNode(native::NiNode* parent = nullptr);

		ni_ptr<native::NiObject> getRoot() const { return m_root; }
		Version getVersion() const { return m_version; }
		bool isCompatible(Version version) const;

		void write(const std::filesystem::path& path);

		static void write(native::NiObject* root, Version version, const std::filesystem::path& path);

	private:
		Version m_version{ Version::UNKNOWN };
		ni_ptr<native::NiObject> m_root;
	};

}