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
#include <filesystem>
#include <vector>
#include "nif_types.h"
#include "NiObject.h"

namespace nif
{
	class NiNode;

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

		using index_type = std::map<native::NiObject*, std::weak_ptr<nif::NiObject>>;

	public:
		File(Version version = Version::UNKNOWN);
		File(const std::filesystem::path& path);

		File(const File&) = delete;
		File& operator=(const File&) = delete;

		~File();

		template<typename T, typename... Args>
		[[nodiscard]] std::shared_ptr<T> create(Args&&... args) 
		{ 
			static_assert(std::is_base_of<NiObject, T>::value);

			std::shared_ptr<T> result;
			//Catch allocation errors, not ctor exceptions (that's the caller's problem).
			// Actually, add this later. We're not checking for failure in our current code.
			//try {
				result = std::make_shared<T>(std::forward<Args>(args)...);
				addToIndex(std::static_pointer_cast<NiObject>(result)->m_ptr, result);
			//}
			//catch (const std::bad_alloc&) {
			//	result.reset();
			//}

			return result;
		}

		template<typename T, typename NativeType>
		[[nodiscard]] std::shared_ptr<T> get(const Niflib::Ref<NativeType>& objRef)
		{
			static_assert(std::is_base_of<NiObject, T>::value);
			static_assert(std::is_base_of<typename T::native_type, NativeType>::value);

			NativeType* obj = static_cast<NativeType*>(objRef);

			std::shared_ptr<T> result;
			if (auto it = m_index.find(obj); it != m_index.end()) {
				//downcast safe if T::native_type is correct
				result = std::static_pointer_cast<T>(it->second.lock());
				if (!result) {
					//recreate
					//try {
						result = std::make_shared<T>(obj);
						it->second = std::weak_ptr<NiObject>(result);
					//}
					//catch (const std::bad_alloc&) {
					//	result.reset();
					//}
				}
			}
			//else ignore. Do not create.

			return result;
		}

		std::shared_ptr<NiNode> getRoot() const { return m_rootNode; }
		Version getVersion() const { return m_version; }
		bool isCompatible(Version version) const;

		void write(const std::filesystem::path& path);

	private:
		index_type::const_iterator addToIndex(native::NiObject* obj, const std::shared_ptr<NiObject>& ptr);

	private:
		Version m_version{ Version::UNKNOWN };
		std::shared_ptr<NiNode> m_rootNode;

		index_type m_index;
	};

}