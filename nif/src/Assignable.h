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
class IAssignable
{
public:
	virtual ~IAssignable() = default;

	virtual void assign(T*) = 0;
	virtual bool isAssigned(T*) const = 0;
};

template<typename T>
using OAssignable = IObservable<IAssignable<T>>;

template<typename T>
class IListener<IAssignable<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAssign(T*) {}
};

namespace nif
{
	template<typename T>
	using Assignable = NiObject::DataField<IAssignable<T>>;

	template<typename T>
	using AssignableListener = IListener<IAssignable<T>>;

	template<typename T, typename BlockType>
	class AssignableBase : public Assignable<T>
	{
	public:
		AssignableBase(BlockType& block) : Assignable<T>(block) {}
		virtual ~AssignableBase() = default;

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

		void notify(T* t)
		{
			for (AssignableListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onAssign(t);
			}
		}
	};

	template<typename T, typename BlockType>
	class AssignableFcn final : public AssignableBase<T, BlockType>
	{
	public:
		using native_type = typename T::native_type;
		//using native_type = typename std::remove_reference<decltype(std::declval<T>().getNative())>::type;

	public:
		//RetType and ArgType here may be Niflib::Ref<native_type>, or a pointer to some type related to native_type
		template<typename ObjType, typename RetType, typename BaseType>
		AssignableFcn(BlockType& block, ObjType* obj, RetType(BaseType::* g)() const, void(BaseType::* s)(native_type*)) :
			AssignableBase<T, BlockType>(block),
			m_get{ std::bind(g, obj) },
			m_set{ std::bind(s, obj, std::placeholders::_1) }
		{
			static_assert(std::is_base_of<BaseType, ObjType>::value);
			assert(obj&& g&& s);
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
}
