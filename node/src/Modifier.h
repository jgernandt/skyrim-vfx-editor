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
#include "node_concepts.h"
#include "node_traits.h"
#include "DeviceImpl.h"
#include "NodeBase.h"
#include "NiPSysModifier.h"

namespace node
{
	class IModifiable;

	class Modifier : public NodeBase
	{
	protected:
		class Device;

	public:
		enum class Requirement
		{
			NONE,
			COLOUR,
			LIFETIME,
			MOVEMENT,
			ROTATION,
			UPDATE,//Not really a modifier requirement
		};

	protected:
		Modifier(ni_ptr<nif::NiPSysModifier>&& obj);

	public:
		virtual ~Modifier();
		virtual nif::NiPSysModifier& object() override;

		void addUnknownController(ni_ptr<nif::NiPSysModifierCtlr>&& ctlr);

	public:
		constexpr static const char* TARGET = "Target";
		constexpr static const char* NEXT_MODIFIER = "Next modifier";

		//Updates modifier order to match position in the sequence
		class OrderUpdater : public nif::SequenceListener<nif::NiPSysModifier>
		{
		public:
			OrderUpdater(ni_ptr<IProperty<unsigned int>>&& order);

			virtual void onInsert(size_t pos) override;
			virtual void onErase(size_t pos) override;

		private:
			const ni_ptr<IProperty<unsigned int>> m_order;
		};

		//Updates modifier name to match its order
		class NameUpdater : public nif::PropertyListener<unsigned int>
		{
		public:
			NameUpdater(ni_ptr<IProperty<std::string>>&& name);

			virtual void onSet(const unsigned int& i) override;

		private:
			const ni_ptr<IProperty<std::string>> m_name;
		};

		//Updates one string to match another string (useful for the modifier name on a NiPSysModifierCtlr)
		class StringMatcher : public nif::PropertyListener<std::string>
		{
		public:
			StringMatcher(ni_ptr<IProperty<std::string>>&& target);
			virtual void onSet(const std::string& s) override { m_target->set(s); }

		private:
			const ni_ptr<IProperty<std::string>> m_target;
		};

	protected:
		class Device final : public SequentialDevice<IModifiable>
		{
		public:
			Device(const ni_ptr<nif::NiPSysModifier>& obj);

			virtual void onConnect(IModifiable& ifc) override;
			virtual void onDisconnect(IModifiable& ifc) override;

			//Add a controller to our connected interface (current or future).
			//Also registers the controller to receive name changes.
			void addController(const ni_ptr<nif::NiPSysModifierCtlr>& ctlr);
			void removeController(nif::NiPSysModifierCtlr* ctlr);

			//Add a requirement to our connected interface (current or future).
			void addRequirement(Requirement req);
			void removeRequirement(Requirement req);

		private:
			const ni_ptr<nif::NiPSysModifier> m_mod;

			//Listens to our connected sequence and keeps our order correct
			OrderUpdater m_orderUpdater;

			//Controllers and requirements may change dynamically
			using ControllerPair = std::pair<ni_ptr<nif::NiPSysModifierCtlr>, std::unique_ptr<StringMatcher>>;
			std::vector<ControllerPair> m_ctlrs;
			std::map<Requirement, int> m_reqs;

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
		const ni_ptr<nif::NiPSysModifier> m_obj;
		Device m_device;

	private:
		//Name should always match our order, to ensure local uniqueness
		NameUpdater m_nameUpdater;

		//We will manage any unknown controller
		std::vector<ni_ptr<nif::NiPSysModifierCtlr>> m_unknownCtlrs;

		std::unique_ptr<Field> m_targetField;
		std::unique_ptr<Field> m_nextField;
	};

	class IModifiable
	{
	public:
		virtual ~IModifiable() = default;

		virtual IObservable<ISequence<nif::NiPSysModifier>>& modifiers() = 0;
		virtual IObservable<ISequence<nif::NiTimeController>>& controllers() = 0;
		virtual IObservable<ISet<Modifier::Requirement>>& requirements() = 0;

		//When connecting a mod: 
		//*insert the modifier
		//*insert the controller, if any
		//*specify any requirements
		//*set your own order from the return of insert. This way it will reach listeners.
		//*listen to the sequence to learn about order changes
	};


	class DummyModifier final : public Modifier
	{
	public:
		DummyModifier(ni_ptr<nif::NiPSysModifier>&& obj);
		~DummyModifier();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 80.0f;
	};
}
