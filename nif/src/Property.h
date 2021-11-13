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
#include <functional>
#include "Observable.h"
#include "DataField.h"

template<typename T>
class IProperty
{
public:
	virtual ~IProperty() = default;

	virtual T get() const = 0;
	virtual void set(const T&) = 0;
};

template<typename T>
using OProperty = IObservable<IProperty<T>>;

template<typename T>
class IListener<IProperty<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onSet(const T&) {}
};

namespace nif
{
	template<typename T>
	using Property = NiObject::DataField<IProperty<T>>;

	template<typename T>
	using PropertyListener = IListener<IProperty<T>>;

	template<typename T, typename BlockType>
	class PropertyBase : public Property<T>
	{
	public:
		PropertyBase(BlockType& block) : Property<T>(block) {}
		virtual ~PropertyBase() = default;

	protected:
		typename BlockType::native_type* nativePtr() const
		{
			assert(this->m_block);
			return &static_cast<BlockType*>(this->m_block)->getNative();
		}
		typename BlockType& block() const
		{
			assert(this->m_block);
			return *static_cast<BlockType*>(this->m_block);
		}

		void notify(const T& t)
		{
			for (PropertyListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onSet(t);
			}
		}
	};

	//Typically, ArgType would be T (e.g. when T=float) or const & to some native type.
	//The latter typically means that RetType is this native type. It may, occasionally, be lvalue reference to it.
	template<typename T,
		typename BlockType,
		typename NativeType = T,
		//typename ArgType = T,
		//typename RetType = typename std::remove_cv<typename std::remove_reference<T>::type>::type, 
		template<typename> typename Converter = NifConverter>
	class PropertyFcn final : public PropertyBase<T, BlockType>
	{
	public:
		PropertyFcn(BlockType& block, const std::function<T()>& get, const std::function<void(const T&)>& set) :
			PropertyBase<T, BlockType>(block), m_get{ get }, m_set{ set } {}

		//RetType may be the native type or any reference thereto.
		//ArgType may be the native type, rvalue reference to the native type or const lvalue reference to the native type.
		template<typename ObjType, typename RetType, typename ArgType, typename BaseType>
		PropertyFcn(BlockType& block, ObjType* obj, RetType(BaseType::* g)() const, void(BaseType::* s)(ArgType)) :
			PropertyBase<T, BlockType>(block),
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
		PropertyFcn(BlockType& block, ObjType* obj, RetType(ObjType::* g)(), void(ObjType::* s)(ArgType)) :
			PropertyFcn(block, obj, (RetType(ObjType::*)() const)g, s) {}
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
