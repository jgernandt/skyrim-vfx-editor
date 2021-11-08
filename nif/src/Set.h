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
class IListener<ISet<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onAdd(const T&) {}
	virtual void onRemove(const T&) {}
};
template<typename T>
using SetListener = IListener<ISet<T>>;

namespace nif
{
	template<typename T>
	class SetBase : public ISet<T>
	{
	public:
		virtual ~SetBase() = default;

		virtual void addListener(SetListener<T>& l) final override { m_obs.addListener(l); }
		virtual void removeListener(SetListener<T>& l) final override { m_obs.removeListener(l); }

	protected:
		void notifyAdd(const T& t) const
		{
			for (SetListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onAdd(t);
			}
		}
		void notifyRemove(const T& t) const
		{
			for (SetListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onRemove(t);
			}
		}

	private:
		ObservableBase<ISet<T>> m_obs;
	};
}
