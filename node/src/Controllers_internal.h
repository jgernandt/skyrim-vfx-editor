//Copyright 2021, 2022 Jonas Gernandt
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
#include "Controllers.h"
#include "ControllerManager.h"
#include "Constructor.h"
#include "AnimationManager.h"

namespace node
{
	using namespace nif;

	constexpr ControllerFlags DEFAULT_CTLR_FLAGS = 72;
	constexpr float DEFAULT_FREQUENCY = 1.0f;
	constexpr float DEFAULT_PHASE = 0.0f;
	constexpr float DEFAULT_STARTTIME = 0.0f;
	constexpr float DEFAULT_STOPTIME = 1.0f;

	template<typename T>
	struct InterpolatorFactory : VerticalTraverser<T, InterpolatorFactory>
	{
		bool operator() (const T&, IplrFactoryVisitor& v, File& file, const std::string& iplrID) { return true; }
	};

	template<>
	class Default<FloatController>
	{
	public:
		std::unique_ptr<FloatController> create(
			File& file, 
			ni_ptr<NiFloatInterpolator> iplr = ni_ptr<NiFloatInterpolator>(),
			const NiTimeController* ctlr = nullptr)
		{
			if (!iplr) {
				iplr = file.create<nif::NiFloatInterpolator>();
				if (!iplr)
					throw std::runtime_error("Failed to create NiFloatInterpolator");
			}

			if (!iplr->data.assigned()) {
				//We don't necessarily need a data block. Should we always have one regardless?
				auto data = file.create<NiFloatData>();
				if (!data)
					throw std::runtime_error("Failed to create NiFloatData");

				data->keyType.set(KEY_LINEAR);

				data->keys.push_back();
				data->keys.back().time.set(ctlr ? ctlr->startTime.get() : 0.0f);
				data->keys.back().value.set(iplr->value.get());

				data->keys.push_back();
				data->keys.back().time.set(ctlr ? ctlr->stopTime.get() : 1.0f);
				data->keys.back().value.set(iplr->value.get());

				iplr->data.assign(data);
			}

			auto node = std::make_unique<FloatController>(iplr);

			if (ctlr) {
				node->flags().raise(ctlr->flags.raised());
				node->frequency().set(ctlr->frequency.get());
				node->phase().set(ctlr->phase.get());
				node->startTime().set(ctlr->startTime.get());
				node->stopTime().set(ctlr->stopTime.get());
			}
			else {
				node->flags().raise(DEFAULT_CTLR_FLAGS);
				node->frequency().set(DEFAULT_FREQUENCY);
				node->phase().set(DEFAULT_PHASE);
				node->startTime().set(DEFAULT_STARTTIME);
				node->stopTime().set(DEFAULT_STOPTIME);
			}

			return node;
		}
	};

	template<>
	class Default<NLFloatController>
	{
	public:
		std::unique_ptr<NLFloatController> create(
			File& file,
			ni_ptr<NiBlendFloatInterpolator> iplr = ni_ptr<NiBlendFloatInterpolator>())
		{
			if (!iplr) {
				iplr = file.create<nif::NiBlendFloatInterpolator>();
				if (!iplr)
					throw std::runtime_error("Failed to create NiBlendFloatInterpolator");

				iplr->managerControlled.set(true);
			}

			return std::make_unique<NLFloatController>(iplr);
		}
	};


	template<>
	class AnimationInit<NiTimeController> : public VerticalTraverser<NiTimeController, AnimationInit>
	{
	public:
		template<typename VisitorType>
		bool operator() (NiTimeController& obj, VisitorType& v) 
		{ 
			ControlledBlock* block = v.getCurrentBlock();
			assert(block && block->controller.assigned().get() == &obj);

			AnimationManager::BlockInfo b;
			b.ctlr = block->controller.assigned();
			b.target = v.findObject(block->nodeName.get());
			b.propertyType = block->propertyType.get();
			b.ctlrType = block->ctlrType.get();
			b.ctlrID = block->ctlrID.get();
			b.iplrID = block->iplrID.get();

			v.registerBlock(b);

			return false; 
		}
	};

	template<>
	class AnimationInit<NiPSysModifierCtlr> : public VerticalTraverser<NiPSysModifierCtlr, AnimationInit>
	{
	public:
		template<typename VisitorType>
		bool operator() (NiPSysModifierCtlr& obj, VisitorType& v)
		{
			ControlledBlock* block = v.getCurrentBlock();
			assert(block && block->controller.assigned().get() == &obj);

			AnimationManager::BlockInfo b;
			b.ctlr = block->controller.assigned();
			b.ctlrIDProperty = make_ni_ptr(std::static_pointer_cast<NiPSysModifierCtlr>(b.ctlr), &NiPSysModifierCtlr::modifierName);
			b.target = v.findObject(block->nodeName.get());
			b.propertyType = block->propertyType.get();
			b.ctlrType = block->ctlrType.get();
			b.ctlrID = obj.modifierName.get();
			b.iplrID = block->iplrID.get();

			v.registerBlock(b);

			return false;
		}
	};


	//NiFloatInterpolator////////////
	template<>
	class Factory<NiFloatInterpolator> : public VerticalTraverser<NiFloatInterpolator, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiFloatInterpolator& obj, C& ctor)
		{
			assert(ctor.getObject().get() == &obj);

			auto node = Default<FloatController>{}.create(
				ctor.getFile(), std::static_pointer_cast<NiFloatInterpolator>(ctor.getObject()), ctor.getController(&obj));
			ctor.addNode(&obj, std::move(node));

			return false;
		}
	};

	//NiBlendFloatInterpolator////////////
	template<>
	class Factory<NiBlendFloatInterpolator> : public VerticalTraverser<NiBlendFloatInterpolator, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiBlendFloatInterpolator& obj, C& ctor)
		{
			assert(ctor.getObject().get() == &obj);

			auto node = Default<NLFloatController>{}.create(
				ctor.getFile(), std::static_pointer_cast<NiBlendFloatInterpolator>(ctor.getObject()));
			ctor.addNode(&obj, std::move(node));

			return false;
		}
	};


	//NiPSysEmitterCtlr////////////
	/*
	Should connect our interpolators to the correct fields of the modifier node (these connections may be unused)
	Should map itself to its interpolators
	Should not create any nodes
	Should forward to our interpolators, if they have data OR they are blends
	Should create a NiFloatInterpolator if ID is "BirthRate" or a NiBoolInterpolator if ID is "EmitterActive"
	*/

	template<>
	class Connector<NiPSysEmitterCtlr> : public VerticalTraverser<NiPSysEmitterCtlr, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiPSysEmitterCtlr& obj, C& ctor)
		{
			//Locate our target modifier
			if (auto target = obj.target.assigned()) {
				for (auto&& mod : std::static_pointer_cast<NiParticleSystem>(target)->modifiers) {
					assert(mod);
					if (mod->name.get() == obj.modifierName.get()) {
						ConnectionInfo info;
						info.object1 = obj.interpolator.assigned().get();
						info.field1 = FloatController::TARGET;
						info.object2 = mod.get();
						info.field2 = Emitter::BirthRate::ID;
						ctor.addConnection(info);

						//TODO: visibility interpolator

						break;
					}
				}
			}

			if (auto&& iplr = obj.interpolator.assigned())
				ctor.mapController(iplr.get(), &obj);
			if (auto&& iplr = obj.visIplr.assigned())
				ctor.mapController(iplr.get(), &obj);

			return true;
		}
	};

	template<>
	class Forwarder<NiPSysEmitterCtlr> : public VerticalTraverser<NiPSysEmitterCtlr, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiPSysEmitterCtlr& obj, C& ctor)
		{
			if (auto&& iplr = obj.interpolator.assigned()) {
				if (ni_type type = iplr->type(); type == NiFloatInterpolator::TYPE) {
					if (static_cast<NiFloatInterpolator*>(iplr.get())->data.assigned()) {
						//forward
						ctor.pushObject(iplr);
						iplr->receive(ctor);
						ctor.popObject();
					}
				}
				else if (type == NiBlendFloatInterpolator::TYPE) {
					//forward
					ctor.pushObject(iplr);
					iplr->receive(ctor);
					ctor.popObject();
				}
			}

			//TODO: visibility interpolator

			return true;
		}
	};

	template<>
	struct InterpolatorFactory<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, InterpolatorFactory>
	{
		bool operator() (const NiPSysEmitterCtlr&, IplrFactoryVisitor& v, File& file, const std::string& iplrID)
		{
			if (iplrID == "BirthRate")
				v.iplr = file.create<NiFloatInterpolator>();
			else if (iplrID == "EmitterActive")
				v.iplr = file.create<NiBoolInterpolator>();
			return false;
		}
	};


	//NiPSysGravityStrengthCtlr////////////
	/*
	Should connect our interpolator to the Strength field of the modifier
	Should map itself to its interpolator (should be inherited functionality)
	Should not create any nodes
	Should forward to our interpolator
	Should create a NiFloatInterpolator (should be inherited functionality)
	*/

	template<>
	class Connector<NiPSysGravityStrengthCtlr> : public VerticalTraverser<NiPSysGravityStrengthCtlr, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiPSysGravityStrengthCtlr& obj, C& ctor)
		{
			//Locate our target modifier
			if (auto target = obj.target.assigned()) {
				for (auto&& mod : static_cast<NiParticleSystem*>(target.get())->modifiers) {
					assert(mod);
					if (mod->name.get() == obj.modifierName.get()) {
						ConnectionInfo info;
						//There should always be an interpolator at this point. Even if the file
						//was missing one (which is an error), our Factory should have added it.
						info.object1 = obj.interpolator.assigned().get();
						info.field1 = FloatController::TARGET;
						info.object2 = mod.get();
						info.field2 = GravityModifier::STRENGTH;
						ctor.addConnection(info);

						break;
					}
				}
			}

			if (auto&& iplr = obj.interpolator.assigned())
				ctor.mapController(iplr.get(), &obj);

			return true;
		}
	};

	template<>
	class Forwarder<NiPSysGravityStrengthCtlr> : public VerticalTraverser<NiPSysGravityStrengthCtlr, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiPSysGravityStrengthCtlr& obj, C& ctor)
		{
			if (auto&& iplr = obj.interpolator.assigned()) {
				ctor.pushObject(iplr);
				iplr->receive(ctor);
				ctor.popObject();
			}

			return true;
		}
	};

	//Should be left to a NiPSysModifierFloatCtlr
	template<>
	struct InterpolatorFactory<NiPSysGravityStrengthCtlr> : VerticalTraverser<NiPSysGravityStrengthCtlr, InterpolatorFactory>
	{
		bool operator() (const NiPSysGravityStrengthCtlr&, IplrFactoryVisitor& v, File& file, const std::string& iplrID)
		{
			v.iplr = file.create<NiFloatInterpolator>();
			return false;
		}
	};
}
