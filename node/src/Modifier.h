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
		Modifier(std::unique_ptr<nif::NiPSysModifier>&& obj);

	public:
		virtual ~Modifier();
		virtual nif::NiPSysModifier& object() override;

		void addUnknownController(std::unique_ptr<nif::NiPSysModifierCtlr>&& ctlr);

	protected:
		void addTargetField(std::shared_ptr<Device>&& device);

	public:
		constexpr static const char* TARGET = "Target";
		constexpr static const char* NEXT_MODIFIER = "Next modifier";

		//Updates modifier order to match position in the sequence
		class OrderListener : public SequenceListener<nif::NiPSysModifier>
		{
		public:
			OrderListener(IProperty<unsigned int>& order) :
				m_order{ order } {}

			virtual void onInsert(const ISequence<nif::NiPSysModifier>&, size_t pos) override;
			virtual void onErase(const ISequence<nif::NiPSysModifier>&, size_t pos) override;

		private:
			IProperty<unsigned int>& m_order;
		};

		//Updates modifier name to match its order
		class NameListener : public PropertyListener<unsigned int>
		{
		public:
			NameListener(IProperty<std::string>& name) :
				m_name{ name } {}

			virtual void onSet(const unsigned int& i) override;

		private:
			IProperty<std::string>& m_name;
		};

		//Updates one string to match another string (useful for the modifier name on a NiPSysModifierCtlr)
		class ModifierNameListener : public PropertyListener<std::string>
		{
		public:
			ModifierNameListener(IProperty<std::string>& name) :
				m_name{ name } {}

			virtual void onSet(const std::string& s) override { m_name.set(s); }

		private:
			IProperty<std::string>& m_name;
		};

	protected:
		//Derive from this to add functionality (controllers, requirements)
		class Device : public SequentialDevice<IModifiable>
		{
		public:
			Device(Modifier& node) :
				m_node{ node }, m_mod{ node.object() }, m_modLsnr{ m_mod.order() }, m_ordLsnr{ m_mod.name() } {}
			virtual ~Device() = default;

			virtual void onConnect(IModifiable& ifc) override;
			virtual void onDisconnect(IModifiable& ifc) override;

		protected:
			//NodeBase will survive us. Modifier will not.
			NodeBase& m_node;
			nif::NiPSysModifier& m_mod;
			OrderListener m_modLsnr;
			NameListener m_ordLsnr;
		};

		template<Requirement Req>
		class ReqDevice : public Device
		{
		public:
			ReqDevice(Modifier& node) : Device(node) {}
			virtual ~ReqDevice() = default;

			virtual void onConnect(IModifiable& ifc) override;
			virtual void onDisconnect(IModifiable& ifc) override;
		};

		class TargetField final : public Field
		{
		public:
			TargetField(const std::string& name, Modifier& node, const std::shared_ptr<Device>& device);

		private:
			std::shared_ptr<Device> m_device;
			Sender<void> m_sndr;
		};

		class NextModField final : public Field
		{
		public:
			NextModField(const std::string& name, Modifier& node, const std::shared_ptr<Device>& device);

		private:
			std::shared_ptr<Device> m_device;
			Receiver<void> m_rcvr;
		};

	private:
		//For our unknown controllers:
		std::vector<std::unique_ptr<ModifierNameListener>> m_lsnrs;
	};

	class IModifiable
	{
	public:
		virtual ~IModifiable() = default;

		virtual ISequence<nif::NiPSysModifier>& modifiers() = 0;
		virtual ISequence<nif::NiTimeController>& controllers() = 0;
		virtual ISet<Modifier::Requirement>& requirements() = 0;

		//When connecting a mod: 
		//*insert the modifier
		//*insert the controller, if any
		//*specify any requirements
		//*set your own order from the return of insert. This way it will reach listeners.
		//*listen to the sequence to learn about order changes
	};

	template<Modifier::Requirement Req>
	inline void node::Modifier::ReqDevice<Req>::onConnect(IModifiable& ifc)
	{
		ifc.requirements().add(Req);
		Device::onConnect(ifc);
	}
	template<Modifier::Requirement Req>
	inline void node::Modifier::ReqDevice<Req>::onDisconnect(IModifiable& ifc)
	{
		Device::onDisconnect(ifc);
		ifc.requirements().remove(Req);
	}


	class DummyModifier final : public Modifier
	{
	public:
		DummyModifier(std::unique_ptr<nif::NiPSysModifier>&& obj);

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 80.0f;
	};
}
