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
#include "Property.h"

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

namespace nif
{
	template<typename T, typename ContainerType = std::vector<T>>
	class ListPropertyBase : public IListProperty<T, ContainerType>
	{
	public:
		virtual ~ListPropertyBase()
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obs.getListeners()) {
				assert(l);
				l->onDestroy();
			}
		}

		virtual void addListener(ListPropertyListener<T, ContainerType>& l) final override
		{
			m_obs.addListener(l);
		}
		virtual void removeListener(ListPropertyListener<T, ContainerType>& l) final override
		{
			m_obs.removeListener(l);
		}

	protected:
		void notifySet(const ContainerType& t) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obs.getListeners()) {
				assert(l);
				l->onSet(t);
			}
		}
		void notifySet(int i, const T& t) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obs.getListeners()) {
				assert(l);
				l->onSet(i, t);
			}
		}
		void notifyInsert(int i) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obs.getListeners()) {
				assert(l);
				l->onInsert(i);
			}
		}
		void notifyErase(int i) const
		{
			for (ListPropertyListener<T, ContainerType>* l : m_obs.getListeners()) {
				assert(l);
				l->onErase(i);
			}
		}

	private:
		ObservableBase<IListProperty<T, ContainerType>> m_obs;
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
}
