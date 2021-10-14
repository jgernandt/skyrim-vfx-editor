//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include <functional>
#include <vector>
#include "nif_types.h"
#include "Rotation.h"


template<typename T>
class IListener
{
public:
	virtual ~IListener() = default;

	virtual void call(const T&) = 0;
};


template<typename T>
class IObservable
{
public:
	virtual ~IObservable() = default;

	virtual void addListener(IListener<T>&) = 0;
	virtual void removeListener(IListener<T>&) = 0;

};


//like a Property for class types
template<typename T>
class IAssignable : public IObservable<IAssignable<T>>
{
public:
	virtual ~IAssignable() = default;

	virtual void assign(T*) = 0;
	virtual bool isAssigned(T*) const = 0;
};

template<typename T>
class IProperty : public IObservable<IProperty<T>>
{
public:
	virtual ~IProperty() = default;

	virtual T get() const = 0;
	virtual void set(const T&) = 0;
};

template<typename T>
class ISequence : public IObservable<ISequence<T>>
{
public:
	virtual ~ISequence() = default;

	//index -1 (cast to unsigned) indicates end here
	virtual size_t insert(size_t, const T&) = 0;
	virtual size_t erase(size_t) = 0;
	virtual size_t find(const T&) const = 0;
	virtual size_t size() const = 0;
};

template<typename T>
class ISet : public IObservable<ISet<T>>
{
public:
	virtual ~ISet() = default;

	virtual void add(const T&) = 0;
	virtual void remove(const T&) = 0;
	virtual bool has(const T&) const = 0;
	virtual size_t size() const = 0;
};



template<typename T>
class IListener<IAssignable<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAssign(T*) = 0;
};
template<typename T>
using IAssignableListener = IListener<IAssignable<T>>;

template<typename T>
class IListener<IProperty<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onSet(const T&) = 0;
};
template<typename T>
using IPropertyListener = IListener<IProperty<T>>;

template<typename T>
class IListener<ISequence<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onInsert(const ISequence<T>&, size_t) = 0;
	virtual void onErase(const ISequence<T>&, size_t) = 0;
};
template<typename T>
using ISequenceListener = IListener<ISequence<T>>;

template<typename T>
class IListener<ISet<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAdd(const T&) = 0;
	virtual void onRemove(const T&) = 0;
};
template<typename T>
using ISetListener = IListener<ISet<T>>;




/*template<typename T>
class IObservableAssignable : public IAssignable<T>, public IObservable<IAssignable<T>>
{
public:
	virtual ~IObservableAssignable() = default;
};

template<typename T>
class IObservableProperty : public IProperty<T>, public IObservable<IProperty<T>>
{
public:
	virtual ~IObservableProperty() = default;
};

template<typename T>
class IObservableSequence : public ISequence<T>, public IObservable<ISequence<T>>
{
public:
	virtual ~IObservableSequence() = default;
};

template<typename T>
class IObservableSet : public ISet<T>, public IObservable<ISet<T>>
{
public:
	virtual ~IObservableSet() = default;
};*/

template<typename T>
class ObservableImpl : public IObservable<T>
{
public:
	virtual ~ObservableImpl() = default;

	virtual void addListener(IListener<T>& l) final override
	{
		if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it == m_listeners.end())
			m_listeners.push_back(&l);
	}

	virtual void removeListener(IListener<T>& l) final override
	{
		if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it != m_listeners.end())
			m_listeners.erase(it);
	}

	const std::vector<IListener<T>*>& getListeners() const { return m_listeners; }

private:
	std::vector<IListener<T>*> m_listeners;
};


namespace nif
{
	template<typename T>
	class AssignableBase : public IAssignable<T>
	{
	public:
		virtual ~AssignableBase() = default;

		virtual void addListener(IAssignableListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(IAssignableListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notify(T* t)
		{
			for (IAssignableListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onAssign(t);
			}
		}

	private:
		ObservableImpl<IAssignable<T>> m_obsImpl;
	};

	template<typename T>
	class PropertyBase : public IProperty<T>
	{
	public:
		virtual ~PropertyBase() = default;

		virtual void addListener(IPropertyListener<T>& l) final override
		{
			m_obsImpl.addListener(l);
			l.onSet(this->get());
		}
		virtual void removeListener(IPropertyListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notify(const T& t)
		{
			for (IPropertyListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onSet(t);
			}
		}

	private:
		ObservableImpl<IProperty<T>> m_obsImpl;
	};

	//Typically, ArgType would be T (e.g. when T=float) or const & to some native type.
	//The latter typically means that RetType is this native type. It may, occasionally, be lvalue reference to it.
	template<typename T,
		typename NativeType = T,
		//typename ArgType = T,
		//typename RetType = typename std::remove_cv<typename std::remove_reference<T>::type>::type, 
		template<typename> typename Converter = NifConverter>
	class Property final : public PropertyBase<T>
	{
	public:
		Property(const std::function<T()>& get, const std::function<void(const T&)>& set) :
			m_get{ get }, m_set{ set } {}

		//RetType may be the native type or any reference thereto.
		//ArgType may be the native type, rvalue reference to the native type or const lvalue reference to the native type.
		template<typename ObjType, typename RetType, typename ArgType, typename BaseType>
		Property(ObjType* obj, RetType(BaseType::* g)() const, void(BaseType::* s)(ArgType)) :
			m_get{ [obj, g]() -> T { return util::type_conversion<T, Converter<T>>::from((obj->*g)()); } },
			m_set{ [obj, s](const T& t) { (obj->*s)(util::type_conversion<NativeType, Converter<NativeType>>::from(t)); } }
			//m_get{ std::bind(g, obj) },
			//m_set{ std::bind(s, obj, std::placeholders::_1) }
		{
			static_assert(std::is_base_of<BaseType, ObjType>::value);
			assert(obj && g && s); 
		}

		//non-const getter
		template<typename ObjType, typename RetType, typename ArgType>
		Property(ObjType* obj, RetType(ObjType::* g)(), void(ObjType::* s)(ArgType)) :
			Property(obj, (RetType(ObjType::*)() const)g, s) {}
		//	m_get{ [obj, g]() -> T { return util::type_conversion<T, Converter<T>>::from((obj->*g)()); } },
		//	m_set{ [obj, s](const T& t) { (obj->*s)(util::type_conversion<NativeType, Converter<NativeType>>::from(t)); } }
		//{ assert(obj && g && s); }

		virtual T get() const override
		{
			assert(m_get);
			return m_get();
			//return util::type_conversion<T, Converter<T>>::from(m_get());
		}

		virtual void set(const T& t) override
		{
			assert(m_set && m_get);
			if (m_get() != t) {
				//m_set(util::type_conversion<NativeType, Converter<NativeType>>::from(t));
				m_set(t);
				this->notify(t);
			}
		}

	private:
		std::function<T()> m_get;
		std::function<void(const T&)> m_set;
		//std::function<NativeType()> m_get;
		//std::function<void(const NativeType&)> m_set;
	};

	template<typename T>
	class Assignable final : public AssignableBase<T>
	{
	public:
		using native_type = typename std::remove_reference<decltype(std::declval<T>().getNative())>::type;

	public:
		//RetType and ArgType here may be Niflib::Ref<native_type>, or a pointer to some type related to native_type
		template<typename ObjType, typename RetType, typename BaseType>
		Assignable(ObjType* obj, RetType(BaseType::* g)() const, void(BaseType::* s)(native_type*)) :
			m_get{ std::bind(g, obj) },
			m_set{ std::bind(s, obj, std::placeholders::_1) }
		{ 
			static_assert(std::is_base_of<BaseType, ObjType>::value);
			assert(obj && g && s); 
		}

		virtual void assign(T* t) override
		{
			assert(m_set);
			m_set(t ? &t->getNative() : nullptr);

			this->notify(t);
		}
		virtual bool isAssigned(T* t) const override
		{
			assert(m_get);
			return m_get() == (t ? &t->getNative() : nullptr);
		}

	private:
		std::function<native_type* ()> m_get;
		std::function<void(native_type*)> m_set;
	};

	template<typename T>
	class SequenceBase : public ISequence<T>
	{
	public:
		virtual ~SequenceBase() = default;

		virtual void addListener(ISequenceListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(ISequenceListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notifyInsert(size_t pos) const
		{
			for (ISequenceListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onInsert(*this, pos);
			}
		}
		void notifyErase(size_t pos) const
		{
			for (ISequenceListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onErase(*this, pos);
			}
		}

	private:
		ObservableImpl<ISequence<T>> m_obsImpl;
	};

	template<typename T>
	class SetBase : public ISet<T>
	{
	public:
		virtual ~SetBase() = default;

		virtual void addListener(ISetListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(ISetListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notifyAdd(const T& t) const
		{
			for (ISetListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onAdd(t);
			}
		}
		void notifyRemove(const T& t) const
		{
			for (ISetListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onRemove(t);
			}
		}

	private:
		ObservableImpl<ISet<T>> m_obsImpl;
	};

	//Really just a Set, but intended for int or enum types
	template<typename T>
	class FlagSet
	{
	public:
		virtual ~FlagSet() = default;

		virtual void set(T, bool) = 0;
		virtual bool isSet(T) const = 0;
	};

	class Transformable
	{
	public:
		virtual ~Transformable() = default;

		virtual IProperty<translation_t>& translation() = 0;
		virtual IProperty<rotation_t>& rotation() = 0;
		virtual IProperty<scale_t>& scale() = 0;
	};
}

template<typename T>
struct util::field_traits<nif::FlagSet<T>>
{
	using field_type = nif::FlagSet<T>;
	using index_type = T;
	using value_type = bool;

	static bool get(const field_type& t, index_type i) { return t.isSet(i); }
	static void set(field_type& t, index_type i, bool val) { t.set(i, val); }
};