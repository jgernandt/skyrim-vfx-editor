//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "Devices.h"
#include "Composition.h"
#include "Visitor.h"

namespace gui
{
	class IPlacementController
	{
	public:
		virtual ~IPlacementController() {}
		virtual Floats<2> place(const Floats<2>& hint) = 0;
	};

	class ConnectionHandler;

	class Connector : public Component
	{
	public:
		using StateMap = std::map<std::pair<Connector*, Connector*>, bool>;

	public:
		Connector(ISender& sender, IReceiver& receiver);
		virtual ~Connector() {}

		virtual void accept(Visitor& v);
		virtual void frame() override;
		virtual Floats<2> getSizeHint() const override;

		virtual void onClick() = 0;
		virtual void onRelease();

		virtual bool isConnected() const = 0;//used to switch displays
		virtual std::vector<Connector*> getConnected() const = 0;

		//Return the set of state changes that would result from a call to setConnectionState.
		//Typically, may include disconnection of a third part.
		//Required to leave us unchanged, but may include non-const this in the return.
		virtual StateMap queryStateChange(Connector*, bool) = 0;

		virtual void setConnectionState(Connector*, bool) = 0;

		//Disconnect us from everything and return the complete set of state changes that resulted.
		virtual StateMap disconnect() = 0;

		ISender& getSender() const { return m_sender; }
		IReceiver& getReceiver() const { return m_receiver; }
		void setController(IPlacementController* ctlr) { m_ctlr = ctlr; }

	protected:
		ConnectionHandler* getHandler() const;

	protected:
		ISender& m_sender;
		IReceiver& m_receiver;

	private:
		IPlacementController* m_ctlr{ nullptr };
	};

	//Should merge source into target, and erase entries that undo one another
	void merge(Connector::StateMap& target, Connector::StateMap&& source);

	class SingleConnector :	public Connector
	{
	public:
		//NOTE: for the moment, it is assumed that either pins or sockets is empty. This can be chaned if we need.
		SingleConnector(ISender& sender, IReceiver& receiver) : Connector(sender, receiver) {}
		virtual ~SingleConnector();

		virtual bool isConnected() const override { return m_connected; }
		virtual void onClick() override;
		virtual std::vector<Connector*> getConnected() const override;

		virtual StateMap queryStateChange(Connector* c, bool state) override;
		virtual void setConnectionState(Connector* c, bool state) override;
		virtual StateMap disconnect() override;

	protected:
		Connector* m_connected{ nullptr };
	};

	class MultiConnector : public Connector
	{
	public:
		MultiConnector(ISender& sender, IReceiver& receiver) : Connector(sender, receiver) {}
		virtual ~MultiConnector();
		virtual bool isConnected() const override { return !m_connected.empty(); }
		virtual void onClick() override;
		virtual std::vector<Connector*> getConnected() const override;

		virtual StateMap queryStateChange(Connector* c, bool state) override;
		virtual void setConnectionState(Connector* c, bool state) override;
		virtual StateMap disconnect() override;

	protected:
		std::set<Connector*> m_connected;
	};

	class Disconnector final : public DescendingVisitor
	{
	public:
		virtual void visit(Connector& c) override;
		void execute();
		const Connector::StateMap& getStateChanges() const { return m_stateChanges; }
		Connector::StateMap takeStateChanges() { return std::move(m_stateChanges); }

	private:
		Connector::StateMap m_stateChanges;
	};


	//This whole Connector and ConnectionHandler system is a mess. We should try a typed system sometime:
	template<typename IType, typename OType>
	class TypedConnector : public Component
	{
	public:
		//TypedConnector<I, O> would only look to be connected to TypedConnector<O, I>:
		virtual void connect(TypedConnector<OType, IType>*) = 0;
	};
}

