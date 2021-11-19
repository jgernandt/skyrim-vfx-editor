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
	class File;
	class ForwardingReadSyncer;
	class ForwardingWriteSyncer;
	class NonForwardingReadSyncer;
	class NonForwardingWriteSyncer;

	template<typename T>
	struct ForwardingReader
	{
		void operator() (T& obj, ForwardingReadSyncer& t);
	};
	template<typename T>
	struct ForwardingWriter
	{
		void operator() (T& obj, ForwardingWriteSyncer& t);
	};
	template<typename T>
	struct NonForwardingReader
	{
		void operator() (T& obj, NonForwardingReadSyncer& t);
	};
	template<typename T>
	struct NonForwardingWriter
	{
		void operator() (T& obj, NonForwardingWriteSyncer& t);
	};

	class ForwardingReadSyncer final : public HorizontalTraverser<ForwardingReadSyncer, ForwardingReader>
	{
	public:
		File& file;
		ForwardingReadSyncer(File& file) : file{ file } {}
	};

	class ForwardingWriteSyncer final : public HorizontalTraverser<ForwardingWriteSyncer, ForwardingWriter>
	{
	public:
		File& file;
		ForwardingWriteSyncer(File& file) : file{ file } {}
	};

	class NonForwardingReadSyncer final : public HorizontalTraverser<NonForwardingReadSyncer, NonForwardingReader>
	{
	public:
		File& file;
		NonForwardingReadSyncer(File& file) : file{ file } {}
	};

	class NonForwardingWriteSyncer final : public HorizontalTraverser<NonForwardingWriteSyncer, NonForwardingWriter>
	{
	public:
		File& file;
		NonForwardingWriteSyncer(File& file) : file{ file } {}
	};

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

	private:
		//This object owns the resources that it points to and will clean them up on destruction.
		struct ObjectBlock
		{
			Niflib::NiObject* native;
			NiObject* object;
		};

		using CreateFcn = std::shared_ptr<ObjectBlock>(*)(const Niflib::Ref<Niflib::NiObject>&);

	public:
		File(Version version = Version::UNKNOWN);
		File(const std::filesystem::path& path);

		File(const File&) = delete;
		File& operator=(const File&) = delete;

		~File();

		//Create a new object of type T.
		template<typename T>
		[[nodiscard]] std::shared_ptr<T> create() { return make_ni<T>(nullptr); }

		//Return the object that corresponds to Niflib object nativeRef.
		//Creates a new object if none existed.
		//(Should maybe be called getOrCreate, or similar)
		template<typename T, typename NativeType>
		[[nodiscard]] std::shared_ptr<T> get(const Niflib::Ref<NativeType>& nativeRef);

		//Return the Niflib object that corresponds to object.
		template<typename NativeType, typename T>
		[[nodiscard]] Niflib::Ref<NativeType> get(T* object) const;

		Version getVersion() const { return m_version; }

		std::shared_ptr<NiNode> getRoot() const { return m_rootNode; }

		void write(const std::filesystem::path& path);

	private:
		//Create a new object and add to our index
		template<typename T>
		std::shared_ptr<T> make_ni(const Niflib::Ref<typename type_map<T>::type>& native);

		//Our factory functions
		template<typename T>
		static std::shared_ptr<ObjectBlock> make_NiObject(const Niflib::Ref<Niflib::NiObject>& native);

	private:
		static void registerTypes();

	private:
		static std::map<size_t, CreateFcn> s_typeRegistry;

		Version m_version{ Version::UNKNOWN };
		std::shared_ptr<NiNode> m_rootNode;

		std::map<Niflib::NiObject*, std::weak_ptr<ObjectBlock>> m_nativeIndex;
		std::map<NiObject*, std::weak_ptr<ObjectBlock>> m_objectIndex;
	};
	
	template<typename T, typename NativeType>
	inline [[nodiscard]] std::shared_ptr<T> File::get(const Niflib::Ref<NativeType>& nativeRef)
	{
		static_assert(std::is_base_of<typename type_map<T>::type, NativeType>::value);

		if (auto it = m_nativeIndex.find(static_cast<NativeType*>(nativeRef)); it != m_nativeIndex.end()) {
			auto block = it->second.lock();
			//if (!block) {
				//The object is indexed, but it has expired.
				//This is unexpected, but not really a problem. We can just recreate it.
				//However, it is probably a bug. Let's treat it as such for now.
			//}
			assert(block);

			//downcast safe if type_map is correct
			return std::shared_ptr<T>(block, static_cast<T*>(block->object));
		}
		else
			//needs sync. Who's responsibility is that?
			return make_ni<T>(nativeRef);
	}

	template<typename NativeType, typename T>
	inline [[nodiscard]] Niflib::Ref<NativeType> nif::File::get(T* object) const
	{
		static_assert(std::is_base_of<NativeType, typename type_map<T>::type>::value);

		std::shared_ptr<ObjectBlock> block;
		if (auto it = m_objectIndex.find(object); it != m_objectIndex.end())
			block = it->second.lock();

		//If the object is indexed but expired, object is dangling. This is a bug.
		//If the object is not indexed, it was not created by us. This is also a bug.
		assert(block);

		//We know it can be cast to NativeType since we know it was used to create object, 
		//and NativeType is a base of the native type of T (type_map<T>::type).
		return static_cast<NativeType*>(block->native);
	}
	
	template<typename T>
	inline std::shared_ptr<T> File::make_ni(const Niflib::Ref<typename type_map<T>::type>& native)
	{
		//Our type registry contains all the types that we care about.
		//If we step up through the inheritance chain of Niflib's type object,
		//the first one we find in our registry is the most derived type that 
		//we care about.

		//We must not go higher up than type_map<T>::type, since that should be
		//the least derived type that maps to T.
		static bool registered = false;
		if (!registered) {
			registerTypes();
			registered = true;
		}

		CreateFcn fcn = nullptr;

		const Niflib::Type* type = native ? &native->GetType() : &type_map<T>::type::TYPE;

		do {
			if (auto it = s_typeRegistry.find(std::hash<const Niflib::Type*>{}(type)); it != s_typeRegistry.end()) {
				fcn = it->second;
				break;
			}
			else {
				assert(type != &type_map<T>::type::TYPE);//or we failed to register this type
				type = type->base_type;
			}
		} while (type != &type_map<T>::type::TYPE);

		assert(fcn);

		//The CreateFcn must guarantee that it creates an object of the type mapped to from
		//type_map<Y>::type, where Y is the Niflib type associated with the Niflib::Type object
		//that the CreateFcn is mapped to.

		auto block = fcn(Niflib::StaticCast<Niflib::NiObject>(native));

		//factory should throw on failure. We won't catch it, we'll typically be part of a longer procedure.
		assert(block);

		if (auto res = m_objectIndex.insert({ block->object, block }); !res.second) {
			//The object has already been indexed. This should be checked before calling us.
			assert(false);
		}
		if (auto res = m_nativeIndex.insert({ block->native, block }); !res.second) {
			//same here
			assert(false);
		}

		//Make sure the output object is synced to the Niflib object
		NonForwardingReadSyncer syncer(*this);
		block->object->receive(syncer);

		//downcast safe if type_map is correct
		return std::shared_ptr<T>(block, static_cast<T*>(block->object));
	}

	template<typename T>
	inline std::shared_ptr<File::ObjectBlock> File::make_NiObject(const Niflib::Ref<Niflib::NiObject>& native)
	{
		//Our implementation of ObjectBlock keeps everything in the same object.
		//The pointers in ObjectBlock are aliases for the members of the derived type.
		struct NiObjectBlock : ObjectBlock
		{
			NiObjectBlock(const Niflib::Ref<Niflib::NiObject>& ref) :
				nativeRef{ ref }
			{
				if (!nativeRef)
					nativeRef = new typename type_map<T>::type();
				this->native = nativeRef;
				this->object = &objectImpl;
			}
			NiObjectBlock(const NiObjectBlock&) = delete;
			NiObjectBlock& operator=(const NiObjectBlock&) = delete;

			Niflib::Ref<Niflib::NiObject> nativeRef;
			T objectImpl;
		};

		return std::make_shared<NiObjectBlock>(native);
	}

	template<typename T>
	inline void nif::ForwardingReader<T>::operator()(T& obj, ForwardingReadSyncer& t)
	{
		ReadSyncer<T>{}.down(obj, t.file.get<typename type_map<T>::type>(&obj), t.file);
		Forwarder<T>{}.down(obj, t);
	}

	template<typename T>
	inline void nif::ForwardingWriter<T>::operator()(T& obj, ForwardingWriteSyncer& t)
	{
		WriteSyncer<T>{}.down(obj, t.file.get<typename type_map<T>::type>(&obj), t.file);
		Forwarder<T>{}.down(obj, t);
	}

	template<typename T>
	inline void nif::NonForwardingReader<T>::operator()(T& obj, NonForwardingReadSyncer& t)
	{
		ReadSyncer<T>{}.down(obj, t.file.get<typename type_map<T>::type>(&obj), t.file);
	}

	template<typename T>
	inline void nif::NonForwardingWriter<T>::operator()(T& obj, NonForwardingWriteSyncer& t)
	{
		WriteSyncer<T>{}.down(obj, t.file.get<typename type_map<T>::type>(&obj), t.file);
	}
}
