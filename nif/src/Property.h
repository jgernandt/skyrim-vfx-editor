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
#include <cassert>
#include "Observable.h"
#include "nif_types.h"

template<typename T>
class IProperty : public IObservable<IProperty<T>>
{
public:
	virtual ~IProperty() = default;

	virtual T get() const = 0;
	virtual void set(const T&) = 0;
};

template<typename T>
class IListener<IProperty<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onSet(const T&) {}
};
template<typename T>
using PropertyListener = IListener<IProperty<T>>;

namespace nif
{
	template<typename T>
	class PropertyBase : public IProperty<T>
	{
	public:
		virtual ~PropertyBase() = default;

		virtual void addListener(PropertyListener<T>& l) final override
		{
			m_obs.addListener(l);
			l.onSet(this->get());
		}
		virtual void removeListener(PropertyListener<T>& l) final override { m_obs.removeListener(l); }

	protected:
		void notify(const T& t)
		{
			for (PropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onSet(t);
			}
		}

	private:
		ObservableBase<IProperty<T>> m_obs;
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
			assert(obj&& g&& s);
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
}
