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
	class IListener<Assignable<T>>
	{
	public:
		virtual ~IListener() = default;
		virtual void onAssign(T*) {}
	};

	template<typename T> using AssignableListener = IListener<Assignable<T>>;

	//Do we want to distinguish between upwards and downwards references?
	//Will we get in trouble for keeping owning pointers upwards in the hierarchy?
	//We'll see.
	template<typename T>
	class Assignable final : public Observable<Assignable<T>>
	{
	public:
		Assignable(const std::shared_ptr<T>& t = std::shared_ptr<T>()) : m_assigned{ t } {}
		~Assignable()
		{
			if (m_assigned) {
				for (AssignableListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onAssign(nullptr);
				}
			}
		}

		void assign(const std::shared_ptr<T>& t)
		{
			if (m_assigned != t) {
				m_assigned = t;
				for (AssignableListener<T>* l : this->m_lsnrs) {
					assert(l);
					l->onAssign(t.get());
				}
			}
		}

		const std::shared_ptr<T>& assigned() const noexcept
		{
			return m_assigned;
		}

	private:
		std::shared_ptr<T> m_assigned;
	};
}
