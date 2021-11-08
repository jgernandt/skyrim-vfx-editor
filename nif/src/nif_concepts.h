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

template<typename T, typename ContainerType = std::vector<T>>
class IListProperty : public IObservable<IListProperty<T>>
{
public:
	virtual ~IListProperty() = default;

	//Regular property functions
	virtual ContainerType get() const = 0;
	virtual void set(const ContainerType&) = 0;

	//Allow random access
	virtual T get(int) const = 0;
	virtual void set(int, const T&) = 0;

	//Allow insertions/deletions
	virtual int insert(int, const T&) = 0;
	virtual int erase(int) = 0;

	//Return a property representing element i (if we have one)
	//(This signature seems wrong. What's the right way of doing this?)
	virtual std::unique_ptr<IProperty<T>> element(int i) = 0;
};



template<typename T>
class IListener<IAssignable<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAssign(T*) {}
};
template<typename T>
using AssignableListener = IListener<IAssignable<T>>;

template<typename T>
class IListener<IProperty<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onSet(const T&) {}
};
template<typename T>
using PropertyListener = IListener<IProperty<T>>;

template<typename T>
class IListener<ISequence<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onInsert(const ISequence<T>&, size_t) {}
	virtual void onErase(const ISequence<T>&, size_t) {}
};
template<typename T>
using SequenceListener = IListener<ISequence<T>>;

template<typename T>
class IListener<ISet<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAdd(const T&) {}
	virtual void onRemove(const T&) {}
};
template<typename T>
using SetListener = IListener<ISet<T>>;

template<typename T, typename ContainerType>
class IListener<IListProperty<T, ContainerType>>
{
public:
	virtual ~IListener() = default;

	virtual void onSet(const ContainerType&) {}
	virtual void onSet(int, const T&) {}
	virtual void onInsert(int) {}
	virtual void onErase(int) {}
	virtual void onDestroy() {}
};
template<typename T, typename ContainerType = std::vector<T>>
using ListPropertyListener = IListener<IListProperty<T, ContainerType>>;


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

		virtual void addListener(AssignableListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(AssignableListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notify(T* t)
		{
			for (AssignableListener<T>* l : m_obsImpl.getListeners()) {
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

		virtual void addListener(PropertyListener<T>& l) final override
		{
			m_obsImpl.addListener(l);
			l.onSet(this->get());
		}
		virtual void removeListener(PropertyListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notify(const T& t)
		{
			for (PropertyListener<T>* l : m_obsImpl.getListeners()) {
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

		virtual void addListener(SequenceListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(SequenceListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notifyInsert(size_t pos) const
		{
			for (SequenceListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onInsert(*this, pos);
			}
		}
		void notifyErase(size_t pos) const
		{
			for (SequenceListener<T>* l : m_obsImpl.getListeners()) {
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

		virtual void addListener(SetListener<T>& l) final override { m_obsImpl.addListener(l); }
		virtual void removeListener(SetListener<T>& l) final override { m_obsImpl.removeListener(l); }

	protected:
		void notifyAdd(const T& t) const
		{
			for (SetListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onAdd(t);
			}
		}
		void notifyRemove(const T& t) const
		{
			for (SetListener<T>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onRemove(t);
			}
		}

	private:
		ObservableImpl<ISet<T>> m_obsImpl;
	};


	template<typename T, typename ContainerType = std::vector<T>>
	class ListPropertyBase : public IListProperty<T, ContainerType>
	{
	public:
		virtual ~ListPropertyBase()
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onDestroy();
			}
		}

		virtual void addListener(ListPropertyListener<T, ContainerType>&l) final override 
		{ m_obsImpl.addListener(l); }
		virtual void removeListener(ListPropertyListener<T, ContainerType>&l) final override 
		{ m_obsImpl.removeListener(l); }

	protected:
		void notifySet(const ContainerType& t) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onSet(t);
			}
		}
		void notifySet(int i, const T& t) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onSet(i, t);
			}
		}
		void notifyInsert(int i) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onInsert(i);
			}
		}
		void notifyErase(int i) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obsImpl.getListeners()) {
				assert(l);
				l->onErase(i);
			}
		}

	private:
		ObservableImpl<IListProperty<T, ContainerType>> m_obsImpl;
	};

	//We can wrap an element of an IListProperty inside a regular IProperty.
	//Exactly how to do this depends on the container type. Here's an example
	//suitable for vectors:
	template<typename T, typename ContainerType = std::vector<T>>
	class VectorElementProperty final : 
		public PropertyBase<T>, 
		public ListPropertyListener<T, ContainerType>
	{
	public:
		VectorElementProperty(IListProperty<T, ContainerType>& list, int index) :
			m_list{ &list }, m_index{ index }
		{
			list.addListener(*this);
		}
		~VectorElementProperty()
		{
			invalidate();
		}

		virtual T get() const override 
		{ 
			if (valid()) {
				assert(m_list);
				return m_list->get(m_index);
			}
			else
				return T();
		}
		virtual void set(const T& t) override 
		{
			if (valid()) {
				assert(m_list);
				m_list->set(m_index, t);
			}
		}

		virtual void onSet(const ContainerType& list) override 
		{ 
			if (valid() && static_cast<size_t>(m_index) < list.size())
				this->notify(list[m_index]);//we don't know if our element actually changed
			else
				invalidate();

			//Or should this always invalidate us? 
			//Techically, the whole list has been replaced. Our element no longer exists.
		}
		virtual void onSet(int i, const T& t) override 
		{
			assert(i >= 0);
			if (i == m_index)
				this->notify(t);
		}
		virtual void onInsert(int i) override 
		{
			assert(i >= 0);
			if (i <= m_index)
				m_index++;
		}
		virtual void onErase(int i) override 
		{
			assert(i >= 0);
			if (i == m_index)
				invalidate();
			else if (i < m_index)
				m_index--;
		}
		virtual void onDestroy() override { m_index = -1; }//we must not remove listener here

	private:
		void invalidate()
		{
			if (valid()) {
				assert(m_list);
				m_list->removeListener(*this);
				m_index = -1;
			}
		}
		bool valid() const { return m_index >= 0; }

	private:
		IListProperty<T, ContainerType>* m_list;
		int m_index;
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