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
#include "nodes_internal.h"

namespace nodes
{
	using namespace nif;

	class MockModifiable : public node::IModifiable
	{
	public:
		std::vector<NiPSysModifier*> modsAdded;
		std::vector<NiPSysModifier*> modsRemoved;

		std::vector<NiTimeController*> ctlrsAdded;
		std::vector<NiTimeController*> ctlrsRemoved;

		std::vector<node::ModRequirement> reqsAdded;
		std::vector<node::ModRequirement> reqsRemoved;

		virtual void addModifier(const ni_ptr<NiPSysModifier>& obj) override
		{
			modsAdded.push_back(obj.get());
		}
		virtual void removeModifier(NiPSysModifier* obj) override
		{
			modsRemoved.push_back(obj);
		}

		virtual void addController(const ni_ptr<NiTimeController>& obj) override
		{
			ctlrsAdded.push_back(obj.get());
		}
		virtual void removeController(NiTimeController* obj) override
		{
			ctlrsRemoved.push_back(obj);
		}

		virtual void addRequirement(node::ModRequirement req) override
		{
			reqsAdded.push_back(req);
		}
		virtual void removeRequirement(node::ModRequirement req) override
		{
			reqsRemoved.push_back(req);
		}
	};

	class TestConnector :
		public node::Receiver<void>,
		public node::Sender<node::IModifiable>,
		public gui::SingleConnector
	{
	public:
		TestConnector(node::IModifiable& ifc) :
			Sender<node::IModifiable>(ifc), SingleConnector(*this, *this) {}
	};

	template<typename T, node::ModRequirement Req>
	struct RequirementsTest
	{
		void run()
		{
			File file{ File::Version::SKYRIM_SE };
			MockModifiable target;
			TestRoot root;
			auto node = node::Default<T>{}.create(file);
			auto mod1 = node.get();
			root.addChild(std::move(node));

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (node::Field* f1 = mod1->getField(node::Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			Assert::IsTrue(target.reqsAdded.size() == 1 && target.reqsAdded[0] == Req);
			target.reqsAdded.clear();
			Assert::IsTrue(target.reqsRemoved.empty());

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.reqsAdded.empty());
			Assert::IsTrue(target.reqsRemoved.size() == 1 && target.reqsRemoved[0] == Req);
			target.reqsRemoved.clear();
		}
	};
}
