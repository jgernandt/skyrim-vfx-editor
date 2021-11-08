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
#include <vector>

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

template<typename T>
class ObservableBase final : public IObservable<T>
{
public:
	virtual ~ObservableBase() = default;

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
