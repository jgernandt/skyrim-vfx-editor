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

namespace nif
{
	template<typename T> class Assignable;

	template<typename T>
	struct Event<Assignable<T>>
	{
		T* obj{ nullptr };
	};

	template<typename T>
	class IListener<Assignable<T>>
	{
	public:
		virtual ~IListener() = default;
		void receive(const Event<Assignable<T>>&e, Observable<Assignable<T>>&)
		{ 
			onAssign(e.obj); 
		}
		virtual void onAssign(T*) {}
	};

	template<typename T> using AssignableListener = IListener<Assignable<T>>;

	template<typename T>
	class Assignable : public Observable<Assignable<T>>
	{
	public:
		using ref_type = T;
	};
	
	template<typename T>
	class Ref final : public Assignable<T>
	{
	public:
		Ref(const std::shared_ptr<T>& t = std::shared_ptr<T>()) : m_assigned{ t } {}
		~Ref()
		{
			if (m_assigned) {
				this->signal(Event<Assignable<T>>{ nullptr });
			}
		}

		void assign(const std::shared_ptr<T>& t)
		{
			if (m_assigned != t) {
				m_assigned = t;
				this->signal(Event<Assignable<T>>{ m_assigned.get() });
			}
		}

		const std::shared_ptr<T>& assigned() const noexcept
		{
			return m_assigned;
		}

	private:
		std::shared_ptr<T> m_assigned;
	};

	template<typename T>
	class Ptr final : public Assignable<T>
	{
	public:
		Ptr(const std::shared_ptr<T>& t = std::shared_ptr<T>()) : m_assigned{ t } {}
		~Ptr()
		{
			//Unlike Ref, we always signal null here. If we were to only signal
			//if we have a resource, we would not signal in a situation where our resource
			//has expired. A listener would probably be interested in hearing that.
			this->signal(Event<Assignable<T>>{ nullptr });
		}

		void assign(const std::shared_ptr<T>& t)
		{
			if (m_assigned.lock() != t) {
				m_assigned = t;
				this->signal(Event<Assignable<T>>{ t.get() });
			}
		}

		std::shared_ptr<T> assigned() const noexcept
		{
			return m_assigned.lock();
		}

	private:
		std::weak_ptr<T> m_assigned;
	};
}
