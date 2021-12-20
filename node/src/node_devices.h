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
#include "nif_types.h"
#include "node_concepts.h"

namespace node
{
	template<typename Signal>
	class IReceiver : public gui::IReceiver
	{
	public:
		virtual ~IReceiver() = default;
		virtual void onConnect(Signal&) = 0;
		virtual void onDisconnect(Signal&) = 0;
	};
	template<typename Signal>
	class ISender : public virtual gui::ISender
	{
	public:
		virtual ~ISender() = default;
		virtual void addReceiver(IReceiver<Signal>& r) = 0;
		virtual void removeReceiver(IReceiver<Signal>& r) = 0;
	};

	template<typename Signal>
	class Receiver : public IReceiver<Signal>
	{
	public:
		virtual ~Receiver() = default;
		virtual bool canReceive(gui::ISender& s) const final override
		{
			return dynamic_cast<ISender<Signal>*>(&s) != nullptr;
		}
		virtual void addSender(gui::ISender& s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s))
				sender->addReceiver(*this);
		}
		virtual void removeSender(gui::ISender& s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s))
				sender->removeReceiver(*this);
		}
	};

	//If we know we are publishing a constant, we don't need to keep track of our subs.
	//If the node we belong is destroyed, the connector will trigger an unsubscribe call from the sub.
	template<typename Signal>
	class Sender : public ISender<Signal>
	{
	public:
		Sender(Signal& t) : m_signal{ t } {}
		Sender(const Sender&) = delete;
		virtual ~Sender() = default;

		Signal& getSignal() { return m_signal; }

		virtual void addReceiver(IReceiver<Signal>& r) override
		{
			r.onConnect(getSignal());
		}
		virtual void removeReceiver(IReceiver<Signal>& r) override
		{
			r.onDisconnect(getSignal());
		}

	protected:
		Signal& m_signal;
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
	template<typename Signal>
	class SequentialReceiver : public IReceiver<Signal>, public ISender<Signal>
	{
	public:
		virtual ~SequentialReceiver() = default;

		virtual bool canReceive(gui::ISender& s) const final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s))
				return sender != static_cast<const ISender<Signal>*>(this);
			else
				return false;
		}
		virtual void addSender(gui::ISender& s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s)) {
				assert(!m_sndr && sender != static_cast<ISender<Signal>*>(this));
				m_sndr = sender;
				sender->addReceiver(*this);
			}
		}
		virtual void removeSender(gui::ISender& s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s)) {
				assert(m_sndr == sender);
				m_sndr = nullptr;
				sender->removeReceiver(*this);
			}
		}

		virtual void onConnect(Signal& t) override
		{
			//override to take an action on the interface
			if (m_rcvr)
				m_rcvr->onConnect(t);
		}
		virtual void onDisconnect(Signal& t) override
		{
			//override to take an action on the interface
			if (m_rcvr)
				m_rcvr->onDisconnect(t);
		}

		virtual void addReceiver(IReceiver<Signal>& r) override
		{
			//We need a self check, since it is possible to form a cycle
			if (&r != static_cast<IReceiver<Signal>*>(this)) {
				//If we have no receiver, this is the next device in our sequence. We save it.
				//If we have a receiver, this call must be forwarded from further back. We do not save it.
				if (!m_rcvr)
					m_rcvr = &r;

				//Either way, we forward the call.
				if (m_sndr)
					m_sndr->addReceiver(r);
			}
		}
		virtual void removeReceiver(IReceiver<Signal>& r) override
		{
			//We need a self check, since it is possible to form a cycle
			if (&r != static_cast<IReceiver<Signal>*>(this)) {
				if (m_sndr)
					m_sndr->removeReceiver(r);

				//If this is our receiver, we clear it. If not, this call must be forwarded from further back.
				if (&r == m_rcvr)
					m_rcvr = nullptr;
			}
		}

	private:
		//could be vectors
		IReceiver<Signal>* m_rcvr{ nullptr };
		ISender<Signal>* m_sndr{ nullptr };
	};

	//Sends a signal to any connected receiver and forwards the connection request
	template<typename Signal>
	class SequentialSender : public IReceiver<Signal>, public ISender<Signal>
	{
	public:
		SequentialSender(Signal& signal) : m_signal{ signal } {}
		virtual ~SequentialSender() = default;

		virtual bool canReceive(gui::ISender & s) const final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s))
				return sender != static_cast<const ISender<Signal>*>(this);
			else
				return false;
		}
		virtual void addSender(gui::ISender & s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s)) {
				//We need a self check, since it is possible to form a cycle
				if (sender != static_cast<ISender<Signal>*>(this)) {
					//If we have no sender, let them connect to us
					if (!m_sndr) {
						m_sndr = sender;
						sender->addReceiver(*this);
					}
					//else this must be forwarded from further up front

					//either way, forward to our receiver
					if (m_rcvr)
						m_rcvr->addSender(s);
				}
			}
		}
		virtual void removeSender(gui::ISender & s) final override
		{
			if (ISender<Signal>* sender = dynamic_cast<ISender<Signal>*>(&s)) {
				//We need a self check, since it is possible to form a cycle
				if (sender != static_cast<ISender<Signal>*>(this)) {
					if (m_rcvr)
						m_rcvr->removeSender(s);

					if (sender == m_sndr) {
						sender->removeReceiver(*this);
						m_sndr = nullptr;
					}
				}
			}
		}

		virtual void onConnect(Signal&) override
		{
			//do nothing
		}
		virtual void onDisconnect(Signal&) override {}

		virtual void addReceiver(IReceiver<Signal>& r) override
		{
			if (&r != this) {
				if (!m_rcvr)
					m_rcvr = &r;

				r.onConnect(m_signal);

				if (m_sndr)
					m_sndr->addReceiver(r);
			}
		}
		virtual void removeReceiver(IReceiver<Signal>& r) override
		{
			if (&r != this) {
				if (m_sndr)
					m_sndr->removeReceiver(r);

				r.onDisconnect(m_signal);

				if (m_rcvr == &r)
					m_rcvr = nullptr;
			}
		}
	private:
		Signal& m_signal;
		IReceiver<Signal>* m_rcvr{ nullptr };
		ISender<Signal>* m_sndr{ nullptr };
	};


	template<typename T>
	class RefReceiver : public Receiver<nif::Ref<T>>
	{
	public:
		RefReceiver(const nif::ni_ptr<T>& obj) : m_obj{ obj } {}
		RefReceiver(const RefReceiver&) = delete;
		virtual ~RefReceiver() = default;
		RefReceiver& operator=(const RefReceiver&) = delete;

		virtual void onConnect(nif::Ref<T>& ifc) override { ifc.assign(m_obj); }
		virtual void onDisconnect(nif::Ref<T>& ifc) override { ifc.assign(nullptr); }

	private:
		nif::ni_ptr<T> m_obj;
	};
	template<typename T>
	class PtrReceiver : public Receiver<nif::Ptr<T>>
	{
	public:
		PtrReceiver(const nif::ni_ptr<T>& obj) : m_obj{ obj } {}
		PtrReceiver(const PtrReceiver&) = delete;
		virtual ~PtrReceiver() = default;
		PtrReceiver& operator=(const PtrReceiver&) = delete;

		virtual void onConnect(nif::Ptr<T>& ifc) override { ifc.assign(m_obj); }
		virtual void onDisconnect(nif::Ptr<T>& ifc) override { ifc.assign(nullptr); }

	private:
		nif::ni_ptr<T> m_obj;
	};

	template<typename T>
	struct SetReceiver : Receiver<nif::Set<T>>
	{
		SetReceiver(const nif::ni_ptr<T>& obj) : m_obj{ obj } {}
		SetReceiver(const SetReceiver&) = delete;
		virtual ~SetReceiver() = default;
		SetReceiver& operator=(const SetReceiver&) = delete;

		virtual void onConnect(nif::Set<T>& ifc) override { ifc.add(m_obj); }
		virtual void onDisconnect(nif::Set<T>& ifc) override { ifc.remove(m_obj.get()); }

	private:
		nif::ni_ptr<T> m_obj;
	};


	class FloatCtlrReceiver final : public Receiver<IController<float>>
	{
	public:
		FloatCtlrReceiver();
		FloatCtlrReceiver(const ni_ptr<NiTimeController>& ctlr);

		virtual void onConnect(IController<float>& ifc) override;
		virtual void onDisconnect(IController<float>& ifc) override;

		void setController(const ni_ptr<NiTimeController>& ctlr);

	private:
		FlagSetSyncer<ControllerFlags> m_lFlags;
		PropertySyncer<float> m_lFrequency;
		PropertySyncer<float> m_lPhase;
		PropertySyncer<float> m_lStartTime;
		PropertySyncer<float> m_lStopTime;

		IController<float>* m_ifc{ nullptr };
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