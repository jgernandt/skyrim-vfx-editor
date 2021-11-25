#pragma once
#include "File.h"

#include "NiObject_internal.h"
#include "NiController_internal.h"
#include "NiExtraData_internal.h"
#include "NiNode_internal.h"
#include "NiParticleSystem_internal.h"
#include "NiProperties_internal.h"
#include "NiPSysEmitter_internal.h"
#include "NiPSysModifier_internal.h"

namespace nif
{
	class ForwardingReadSyncer final : public HorizontalTraverser<ForwardingReadSyncer>
	{
		File& m_file;

	public:
		ForwardingReadSyncer(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& object);
	};

	class ForwardingWriteSyncer final : public HorizontalTraverser<ForwardingWriteSyncer>
	{
		File& m_file;

	public:
		ForwardingWriteSyncer(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& object);
	};

	class NonForwardingReadSyncer final : public HorizontalTraverser<NonForwardingReadSyncer>
	{
		File& m_file;

	public:
		NonForwardingReadSyncer(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& object);
	};

	class NonForwardingWriteSyncer final : public HorizontalTraverser<NonForwardingWriteSyncer>
	{
		File& m_file;

	public:
		NonForwardingWriteSyncer(File& file) : m_file{ file } {}

		template<typename T>
		void invoke(T& object);
	};

	template<typename T>
	inline [[nodiscard]] std::shared_ptr<T> File::get(const Niflib::Ref<typename type_map<T>::type>& nativeRef)
	{
		std::shared_ptr<T> result;

		if (nativeRef) {
			if (auto it = m_nativeIndex.find(static_cast<Niflib::NiObject*>(nativeRef)); it != m_nativeIndex.end()) {
				auto object = it->second.lock();
				//if (!object) {
					//The object is indexed, but it has expired.
					//This is unexpected, but not really a problem. We can just recreate it.
					//However, it is probably a bug. Let's treat it as such for now.
				//}
				assert(object);

				//downcast safe if type_map is correct
				result = std::static_pointer_cast<T>(object);
			}
			else
				//needs sync. Who's responsibility is that?
				result = make_ni<T>(nativeRef);
		}

		return result;
	}

	template<typename T>
	inline [[nodiscard]] Niflib::Ref<typename type_map<T>::type> File::getNative(T* object) const
	{
		Niflib::Ref<typename type_map<T>::type> result;

		if (object) {
			if (auto it = m_objectIndex.find(object); it != m_objectIndex.end()) {
				if (auto native = it->second.lock())
					//We know it can be cast to the native type since we know it was used to create object
					result = static_cast<typename type_map<T>::type*>(native.get());
				else
					//object is indexed but has expired, so it is dangling. This is a bug.
					assert(false);
			}
			else
				//object is not indexed, so it does not belong to us. This is also a bug.
				assert(false);
		}

		return result;
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

		nif::File::CreateFcn fcn = nullptr;

		const Niflib::Type* type = native ? &native->GetType() : &type_map<T>::type::TYPE;

		while (type) {
			if (auto it = s_typeRegistry.find(std::hash<const Niflib::Type*>{}(type)); it != s_typeRegistry.end()) {
				fcn = it->second;
				break;
			}

			if (type == &type_map<T>::type::TYPE)
				break;
			else
				type = type->base_type;
		}

		assert(fcn);

		//The CreateFcn must guarantee that it creates an object of the type mapped to from
		//type_map<Y>::type, where Y is the Niflib type associated with the Niflib::Type object
		//that the CreateFcn is mapped to.

		auto pair = fcn(Niflib::StaticCast<Niflib::NiObject>(native));

		//factory should throw on failure. We won't catch it, we'll typically be part of a longer procedure.
		assert(pair.first && pair.second);

		if (auto res = m_objectIndex.insert({ pair.first.get(), pair.second }); !res.second) {
			//The object has already been indexed. Since we just created it, this must be a reused address.
			//The block must have expired.
			assert(res.first->second.expired());
			res.first->second = pair.second;
		}
		if (auto res = m_nativeIndex.insert({ pair.second.get(), pair.first }); !res.second) {
			//if native was created by us, the address has been reused and the block must have expired.
			assert(!native || res.first->second.expired());
			res.first->second = pair.first;
		}

		//Make sure the output object is synced to the Niflib object
		NonForwardingReadSyncer syncer(*this);
		pair.first->receive(syncer);

		//downcast safe if type_map is correct
		return std::static_pointer_cast<T>(pair.first);
	}

	template<typename T>
	inline void ForwardingReadSyncer::invoke(T& object)
	{
		ReadSyncer<T>{}.down(object, m_file.getNative<T>(&object), m_file);
		Forwarder<T>{}.down(object, *this);
	}

	template<typename T>
	inline void ForwardingWriteSyncer::invoke(T& object)
	{
		WriteSyncer<T>{}.down(object, m_file.getNative<T>(&object), m_file);
		Forwarder<T>{}.down(object, *this);
	}

	template<typename T>
	inline void NonForwardingReadSyncer::invoke(T& object)
	{
		ReadSyncer<T>{}.down(object, m_file.getNative<T>(&object), m_file);
	}

	template<typename T>
	inline void NonForwardingWriteSyncer::invoke(T& object)
	{
		WriteSyncer<T>{}.down(object, m_file.getNative<T>(&object), m_file);
	}
}
