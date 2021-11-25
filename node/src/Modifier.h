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
#include "NodeBase.h"

namespace node
{
	using namespace nif;

	class IModifiable;

	class Modifier : public NodeBase
	{
	protected:
		class Device;

	protected:
		Modifier(const ni_ptr<NiPSysModifier>& obj);

	public:
		virtual ~Modifier();

		void addController(const ni_ptr<NiPSysModifierCtlr>& ctlr);

	public:
		constexpr static const char* TARGET = "Target";
		constexpr static const char* NEXT_MODIFIER = "Next modifier";

		//Updates modifier name to match its order
		class NameUpdater : public PropertyListener<unsigned int>
		{
		public:
			NameUpdater(ni_ptr<Property<std::string>>&& name);

			virtual void onSet(const unsigned int& i) override;

		private:
			const ni_ptr<Property<std::string>> m_name;
		};

	protected:
		class Device final : public SequentialDevice<IModifiable>
		{
		public:
			Device(const ni_ptr<NiPSysModifier>& obj);

			virtual void onConnect(IModifiable& ifc) override;
			virtual void onDisconnect(IModifiable& ifc) override;

			//Add a controller to our connected interface (current or future).
			//Also registers the controller to receive name changes.
			void addController(const ni_ptr<NiPSysModifierCtlr>& ctlr);
			void removeController(NiPSysModifierCtlr* ctlr);

			//Add a requirement to our connected interface (current or future).
			void addRequirement(ModRequirement req);
			void removeRequirement(ModRequirement req);

		private:
			const ni_ptr<NiPSysModifier> m_mod;

			//Listens to our connected sequence and keeps our order correct
			//OrderUpdater m_orderUpdater;

			//Controllers and requirements may change dynamically
			using ControllerPair = std::pair<ni_ptr<NiPSysModifierCtlr>, std::unique_ptr<PropertySyncer<std::string>>>;
			std::vector<ControllerPair> m_ctlrs;
			std::map<ModRequirement, int> m_reqs;

			//Our connected interface
			IModifiable* m_ifc{ nullptr };
		};

		class TargetField final : public Field
		{
		public:
			TargetField(const std::string& name, Modifier& node, Device& rcvr);

		private:
			Device& m_rcvr;
			Sender<void> m_sndr;
		};

		class NextModField final : public Field
		{
		public:
			NextModField(const std::string& name, Modifier& node, Device& sndr);

		private:
			Device& m_sndr;
			Receiver<void> m_rcvr;
		};

	protected:
		Device m_device;

	private:
		//Name should always match our order, to ensure local uniqueness
		NameUpdater m_nameUpdater;

		std::unique_ptr<Field> m_targetField;
		std::unique_ptr<Field> m_nextField;
	};


	class DummyModifier final : public Modifier
	{
	public:
		using default_object = NiPSysModifier;

		DummyModifier(const ni_ptr<NiPSysModifier>& obj);
		~DummyModifier();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 80.0f;
	};
}
