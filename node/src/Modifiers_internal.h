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
#include "GravityModifier.h"
#include "RotationModifier.h"
#include "ScaleModifier.h"
#include "SimpleColourModifier.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	class ModifierCtlrTraverser : public NiTraverser
	{
	protected:
		std::string m_name;

	public:
		ni_ptr<NiTimeController> current;
		std::vector<ni_ptr<NiPSysModifierCtlr>> controllers;

		ModifierCtlrTraverser(std::string&& name) : m_name{ std::move(name) } {}

		virtual void traverse(NiPSysModifierCtlr& obj) final override
		{
			assert(current.get() == &obj);
			if (obj.modifierName.get() == m_name)
				controllers.push_back(std::static_pointer_cast<NiPSysModifierCtlr>(current));
		}
		virtual void traverse(NiPSysEmitterCtlr& obj) override
		{
			ModifierCtlrTraverser::traverse(static_cast<NiPSysModifierCtlr&>(obj));
		}
	};

	//NiPSysModifier/////

	template<>
	class Default<Modifier>
	{
	public:
		void setDefaults(NiPSysModifier& obj)
		{
			obj.active.set(true);
			obj.order.set(-1);
		}
	};
	
	template<>
	class Default<DummyModifier> : public Default<Modifier>
	{
	public:
		std::unique_ptr<DummyModifier> create(nif::File& file,
			const ni_ptr<NiPSysModifier>& obj = ni_ptr<NiPSysModifier>())
		{
			if (obj)
				return std::make_unique<DummyModifier>(obj);
			else {
				auto new_obj = file.create<NiPSysModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create NiPSysModifier");

				this->setDefaults(*new_obj);

				return std::make_unique<DummyModifier>(new_obj);
			}
		}
	};

	//No Connector specialisation

	template<>
	class Factory<NiPSysModifier> : public VerticalTraverser<NiPSysModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysModifier& obj, C& ctor)
		{
			if (ni_ptr<NiPSysModifier> ptr = std::static_pointer_cast<NiPSysModifier>(ctor.getObject()); ptr.get() == &obj) {

				//need to go through target's controllers
				//if PSysModifierCtlr, look if name matches that of obj
				ModifierCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<DummyModifier>{}.create(ctor.getFile(), ptr);

				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}

		void findControllers(NiPSysModifier& obj, ModifierCtlrTraverser& t)
		{
			if (auto&& target = obj.target.assigned()) {
				for (auto&& ctlr : target->controllers) {
					t.current = ctlr;
					ctlr->receive(t);
				}
			}
		}
	};

	//No Forwarder specialisation


	//NiPSysAgeDeathModifier/////

	//No Default specialisation
	//No Connector specialisation

	template<>
	class Factory<NiPSysAgeDeathModifier> : public VerticalTraverser<NiPSysAgeDeathModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysAgeDeathModifier&, C&) { return false; }
	};
	
	//No Forwarder specialisation


	//NiPSysBoundUpdateModifier/////

	//No Default specialisation
	//No Connector specialisation

	template<>
	class Factory<NiPSysBoundUpdateModifier> : public VerticalTraverser<NiPSysBoundUpdateModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysBoundUpdateModifier&, C&) { return false; }
	};

	//No Forwarder specialisation


	//NiPSysPositionModifier/////

	//No Default specialisation
	//No Connector specialisation

	template<>
	class Factory<NiPSysPositionModifier> : public VerticalTraverser<NiPSysPositionModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysPositionModifier&, C&) { return false; }
	};

	//No Forwarder specialisation


	//NiPSysGravityModifier/////

	template<>
	class Default<PlanarForceField> : public Default<Modifier>
	{
	public:
		std::unique_ptr<PlanarForceField> create(File& file, const ni_ptr<NiPSysGravityModifier>& obj = ni_ptr<NiPSysGravityModifier>())
		{
			if (obj)
				return std::make_unique<PlanarForceField>(obj);
			else {
				auto new_obj = file.create<NiPSysGravityModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create NiPSysGravityModifier");

				this->setDefaults(*new_obj);
				new_obj->forceType.set(FORCE_PLANAR);
				new_obj->gravityAxis.set({ 0.0f, 0.0f, 1.0f });

				return std::make_unique<PlanarForceField>(new_obj);
			}
		}
	};

	template<>
	class Default<SphericalForceField> : public Default<Modifier>
	{
	public:
		std::unique_ptr<SphericalForceField> create(File& file, const ni_ptr<NiPSysGravityModifier>& obj = ni_ptr<NiPSysGravityModifier>())
		{
			if (obj)
				return std::make_unique<SphericalForceField>(obj);
			else {
				auto new_obj = file.create<NiPSysGravityModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create NiPSysGravityModifier");

				this->setDefaults(*new_obj);
				new_obj->forceType.set(FORCE_SPHERICAL);

				return std::make_unique<SphericalForceField>(new_obj);
			}
		}
	};

	template<>
	class Connector<NiPSysGravityModifier> : public VerticalTraverser<NiPSysGravityModifier, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiPSysGravityModifier& obj, C& ctor)
		{
			ConnectionInfo info;
			info.object1 = &obj;
			info.field1 = GravityModifier::GRAVITY_OBJECT;
			info.object2 = obj.gravityObject.assigned().get();
			info.field2 = Node::OBJECT;
			ctor.addConnection(info);

			return true;
		}
	};

	class GravityCtlrTraverser final : public ModifierCtlrTraverser
	{
	public:
		GravityCtlrTraverser(std::string&& name) : ModifierCtlrTraverser{ std::move(name) } {}
		//virtual void traverse(NiPSysGravityStrengthCtlr& obj) final override {}
	};

	template<>
	class Factory<NiPSysGravityModifier> : public VerticalTraverser<NiPSysGravityModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysGravityModifier& obj, C& ctor)
		{
			if (ni_ptr<NiPSysGravityModifier> ptr = std::static_pointer_cast<NiPSysGravityModifier>(ctor.getObject()); ptr.get() == &obj) {

				GravityCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				std::unique_ptr<GravityModifier> node;
				if (ptr->forceType.get() == FORCE_PLANAR)
					node = Default<PlanarForceField>{}.create(ctor.getFile(), ptr);
				else if (ptr->forceType.get() == FORCE_SPHERICAL)
					node = Default<SphericalForceField>{}.create(ctor.getFile(), ptr);

				if (node) {
					for (auto&& ctlr : t.controllers)
						node->addController(ctlr);

					ctor.addNode(&obj, std::move(node));
					return false;
				}
				else
					return true;
			}
			return false;
		}
	};

	//No Forwarder specialisation


	//NiPSysRotationModifier/////

	template<>
	class Default<RotationModifier> : public Default<Modifier>
	{
	public:
		std::unique_ptr<RotationModifier> create(File& file, const ni_ptr<NiPSysRotationModifier>& obj = ni_ptr<NiPSysRotationModifier>())
		{
			if (obj)
				return std::make_unique<RotationModifier>(obj);
			else {
				auto new_obj = file.create<NiPSysRotationModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create NiPSysRotationModifier");

				this->setDefaults(*new_obj);

				return std::make_unique<RotationModifier>(new_obj);
			}
		}
	};

	//No Connector specialisation

	class RotationCtlrTraverser final : public ModifierCtlrTraverser
	{
	public:
		RotationCtlrTraverser(std::string&& name) : ModifierCtlrTraverser{ std::move(name) } {}
		//virtual void traverse(NiPSysInitialRotAngleCtlr& obj) final override {}
		//virtual void traverse(NiPSysInitialRotAngleVarCtlr& obj) final override {}
		//virtual void traverse(NiPSysInitialRotSpeedCtlr& obj) final override {}
		//virtual void traverse(NiPSysInitialRotSpeedVarCtlr& obj) final override {}
	};

	template<>
	class Factory<NiPSysRotationModifier> : public VerticalTraverser<NiPSysRotationModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysRotationModifier& obj, C& ctor)
		{
			if (ni_ptr<NiPSysRotationModifier> ptr = std::static_pointer_cast<NiPSysRotationModifier>(ctor.getObject()); ptr.get() == &obj) {

				RotationCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<RotationModifier>{}.create(ctor.getFile(), ptr);

				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation


	//BSPSysScaleModifier/////

	template<>
	class Default<ScaleModifier> : public Default<Modifier>
	{
	public:
		std::unique_ptr<ScaleModifier> create(File& file, const ni_ptr<BSPSysScaleModifier>& obj = ni_ptr<BSPSysScaleModifier>())
		{
			if (obj)
				return std::make_unique<ScaleModifier>(obj);
			else {
				auto new_obj = file.create<BSPSysScaleModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create BSPSysScaleModifier");

				this->setDefaults(*new_obj);
				new_obj->scales.set({ 0.0f, 1.0f });

				return std::make_unique<ScaleModifier>(new_obj);
			}
		}
	};

	//No Connector specialisation

	template<>
	class Factory<BSPSysScaleModifier> : public VerticalTraverser<BSPSysScaleModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (BSPSysScaleModifier& obj, C& ctor)
		{
			if (ni_ptr<BSPSysScaleModifier> ptr = std::static_pointer_cast<BSPSysScaleModifier>(ctor.getObject()); ptr.get() == &obj) {

				ModifierCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<ScaleModifier>{}.create(ctor.getFile(), ptr);

				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation


	//BSPSysSimpleColorModifier/////

	template<>
	class Default<SimpleColourModifier> : public Default<Modifier>
	{
	public:
		std::unique_ptr<SimpleColourModifier> create(File& file, const ni_ptr<BSPSysSimpleColorModifier>& obj = ni_ptr<BSPSysSimpleColorModifier>())
		{
			if (obj)
				return std::make_unique<SimpleColourModifier>(obj);
			else {
				auto new_obj = file.create<BSPSysSimpleColorModifier>();
				if (!new_obj)
					throw std::runtime_error("Failed to create BSPSysSimpleColorModifier");

				this->setDefaults(*new_obj);

				new_obj->col1.value.set({ 1.0f, 0.0f, 0.0f, 0.0f });
				new_obj->col2.value.set(COL_GREEN);
				new_obj->col3.value.set({ 0.0f, 0.0f, 1.0f, 0.0f });

				new_obj->col1.RGBend.set(0.2f);
				new_obj->col2.RGBbegin.set(0.4f);
				new_obj->col2.RGBend.set(0.6f);
				new_obj->col3.RGBbegin.set(0.8f);

				new_obj->col2.Abegin.set(0.1f);
				new_obj->col2.Aend.set(0.9f);

				return std::make_unique<SimpleColourModifier>(new_obj);
			}
		}
	};

	//No Connector specialisation

	template<>
	class Factory<BSPSysSimpleColorModifier> : public VerticalTraverser<BSPSysSimpleColorModifier, Factory>
	{
	public:
		template<typename C>
		bool operator() (BSPSysSimpleColorModifier& obj, C& ctor)
		{
			if (ni_ptr<BSPSysSimpleColorModifier> ptr = std::static_pointer_cast<BSPSysSimpleColorModifier>(ctor.getObject()); ptr.get() == &obj) {

				ModifierCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<SimpleColourModifier>{}.create(ctor.getFile(), ptr);

				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation
}
