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
#include "Controllers.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

	constexpr ControllerFlags DEFAULT_CTLR_FLAGS = 72;
	constexpr float DEFAULT_FREQUENCY = 1.0f;
	constexpr float DEFAULT_PHASE = 0.0f;
	constexpr float DEFAULT_STARTTIME = 0.0f;
	constexpr float DEFAULT_STOPTIME = 1.0f;

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


	//NiPSysEmitterCtlr////////////

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
						info.field2 = Emitter::BIRTH_RATE;
						ctor.addConnection(info);

						break;
					}
				}
			}

			return true;
		}
	};

	template<>
	class Factory<NiPSysEmitterCtlr> : public VerticalTraverser<NiPSysEmitterCtlr, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysEmitterCtlr& obj, C& ctor)
		{
			//If we have a float iplr with data, create a controller node.
			//That we invoke a factory on the interpolator is perhaps not entirely consistent
			//with how we do this elsewhere, but seems like the cleanest solution. We are the
			//one that knows what type of node (if any) to create. 
			//Not the interpolator, not the modifier, not the particle system.
			if (auto&& iplr = obj.interpolator.assigned()) {
				if (ni_type type = iplr->type(); type == NiFloatInterpolator::TYPE) {
					if (static_cast<NiFloatInterpolator*>(iplr.get())->data.assigned()) {
						auto node = Default<FloatController>{}.create(
							ctor.getFile(), std::static_pointer_cast<NiFloatInterpolator>(iplr), &obj);
						ctor.addNode(iplr.get(), std::move(node));
					}
				}
				else if (type == NiBlendFloatInterpolator::TYPE) {
					ctor.addNode(iplr.get(), Default<NLFloatController>{}.create(
						ctor.getFile(), std::static_pointer_cast<NiBlendFloatInterpolator>(iplr)));
				}
			}

			return false;
		}
	};


	//NiPSysGravityStrengthCtlr////////////

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

			return true;
		}
	};

	template<>
	class Factory<NiPSysGravityStrengthCtlr> : public VerticalTraverser<NiPSysGravityStrengthCtlr, Factory>
	{
	public:
		template<typename C>
		bool operator() (NiPSysGravityStrengthCtlr& obj, C& ctor)
		{
			if (auto&& iplr = obj.interpolator.assigned()) {
				if (ni_type type = iplr->type(); type == NiFloatInterpolator::TYPE) {
					ctor.addNode(iplr.get(), Default<FloatController>{}.create(
						ctor.getFile(), std::static_pointer_cast<NiFloatInterpolator>(iplr), &obj));
				}
				else if (type == NiBlendFloatInterpolator::TYPE) {
					ctor.addNode(iplr.get(), Default<NLFloatController>{}.create(
						ctor.getFile(), std::static_pointer_cast<NiBlendFloatInterpolator>(iplr)));
				}
			}
			else {
				auto new_iplr = ctor.getFile().create<NiFloatInterpolator>();
				ctor.addNode(new_iplr.get(), Default<FloatController>{}.create(ctor.getFile(), new_iplr, &obj));
			}

			return false;
		}
	};
}
