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
#include <vector>
#include "Devices.h"
#include "nif_concepts.h"

namespace node
{
	template<typename T>
	class IReceiver : public gui::IReceiver
	{
	public:
		virtual ~IReceiver() = default;
		virtual void onConnect(T&) = 0;
		virtual void onDisconnect(T&) = 0;
	};
	template<typename T>
	class ISender : public virtual gui::ISender
	{
	public:
		virtual ~ISender() = default;
		virtual void addReceiver(IReceiver<T>& r) = 0;
		virtual void removeReceiver(IReceiver<T>& r) = 0;
	};

	template<typename T>
	class Receiver : public IReceiver<T>
	{
	public:
		virtual ~Receiver() = default;
		virtual bool canReceive(gui::ISender& s) const final override
		{
			return dynamic_cast<ISender<T>*>(&s) != nullptr;
		}
		virtual void addSender(gui::ISender& s) final override
		{
			if (ISender<T>* sender = dynamic_cast<ISender<T>*>(&s))
				sender->addReceiver(*this);
		}
		virtual void removeSender(gui::ISender& s) final override
		{
			if (ISender<T>* sender = dynamic_cast<ISender<T>*>(&s))
				sender->removeReceiver(*this);
		}
	};

	//If we know we are publishing a constant, we don't need to keep track of our subs.
	//If the node we belong is destroyed, the connector will trigger an unsubscribe call from the sub.
	template<typename T>
	class Sender : public ISender<T>
	{
	public:
		Sender(T& t) : m_ifc{ t } {}
		Sender(const Sender&) = delete;
		virtual ~Sender() = default;

		T& getSignal() { return m_ifc; }

		virtual void addReceiver(IReceiver<T>& r) override
		{
			r.onConnect(getSignal());
		}
		virtual void removeReceiver(IReceiver<T>& r) override
		{
			r.onDisconnect(getSignal());
		}

	protected:
		T& m_ifc;
	};

	template<>
	class IReceiver<void> : public gui::IReceiver
	{
	public:
		virtual ~IReceiver() = default;
	};
	template<>
	class ISender<void> : public virtual gui::ISender
	{
	public:
		virtual ~ISender() = default;
	};
	//Not really necessary, but a simplification:
	template<> class Sender<void> : public ISender<void>
	{
	public:
		virtual ~Sender() = default;
	};
	template<> class Receiver<void> : public IReceiver<void>
	{
	public:
		virtual ~Receiver() = default;
		virtual bool canReceive(gui::ISender& s) const final override
		{
			return dynamic_cast<ISender<void>*>(&s) != nullptr;
		}
		virtual void addSender(gui::ISender&) final override {}
		virtual void removeSender(gui::ISender&) final override {}
	};


	//A device that forwards signals through a sequence, but doesn't send one itself.
	//If the first device in the sequence connects to a signal, every device in 
	//the sequence receives it. If a device is connected to the end of the sequence,
	//or if a device anywhere in the sequence is disconnected, the signal (if one is connected)
	//will be sent to that device (and any subsequent) only.
	template<typename T>
	class SequentialDevice : public IReceiver<T>, public ISender<T>
	{
	public:
		virtual ~SequentialDevice() = default;

		virtual bool canReceive(gui::ISender& s) const final override
		{
			if (ISender<T>* sender = dynamic_cast<ISender<T>*>(&s))
				return sender != static_cast<const ISender<T>*>(this);
			else
				return false;
		}
		virtual void addSender(gui::ISender& s) final override
		{
			if (ISender<T>* sender = dynamic_cast<ISender<T>*>(&s)) {
				assert(!m_sndr && sender != static_cast<ISender<T>*>(this));
				m_sndr = sender;
				sender->addReceiver(*this);
			}
		}
		virtual void removeSender(gui::ISender& s) final override
		{
			if (ISender<T>* sender = dynamic_cast<ISender<T>*>(&s)) {
				assert(m_sndr == sender);
				m_sndr = nullptr;
				sender->removeReceiver(*this);
			}
		}

		virtual void onConnect(T& t) override
		{
			//override to take an action on the interface
			if (m_rcvr)
				m_rcvr->onConnect(t);
		}
		virtual void onDisconnect(T& t) override
		{
			//override to take an action on the interface
			if (m_rcvr)
				m_rcvr->onDisconnect(t);
		}

		virtual void addReceiver(IReceiver<T>& r) override
		{
			//We need a self check, since it is possible to form a cycle
			if (&r != static_cast<IReceiver<T>*>(this)) {
				//If we have no receiver, this is the next device in our sequence. We save it.
				//If we have a receiver, this call must be forwarded from further back. We do not save it.
				if (!m_rcvr)
					m_rcvr = &r;

				//Either way, we forward the call.
				if (m_sndr)
					m_sndr->addReceiver(r);
			}
		}
		virtual void removeReceiver(IReceiver<T>& r) override
		{
			//We need a self check, since it is possible to form a cycle
			if (&r != static_cast<IReceiver<T>*>(this)) {
				if (m_sndr)
					m_sndr->removeReceiver(r);

				//If this is our receiver, we clear it. If not, this call must be forwarded from further back.
				if (&r == m_rcvr)
					m_rcvr = nullptr;
			}
		}

	private:
		//could be vectors
		IReceiver<T>* m_rcvr{ nullptr };
		ISender<T>* m_sndr{ nullptr };
	};


	template<typename T>
	class AssignableReceiver : public Receiver<IAssignable<T>>
	{
	public:
		AssignableReceiver(T& obj) : m_obj{ obj } {}
		AssignableReceiver(const AssignableReceiver&) = delete;
		virtual ~AssignableReceiver() = default;

		virtual void onConnect(IAssignable<T>& ifc) override { ifc.assign(&m_obj); }
		virtual void onDisconnect(IAssignable<T>& ifc) override { ifc.assign(nullptr); }

	private:
		T& m_obj;
	};

	template<typename T>
	struct SetReceiver : Receiver<ISet<T>>
	{
		SetReceiver(T& obj) : m_obj{ obj } {}
		virtual ~SetReceiver() = default;

		virtual void onConnect(ISet<T>& ifc) override { ifc.add(m_obj); }
		virtual void onDisconnect(ISet<T>& ifc) override { ifc.remove(m_obj); }

	private:
		T& m_obj;
	};
}

#define derived_sender(T, P)								\
template<>													\
class Sender<T> : public Sender<P>							\
{															\
public:														\
	Sender(T& t) : Sender<P>(t) {}							\
	Sender(const Sender&) = delete;							\
	virtual ~Sender() = default;							\
	T& getSignal() { return static_cast<T&>(this->m_ifc); }	\
	virtual void addReceiver(IReceiver<T>& r) override		\
	{														\
		r.onConnect(getSignal());							\
	}														\
	virtual void removeReceiver(IReceiver<T>& r) override	\
	{														\
		r.onDisconnect(getSignal());						\
	}														\
}