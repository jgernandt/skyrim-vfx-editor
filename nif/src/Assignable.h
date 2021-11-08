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
class IListener<IAssignable<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAssign(T*) {}
};
template<typename T>
using AssignableListener = IListener<IAssignable<T>>;

namespace nif
{
	template<typename T>
	class AssignableBase : public IAssignable<T>
	{
	public:
		virtual ~AssignableBase() = default;

		virtual void addListener(AssignableListener<T>& l) final override { m_obs.addListener(l); }
		virtual void removeListener(AssignableListener<T>& l) final override { m_obs.removeListener(l); }

	protected:
		void notify(T* t)
		{
			for (AssignableListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onAssign(t);
			}
		}

	private:
		ObservableBase<IAssignable<T>> m_obs;
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
