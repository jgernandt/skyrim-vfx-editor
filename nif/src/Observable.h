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

/*class Unsubscriber
{
public:
	virtual ~Unsubscriber() = default;
	virtual void unsub() = 0;
};*/

template<typename T>
class IObservable : public T
{
public:
	virtual ~IObservable() = default;

	virtual void addListener(IListener<T>&) = 0;
	virtual void removeListener(IListener<T>&) = 0;

	//Postpone this change. It may or may not be a good idea.
	//[[nodiscard]] virtual std::unique_ptr<Unsubscriber> addListener(IListener<T>&) = 0;

};

template<typename T>
class ObservableBase : public IObservable<T>
{
private:
	/*class BaseUnsubscriber final : public Unsubscriber
	{
	public:
		BaseUnsubscriber(ObservableBase<T>& target, IListener<T>& lsnr) :
			m_target{ target }, m_lsnr{ lsnr } {}
		~BaseUnsubscriber() { unsub(); }

		virtual void unsub() override { m_target.removeListener(m_lsnr); }

	private:
		ObservableBase<T>& m_target;
		IListener<T>& m_lsnr;
	};*/
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

protected:
	const std::vector<IListener<T>*>& getListeners() const { return m_listeners; }

	/* 
public:
	[[nodiscard]] virtual std::unique_ptr<Unsubscriber> addListener(IListener<T>& l) final override
	{
		if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it == m_listeners.end()) {
			m_listeners.push_back(&l);
			return std::make_unique<BaseUnsubscriber>(*this, l);
		}
		else
			return std::unique_ptr<Unsubscriber>();
	}

private:
	void removeListener(IListener<T>& l)
	{
		if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it != m_listeners.end())
			m_listeners.erase(it);
	}*/

private:
	std::vector<IListener<T>*> m_listeners;
};

