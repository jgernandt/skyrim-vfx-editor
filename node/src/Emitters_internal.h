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
#include "Emitters.h"
#include "Constructor.h"
#include "Controllers_internal.h"
#include "Modifiers_internal.h"

namespace node
{
	using namespace nif;

	constexpr ColRGBA DEFAULT_VCOLOUR = COL_WHITE;
	constexpr math::degf DEFAULT_ELEVATION = math::degf{ 0.0f };

	class EmitterCtlrTraverser final : public ModifierCtlrTraverser
	{
	public:
		ni_ptr<NiPSysEmitterCtlr> emitterCtlr;

		EmitterCtlrTraverser(std::string&& name) : ModifierCtlrTraverser{ std::move(name) } {}

		virtual void traverse(NiPSysEmitterCtlr& obj) final override
		{
			assert(current.get() == &obj);
			if (obj.modifierName.get() == m_name) {
				if (emitterCtlr)
					//Unexpected: we have multiple emitter ctlrs (file error?). Treat this as an unknown ctlr.
					ModifierCtlrTraverser::traverse(static_cast<NiPSysModifierCtlr&>(obj));
				else
					emitterCtlr = std::static_pointer_cast<NiPSysEmitterCtlr>(current);
			}
		}
	};

	template<>
	class Default<Emitter> : public Default<Modifier>
	{
	public:

		void getIplrs(File& file, ni_ptr<NiPSysEmitterCtlr>& ctlr, 
			ni_ptr<NiFloatInterpolator>& iplr, ni_ptr<NiBoolInterpolator>& visI)
		{
			//inspect ctlr and create and/or assign to the iplrs

			if (!ctlr) {
				ctlr = file.create<nif::NiPSysEmitterCtlr>();
				if (!ctlr)
					throw std::runtime_error("Failed to create NiPSysEmitterCtlr");

				ctlr->flags.raise(DEFAULT_CTLR_FLAGS);
				ctlr->frequency.set(DEFAULT_FREQUENCY);
				ctlr->phase.set(DEFAULT_PHASE);
				ctlr->startTime.set(DEFAULT_STARTTIME);
				ctlr->stopTime.set(DEFAULT_STOPTIME);
			}

			//We'll use the assigned iplr only if it is a float interpolator without data
			if (auto&& i = ctlr->interpolator.assigned()) {
				if (i->type() == NiFloatInterpolator::TYPE) {
					auto fiplr = std::static_pointer_cast<NiFloatInterpolator>(i);
					if (!fiplr->data.assigned()) {
						iplr = fiplr;
					}
				}
			}
			//else create one
			if (!iplr) {
				iplr = file.create<nif::NiFloatInterpolator>();
				if (!iplr)
					throw std::runtime_error("Failed to create NiFloatInterpolator");
			}

			//We'll make a new visibility iplr regardless, since we have no system for that yet
			visI = file.create<nif::NiBoolInterpolator>();
			if (!visI)
				throw std::runtime_error("Failed to create NiBoolInterpolator");
			visI->value.set(true);
		}

		void setDefaults(NiPSysEmitter& obj) {
			Default<Modifier>::setDefaults(obj);
			obj.colour.set(DEFAULT_VCOLOUR);
			obj.elevation.set(DEFAULT_ELEVATION);
		}
	};


	//NiPSysVolumeEmitter/////

	//No Default specialisation

	template<>
	class Connector<NiPSysVolumeEmitter> : public VerticalTraverser<NiPSysVolumeEmitter, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiPSysVolumeEmitter& obj, C& ctor)
		{
			ConnectionInfo info;
			info.object1 = &obj;
			info.field1 = VolumeEmitter::EMITTER_OBJECT;
			info.object2 = obj.emitterObject.assigned().get();
			info.field2 = Node::OBJECT;
			ctor.addConnection(info);

			return true;
		}
	};

	//No Factory specialisation
	//No Forward specialisation


	//NiPSysBoxEmitter/////

	template<>
	class Default<BoxEmitter> : public Default<Emitter>
	{
	public:
		std::unique_ptr<BoxEmitter> create(File& file,
			ni_ptr<NiPSysBoxEmitter> obj = ni_ptr<NiPSysBoxEmitter>(),
			ni_ptr<NiPSysEmitterCtlr> ctlr = ni_ptr<NiPSysEmitterCtlr>())
		{
			ni_ptr<NiFloatInterpolator> iplr;
			ni_ptr<NiBoolInterpolator> visIplr;

			if (!obj) {
				obj = file.create<NiPSysBoxEmitter>();
				if (!obj)
					throw std::runtime_error("Failed to create NiPSysBoxEmitter");
				setDefaults(*obj);
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<BoxEmitter>(obj, ctlr, iplr, visIplr);
		}
	};

	//No Connector specialisation

	template<>
	class Factory<NiPSysBoxEmitter> : public VerticalTraverser<NiPSysBoxEmitter, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysBoxEmitter& obj, C& ctor) 
		{ 
			if (ni_ptr<NiPSysBoxEmitter> ptr = std::static_pointer_cast<NiPSysBoxEmitter>(ctor.getObject()); ptr.get() == &obj) {

				//Find our controllers
				EmitterCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<BoxEmitter>{}.create(ctor.getFile(), ptr, t.emitterCtlr);

				//To be replaced by known controllers
				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false; 
		}
	};

	//No Forwarder specialisation


	//NiPSysCylinderEmitter/////

	template<>
	class Default<CylinderEmitter> : public Default<Emitter>
	{
	public:
		std::unique_ptr<CylinderEmitter> create(File& file,
			ni_ptr<NiPSysCylinderEmitter> obj = ni_ptr<NiPSysCylinderEmitter>(),
			ni_ptr<NiPSysEmitterCtlr> ctlr = ni_ptr<NiPSysEmitterCtlr>())
		{
			ni_ptr<NiFloatInterpolator> iplr;
			ni_ptr<NiBoolInterpolator> visIplr;

			if (!obj) {
				obj = file.create<NiPSysCylinderEmitter>();
				if (!obj)
					throw std::runtime_error("Failed to create NiPSysCylinderEmitter");
				setDefaults(*obj);
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<CylinderEmitter>(obj, ctlr, iplr, visIplr);
		}
	};

	//No Connector specialisation

	template<>
	class Factory<NiPSysCylinderEmitter> : public VerticalTraverser<NiPSysCylinderEmitter, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysCylinderEmitter& obj, C& ctor)
		{
			if (ni_ptr<NiPSysCylinderEmitter> ptr = std::static_pointer_cast<NiPSysCylinderEmitter>(ctor.getObject()); ptr.get() == &obj) {

				//Find our controllers
				EmitterCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<CylinderEmitter>{}.create(ctor.getFile(), ptr, t.emitterCtlr);

				//To be replaced by known controllers
				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation


	//NiPSysSphereEmitter/////

	template<>
	class Default<SphereEmitter> : public Default<Emitter>
	{
	public:
		std::unique_ptr<SphereEmitter> create(File& file,
			ni_ptr<NiPSysSphereEmitter> obj = ni_ptr<NiPSysSphereEmitter>(),
			ni_ptr<NiPSysEmitterCtlr> ctlr = ni_ptr<NiPSysEmitterCtlr>())
		{
			ni_ptr<NiFloatInterpolator> iplr;
			ni_ptr<NiBoolInterpolator> visIplr;

			if (!obj) {
				obj = file.create<NiPSysSphereEmitter>();
				if (!obj)
					throw std::runtime_error("Failed to create NiPSysSphereEmitter");
				setDefaults(*obj);
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<SphereEmitter>(obj, ctlr, iplr, visIplr);
		}
	};

	//No Connector specialisation

	template<>
	class Factory<NiPSysSphereEmitter> : public VerticalTraverser<NiPSysSphereEmitter, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysSphereEmitter& obj, C& ctor)
		{
			if (ni_ptr<NiPSysSphereEmitter> ptr = std::static_pointer_cast<NiPSysSphereEmitter>(ctor.getObject()); ptr.get() == &obj) {

				//Find our controllers
				EmitterCtlrTraverser t(obj.name.get());
				findControllers(obj, t);

				auto node = Default<SphereEmitter>{}.create(ctor.getFile(), ptr, t.emitterCtlr);

				//To be replaced by known controllers
				for (auto&& ctlr : t.controllers)
					node->addController(ctlr);

				ctor.addNode(&obj, std::move(node));
			}
			return false;
		}
	};

	//No Forwarder specialisation
}
