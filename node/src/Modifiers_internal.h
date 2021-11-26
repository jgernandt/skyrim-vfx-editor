#pragma once
#include "GravityModifier.h"
#include "RotationModifier.h"
#include "ScaleModifier.h"
#include "SimpleColourModifier.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	//NiPSysModifier/////
	
	//No Default specialisation

	template<>
	class Connector<NiPSysModifier> : public VerticalTraverser<NiPSysModifier, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiPSysModifier& obj, C& ctor)
		{
			//Request connection to our psys and specify our order. 
			//Who we will actually connect to will be sorted out later.
			ConnectionInfo info;
			info.object1 = &obj;
			info.field1 = Modifier::TARGET;
			info.object2 = obj.target.assigned().get();
			info.field2 = ParticleSystem::MODIFIERS;
			info.order = obj.order.get();
			ctor.addConnection(info);

			return true;
		}
	};

	template<>
	class Factory<NiPSysModifier> : public VerticalTraverser<NiPSysModifier, Factory>
	{
	public:
		class Traverser final : public NiTraverser
		{
			std::string m_name;
		public:
			ni_ptr<NiTimeController> current;
			std::vector<ni_ptr<NiPSysModifierCtlr>> controllers;

			Traverser(std::string&& name) : m_name{ std::move(name) } {}

			virtual void traverse(NiPSysModifierCtlr& obj) override
			{
				assert(current.get() == &obj);
				if (obj.modifierName.get() == m_name)
					controllers.push_back(std::static_pointer_cast<NiPSysModifierCtlr>(current));
			}
			virtual void traverse(NiPSysEmitterCtlr& obj) override
			{
				Traverser::traverse(static_cast<NiPSysModifierCtlr&>(obj));
			}
		};

		template<typename C>
		bool operator() (NiPSysModifier& obj, C& ctor)
		{
			if (ni_ptr<NiPSysModifier> ptr = std::static_pointer_cast<NiPSysModifier>(ctor.getObject()); ptr.get() == &obj) {

				//need to go through target's controllers
				//if PSysModifierCtlr, look if name matches that of obj
				Traverser t(obj.name.get());
				if (auto&& target = obj.target.assigned()) {
					for (auto&& ctlr : target->controllers) {
						t.current = ctlr;
						ctlr->receive(t);
					}
				}

				auto node = Default<DummyModifier>{}.create(ctor.getFile(), ptr);

				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation



	template<>
	class Default<PlanarForceField>
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

				new_obj->active.set(true);
				new_obj->forceType.set(FORCE_PLANAR);
				new_obj->gravityAxis.set({ 0.0f, 0.0f, 1.0f });

				return std::make_unique<PlanarForceField>(new_obj);
			}
		}
	};

	template<>
	class Default<SphericalForceField>
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

				new_obj->active.set(true);
				new_obj->forceType.set(FORCE_SPHERICAL);

				return std::make_unique<SphericalForceField>(new_obj);
			}
		}
	};

	template<>
	class Default<RotationModifier>
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

				new_obj->active.set(true);

				return std::make_unique<RotationModifier>(new_obj);
			}
		}
	};

	template<>
	class Default<ScaleModifier>
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

				new_obj->active.set(true);
				new_obj->scales.set({ 0.0f, 1.0f });

				return std::make_unique<ScaleModifier>(new_obj);
			}
		}
	};

	template<>
	class Default<SimpleColourModifier>
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

				new_obj->active.set(true);

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
}
