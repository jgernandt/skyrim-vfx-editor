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
#include <random>
#include "CppUnitTest.h"
#include "ConnectionHandler.h"
#include "node_devices.h"
#include "NodeBase.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TestRoot final :
	public gui::ConnectionHandler
{
public:
	~TestRoot()
	{
		//gui::Disconnector dc;
		//accept(dc);
		//dc.execute();
	}

};

template<typename T>
T* findNode(const std::vector<std::unique_ptr<node::NodeBase>>& nodes, const nif::NiObject& obj)
{
	for (auto&& node : nodes) {
		if (T* result = dynamic_cast<T*>(node.get()); result && result->object() == obj)
			return result;
	}
	return nullptr;
}

inline bool areConnected(gui::Connector* c1, gui::Connector* c2)
{
	if (!c1 || !c2)
		return false;

	std::vector<gui::Connector*> connected = c1->getConnected();

	if (std::find(connected.begin(), connected.end(), c2) == connected.end())
		return false;

	connected = c2->getConnected();

	if (std::find(connected.begin(), connected.end(), c1) == connected.end())
		return false;

	return true;
}


template<typename NodeType>
class ConnectorTester
{
private:
	template<typename RType, typename SType>
	class TestConnector : public node::Receiver<RType>, public node::Sender<SType>, public gui::SingleConnector
	{
	public:
		TestConnector(SType& ifc) : node::Sender<SType>(ifc), SingleConnector(*this, *this) {}

		virtual void onConnect(RType& signal) override { connect_signal = &signal; }
		virtual void onDisconnect(RType& signal) override { disconnect_signal = &signal; }

		RType* connect_signal{ nullptr };
		RType* disconnect_signal{ nullptr };
	};

	template<typename SType>
	class TestConnector<void, SType> : public node::Receiver<void>, public node::Sender<SType>, public gui::SingleConnector
	{
	public:
		TestConnector(SType& ifc) : node::Sender<SType>(ifc), SingleConnector(*this, *this) {}

		void* connect_signal{ nullptr };
		void* disconnect_signal{ nullptr };
	};

	template<typename RType>
	class TestConnector<RType, void> : public node::Receiver<RType>, public node::Sender<void>, public gui::SingleConnector
	{
	public:
		TestConnector() : SingleConnector(*this, *this) {}

		virtual void onConnect(RType& signal) override { connect_signal = &signal; }
		virtual void onDisconnect(RType& signal) override { disconnect_signal = &signal; }

		RType* connect_signal{ nullptr };
		RType* disconnect_signal{ nullptr };
	};

public:
	ConnectorTester(std::unique_ptr<NodeType>&& node)
	{
		m_node = node.get();
		m_root.addChild(std::move(node));
	}

	~ConnectorTester() 
	{
		for (auto&& target : m_connectors)
			target.second->disconnect();
	}

	//Attempt to connect a Receiver<RType>, Sender<SType> to the given field.
	//If successful, sends the given target to the field and returns the interface exposed by it.
	//Tests single- or multi connectivity.
	//Multi is kind of broken, though. You need to manually call the function multiple times.
	template<typename RType, typename SType = void>
	RType* tryConnect(const std::string& field, bool multi, SType* target)
	{
		//Require unique targets
		Assert::IsTrue(m_connectors.find(target) == m_connectors.end());

		//Locate the connector
		gui::Connector* c1 = nullptr;
		if (node::Field* f = m_node ? m_node->getField(field) : nullptr)
			c1 = f->connector;
		Assert::IsNotNull(c1);

		//Make note of its current connections
		auto preConnect = c1->getConnected();

		//Add a new target and connect it
		TestConnector<RType, SType>* c2;
		if constexpr (std::is_same<SType, void>::value) {
			c2 = m_root.newChild<TestConnector<RType, SType>>();
		}
		else {
			Assert::IsNotNull(target);
			c2 = m_root.newChild<TestConnector<RType, SType>>(*target);
			m_connectors.insert({ target, c2 });
		}

		c2->onClick();
		c1->onRelease();

		//Test for success
		Assert::IsTrue(areConnected(c1, c2));

		//I wanted to assert on the returned interface, but not all devices may send one. Should they?
		/*if constexpr (!std::is_same<RType, void>::value) {
			Assert::IsTrue(c2->connect_signal != nullptr);
			Assert::IsNull(c2->disconnect_signal);
		}*/

		//Test for correct single/multi behaviour
		for (auto&& c : preConnect) {
			Assert::IsTrue(areConnected(c1, c) == multi);

			//See previous
			/*if constexpr (!std::is_same<RType, void>::value) {
				//it's safe to assume that the previous connectors are of the same type
				Assert::IsTrue((static_cast<TestConnector<RType, SType>*>(c)->disconnect_signal == nullptr) == multi);
			}*/
		}

		return c2->connect_signal;
	}

	template<typename SType>
	void disconnect(SType* target)
	{
		//Disconnect and remove the connector
		if (auto it = m_connectors.find(target); it != m_connectors.end()) {
			it->second->disconnect();
			m_root.removeChild(it->second);
			m_connectors.erase(it);
		}
	}

	NodeType* getNode() const { return m_node; }

private:
	TestRoot m_root;
	NodeType* m_node;
	std::map<void*, gui::Connector*> m_connectors;
};

namespace nodes
{
	using namespace nif;

	//Test that the node's connector responds to a signal by assigning the expected object to the sender
	template<template<typename> typename AssType, typename NodeType, typename RefType>
	void AssignableReceiverTest(std::unique_ptr<NodeType>&& node, RefType& expected, 
		const std::string& connector, bool multi)
	{
		AssType<RefType> target1;
		AssType<RefType> target2;
		ConnectorTester<NodeType> tester(std::move(node));

		tester.tryConnect<void, AssType<RefType>>(connector, multi, &target1);
		tester.tryConnect<void, AssType<RefType>>(connector, multi, &target2);

		Assert::IsTrue((target1.assigned() == &expected) == multi);
		Assert::IsTrue(target2.assigned() == &expected);

		if (multi) {
			tester.disconnect<AssType<RefType>>(&target1);
			Assert::IsFalse(target1.assigned() == &expected);
		}

		tester.disconnect<AssType<RefType>>(&target2);
		Assert::IsFalse(target2.assigned() == &expected);
	}

	//Test that the node exposes the expected Assignable through the given connector
	template<template<typename> typename AssType, typename NodeType, typename RefType>
	void AssignableSenderTest(std::unique_ptr<NodeType>&& node, AssType<RefType>& expected,
		const std::string& connector, bool multi)
	{
		ConnectorTester<NodeType> tester(std::move(node));
		tester.tryConnect<AssType<RefType>, void>(connector, multi, nullptr);
		auto ifc = tester.tryConnect<AssType<RefType>, void>(connector, multi, nullptr);
		Assert::IsTrue(ifc == &expected);
	}

	//Test that the node's connector responds to a signal by adding the expected object to the sender
	template<typename NodeType, typename ElementType>
	void SetReceiverTest(std::unique_ptr<NodeType>&& node, ElementType& expected, 
		const std::string& connector, bool multi)
	{
		Set<ElementType> target1;
		Set<ElementType> target2;
		ConnectorTester<NodeType> tester(std::move(node));

		tester.tryConnect<void, Set<ElementType>>(connector, multi, &target1);
		tester.tryConnect<void, Set<ElementType>>(connector, multi, &target2);

		Assert::IsTrue(target1.has(&expected) == multi);
		Assert::IsTrue(target2.has(&expected));

		if (multi) {
			tester.disconnect<Set<ElementType>>(&target1);
			Assert::IsFalse(target1.has(&expected));
		}

		tester.disconnect<Set<ElementType>>(&target2);
		Assert::IsFalse(target2.has(&expected));
	}

	//Test that the node exposes the expected Set through the given connector
	template<typename NodeType, typename ElementType>
	void SetSenderTest(std::unique_ptr<NodeType>&& node, Set<ElementType>& expected,
		const std::string& connector, bool multi)
	{
		ConnectorTester<NodeType> tester(std::move(node));
		tester.tryConnect<Set<ElementType>, void>(connector, multi, nullptr);
		auto ifc = tester.tryConnect<Set<ElementType>, void>(connector, multi, nullptr);
		Assert::IsTrue(ifc == &expected);
	}

	template<typename T>
	void ControllableTest(std::unique_ptr<node::NodeBase>&& node, NiSingleInterpController* ctlr, const char* connector, nif::File& file)
	{
		class MockController : public node::IController<float>
		{
		public:
			virtual FlagSet<ControllerFlags>& flags() override { return m_flags; }
			virtual Property<float>& frequency() override { return m_frequency; }
			virtual Property<float>& phase() override { return m_phase; }
			virtual Property<float>& startTime() override { return m_startTime; }
			virtual Property<float>& stopTime() override { return m_stopTime; }

		private:
			FlagSet<ControllerFlags> m_flags;
			Property<float> m_frequency;
			Property<float> m_phase;
			Property<float> m_startTime;
			Property<float> m_stopTime;
		};

		std::mt19937 rng;

		MockController target0;
		MockController target;
		ConnectorTester<node::NodeBase> tester(std::move(node));

		tester.tryConnect<node::IControllable, node::IController<T>>(connector, false, &target0);
		auto ifc = tester.tryConnect<node::IControllable, node::IController<T>>(connector, false, &target);
		Assert::IsNotNull(ifc);

		//Setting the properties on target (but not target0) should set the corresponding on ctlr
		std::uniform_int_distribution<unsigned short> I;
		std::uniform_real_distribution<float> F;

		target0.flags().clear(-1);//better clear any defaults
		target0.flags().raise(I(rng));
		target0.frequency().set(F(rng));
		target0.phase().set(F(rng));
		target0.startTime().set(F(rng));
		target0.stopTime().set(F(rng));
		Assert::IsFalse(ctlr->flags.raised() == target0.flags().raised());
		Assert::IsFalse(ctlr->frequency.get() == target0.frequency().get());
		Assert::IsFalse(ctlr->phase.get() == target0.phase().get());
		Assert::IsFalse(ctlr->startTime.get() == target0.startTime().get());
		Assert::IsFalse(ctlr->stopTime.get() == target0.stopTime().get());

		target.flags().clear(-1);//better clear any defaults
		target.flags().raise(I(rng));
		target.frequency().set(F(rng));
		target.phase().set(F(rng));
		target.startTime().set(F(rng));
		target.stopTime().set(F(rng));
		Assert::IsTrue(ctlr->flags.raised() == target.flags().raised());
		Assert::IsTrue(ctlr->frequency.get() == target.frequency().get());
		Assert::IsTrue(ctlr->phase.get() == target.phase().get());
		Assert::IsTrue(ctlr->startTime.get() == target.startTime().get());
		Assert::IsTrue(ctlr->stopTime.get() == target.stopTime().get());

		//Assigning to the interface should assign to ctlr->interpolator
		auto iplr = file.create<NiInterpolator>();
		Assert::IsNotNull(iplr.get());
		ifc->iplr().assign(iplr);
		Assert::IsTrue(ctlr->interpolator.assigned() == iplr);
		ifc->iplr().assign(nullptr);
		Assert::IsTrue(ctlr->interpolator.assigned() != iplr);

		//TODO: Test the other fields of the interface (no point until we start on nonlinear animations)

		//Make sure listeners are removed
		tester.disconnect<node::IController<float>>(&target);

		//these calls should not reach the controller
		target.flags().clear(-1);
		target.flags().raise(I(rng));
		target.frequency().set(F(rng));
		target.phase().set(F(rng));
		target.startTime().set(F(rng));
		target.stopTime().set(F(rng));
		Assert::IsTrue(ctlr->flags.raised() != target.flags().raised());
		Assert::IsTrue(ctlr->frequency.get() != target.frequency().get());
		Assert::IsTrue(ctlr->phase.get() != target.phase().get());
		Assert::IsTrue(ctlr->startTime.get() != target.startTime().get());
		Assert::IsTrue(ctlr->stopTime.get() != target.stopTime().get());
	}
}

