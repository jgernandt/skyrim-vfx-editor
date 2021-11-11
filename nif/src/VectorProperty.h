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

template<typename T>
class IVectorProperty : public IObservable<IVectorProperty<T>>
{
public:
	using container = std::vector<T>;
	class element;
	//class iterator;
public:
	virtual ~IVectorProperty() = default;

	//virtual iterator begin() = 0;
	//virtual iterator end() = 0;

	virtual element at(int) = 0;

	//Regular property functions
	virtual container get() const = 0;
	virtual void set(const container&) = 0;

	//Allow random access
	virtual T get(int) const = 0;
	virtual void set(int, const T&) = 0;

	//Allow insertions/deletions
	virtual int insert(int, const T&) = 0;
	virtual int erase(int) = 0;
};

template<typename T>
class IListener<IVectorProperty<T>>
{
public:
	virtual ~IListener() = default;

	//Should send insert/erase instead, will be more predictable
	//virtual void onSet(const typename IVectorProperty<T>::container&) {}
	virtual void onSet(int, const T&) {}
	virtual void onInsert(int) {}
	virtual void onErase(int) {}
	virtual void onDestroy() {}
};
template<typename T>
using VectorPropertyListener = IListener<IVectorProperty<T>>;

template<typename T>
class IVectorProperty<T>::element final : public PropertyBase<T>, public VectorPropertyListener<T>
{
public:
	element(IVectorProperty<T>& list, int index) : m_list{ &list }, m_index{ index }
	{
		if (valid())
			list.addListener(*this);
	}
	element(const element& other) { *this = other; }
	~element() { invalidate(); }

	element& operator=(const element& other)
	{
		m_list = other.m_list;
		m_index = other.m_index;
		if (valid()) {
			assert(m_list);
			m_list->addListener(*this);
		}
		return *this;
	}

	virtual T get() const override
	{
		assert(valid() && m_list);
		return m_list->get(m_index);
	}
	virtual void set(const T & t) override
	{
		if (valid()) {
			assert(m_list);
			m_list->set(m_index, t);
		}
	}

	/*Should send insert/erase instead, will be more predictable
	virtual void onSet(const std::vector<T>& list) override
	{
		if (valid() && static_cast<size_t>(m_index) < list.size())
			this->notify(list[m_index]);//we don't know if our element actually changed
		else
			invalidate();

		//Or should this always invalidate us? 
		//Techically, the whole list has been replaced. Our element no longer exists.
	}*/
	virtual void onSet(int i, const T & t) override
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
	IVectorProperty<T>* m_list;
	int m_index;
};

namespace nif
{
	template<typename T>
	class VectorPropertyBase : public IVectorProperty<T>
	{
	public:
		virtual ~VectorPropertyBase()
		{
			for (VectorPropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onDestroy();
			}
		}

		virtual void addListener(VectorPropertyListener<T>& l) final override
		{
			m_obs.addListener(l);
		}
		virtual void removeListener(VectorPropertyListener<T>& l) final override
		{
			m_obs.removeListener(l);
		}

	protected:
		/*Should send insert/erase instead, will be more predictable
		void notifySet(const typename IVectorProperty<T>::container& t) const
		{
			for (VectorPropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onSet(t);
			}
		}*/
		void notifySet(int i, const T& t) const
		{
			for (VectorPropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onSet(i, t);
			}
		}
		void notifyInsert(int i) const
		{
			for (VectorPropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onInsert(i);
			}
		}
		void notifyErase(int i) const
		{
			for (VectorPropertyListener<T>* l : m_obs.getListeners()) {
				assert(l);
				l->onErase(i);
			}
		}

	private:
		ObservableBase<IVectorProperty<T>> m_obs;
	};
}
