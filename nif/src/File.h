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
#include <map>
#include <set>
#include <vector>

#include "ni_objects.h"

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

		using FactoryFcn = 
			std::pair<std::shared_ptr<NiObject>, std::shared_ptr<Niflib::NiObject>>
			(*)(File&, const Niflib::Ref<Niflib::NiObject>&);

	public:
		File(Version version = Version::UNKNOWN);
		File(const std::filesystem::path& path);

		File(const File&) = delete;
		File& operator=(const File&) = delete;

		~File();

		//template<typename T>
		//[[nodiscard]] std::shared_ptr<T> create();

		//template<typename T, typename NativeType>
		//[[nodiscard]] std::shared_ptr<T> get(const Niflib::Ref<NativeType>& objRef);

		Version getVersion() const { return m_version; }

		std::shared_ptr<NiNode> getRoot() const { return m_rootNode; }
		void makeRoot(const Niflib::Ref<Niflib::NiNode>& node);

		void write(const std::filesystem::path& path);

		static FactoryFcn pushType(size_t type, FactoryFcn fcn) { return s_typeRegistry[type] = fcn; }

	private:
		template<typename T>
		std::shared_ptr<T> make_ni(const Niflib::Ref<typename type_map<T>::type>& native);
		template<typename T>
		std::shared_ptr<T> make_ni() { return make_ni<T>(Niflib::Ref<typename type_map<T>::type>()); }

	private:
		static std::map<size_t, FactoryFcn> s_typeRegistry;

		Version m_version{ Version::UNKNOWN };
		std::shared_ptr<NiNode> m_rootNode;

		std::map<NiObject*, std::weak_ptr<Niflib::NiObject>> m_nativeIndex;
		std::map<Niflib::NiObject*, std::weak_ptr<NiObject>> m_objectIndex;
	};
	/*
	template<typename T>
	inline [[nodiscard]] std::shared_ptr<T> File::create()
	{
		static_assert(std::is_base_of<NiObject, T>::value);

		//Catch allocation errors, not ctor exceptions (that's the caller's problem).
		// Actually, add this later. We're not checking for failure in our current code.
		//try {
		return std::static_pointer_cast<T>(T::type_id);
		//}
		//catch (const std::bad_alloc&) {
		//	return std::shared_ptr<T>();
		//}
	}

	template<typename T, typename NativeType>
	inline [[nodiscard]] std::shared_ptr<T> File::get(const Niflib::Ref<NativeType>& objRef)
	{
		static_assert(std::is_base_of<NiObject, T>::value);
		static_assert(std::is_base_of<typename T::native_type, NativeType>::value);

		NativeType* obj = static_cast<NativeType*>(objRef);

		std::shared_ptr<T> result;
		if (auto it = m_index.find(obj); it != m_index.end()) {
			//downcast safe if T::native_type is correct
			result = std::static_pointer_cast<T>(it->second.lock());
			if (!result) {
				//try {
				result = std::static_pointer_cast<T>(make_ni(obj));
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
	*/
	template<typename T>
	inline std::shared_ptr<T> File::make_ni(const Niflib::Ref<typename type_map<T>::type>& native)
	{
		//Our type registry contains all the types that we care about.
		//If we step up through the inheritance chain of Niflib's type object,
		//the first one we find in our registry is the most derived type that 
		//we care about.
		//There are nicer ways to set that up, but they require more work.

		FactoryFcn fcn = nullptr;

		Niflib::Type* type = native ? native->GetType() : &type_map<T>::type::TYPE;
		while (type) {
			if (auto it = s_typeRegistry.find(std::hash<Niflib::Type*>{}(type)); it != s_typeRegistry.end()) {
				fcn = it->second;
				break;
			}
			else
				type = type->base_type;
		}
		assert(fcn);

		auto pair = fcn(*this, Niflib::StaticCast<Niflib::NiObject>(native));
		//factory should throw on failure. We won't catch it, we'll typically be part of a longer procedure.
		assert(pair.first && pair.second);

		m_nativeIndex.insert({ pair.first.get(), pair.second });
		m_objectIndex.insert({ pair.second.get(), pair.first });

		//Unless we messed something up, the created object should be of (at least) type T.
		return std::static_pointer_cast<T>(pair.first);
	}
}
