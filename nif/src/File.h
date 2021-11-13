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
		File(Version version = Version::UNKNOWN) : m_version{ version } {}
		File(const std::filesystem::path& path);
		~File();

		template<typename T, typename... Args>
		[[nodiscard]] std::shared_ptr<T> create(Args&&... args) 
		{ 
			static_assert(std::is_base_of<NiObject, T>::value);

			std::shared_ptr<T> result;
			//Catch allocation errors, not ctor exceptions (that's the caller's problem).
			// Actually, add this later. We're not checking for failure in our current code.
			//try {
				//make_shared cannot access protected ctors. What we need is allocate_shared with a custom allocator.
				result = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
				//result = std::make_shared<T>(std::forward<Args>(args)...);
				std::static_pointer_cast<NiObject>(result)->m_file = this;
				//std::static_pointer_cast<NiObject>(result)->m_it =
					addToIndex(std::static_pointer_cast<NiObject>(result)->m_ptr, result);
				//std::static_pointer_cast<NiObject>(result)->m_index = &m_index;
			//}
			//catch (const std::bad_alloc&) {
			//	result.reset();
			//}

			return result;
		}

		template<typename T, typename NativeType>
		[[nodiscard]] std::shared_ptr<T> get(NativeType* obj)
		{
			static_assert(std::is_base_of<NiObject, T>::value);
			static_assert(std::is_base_of<typename T::native_type, NativeType>::value);

			std::shared_ptr<T> result;
			if (auto it = m_index.find(obj); it != m_index.end()) {
				//downcast safe if T::native_type is correct
				result = std::static_pointer_cast<T>(it->second.lock());
				if (!result) {
					//recreate
					//try {
						//make_shared cannot access protected ctors. What we need is allocate_shared with a custom allocator.
						result = std::shared_ptr<T>(new T(obj));
						//result = std::make_shared<T>(obj);
						std::static_pointer_cast<NiObject>(result)->m_file = this;
						//std::static_pointer_cast<NiObject>(result)->m_it = it;
						//std::static_pointer_cast<NiObject>(result)->m_index = &m_index;
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

		//Add a new node under the given one (is this really how we want to do it?).
		//Update: it is not. Will be changed!
		void addFadeNode(native::NiNode* parent = nullptr);

		ni_ptr<native::NiObject> getRoot() const { return m_root; }
		Version getVersion() const { return m_version; }
		bool isCompatible(Version version) const;

		void write(const std::filesystem::path& path);

		static void write(native::NiObject* root, Version version, const std::filesystem::path& path);

	private:
		index_type::const_iterator addToIndex(native::NiObject* obj, const std::shared_ptr<NiObject>& ptr);

	private:
		Version m_version{ Version::UNKNOWN };
		ni_ptr<native::NiObject> m_root;

		//TODO: garbage collection (if first.GetNumRefs() == 1 && second.expired())
		index_type m_index;
	};

}