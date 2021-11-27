#pragma once
#include "ParticleSystem.h"
#include "Constructor.h"
#include "AVObject_internal.h"
#include "Controllers_internal.h"

namespace node
{
	using namespace nif;

	constexpr unsigned short DEFAULT_MAX_COUNT = 100;

	template<>
	class Default<ParticleSystem> : Default<AVObject>
	{
	public:
		std::unique_ptr<ParticleSystem> create(File& file,
			ni_ptr<NiParticleSystem> psys = ni_ptr<NiParticleSystem>(),
			ni_ptr<NiPSysData> data = ni_ptr<NiPSysData>(),
			ni_ptr<NiAlphaProperty> alpha = ni_ptr<NiAlphaProperty>(),
			ni_ptr<NiPSysAgeDeathModifier> adm = ni_ptr<NiPSysAgeDeathModifier>(),
			ni_ptr<NiPSysBoundUpdateModifier> bum = ni_ptr<NiPSysBoundUpdateModifier>(),
			ni_ptr<NiPSysPositionModifier> pm = ni_ptr<NiPSysPositionModifier>(),
			ni_ptr<NiPSysUpdateCtlr> ctlr = ni_ptr<NiPSysUpdateCtlr>())
		{
			if (!psys) {
				psys = file.create<NiParticleSystem>();
				if (!psys)
					throw std::runtime_error("Failed to create NiParticleSystem");

				setDefaults(*psys);
				static int n = 0;
				psys->name.set(std::string("ParticleSystem") + std::to_string(++n));
			}

			if (!data) {
				data = file.create<nif::NiPSysData>();
				if (!data)
					throw std::runtime_error("Failed to create NiPSysData");

				assert(!psys->data.assigned());
				psys->data.assign(data);
				data->maxCount.set(DEFAULT_MAX_COUNT);
			}

			if (!alpha) {
				alpha = file.create<nif::NiAlphaProperty>();
				if (!alpha)
					throw std::runtime_error("Failed to create NiAlphaProperty");

				alpha->mode.set(AlphaMode::BLEND);
				alpha->srcFcn.set(BlendFunction::SRC_ALPHA);
				alpha->dstFcn.set(BlendFunction::ONE_MINUS_SRC_ALPHA);

				assert(!psys->alphaProperty.assigned());
				psys->alphaProperty.assign(alpha);
			}

			if (!adm) {
				adm = file.create<nif::NiPSysAgeDeathModifier>();
				if (!adm)
					throw std::runtime_error("Failed to create NiPSysAgeDeathModifier");
				adm->active.set(true);
			}

			if (!bum) {
				bum = file.create<nif::NiPSysBoundUpdateModifier>();
				if (!bum)
					throw std::runtime_error("Failed to create NiPSysBoundUpdateModifier");
				bum->active.set(true);
			}

			if (!pm) {
				pm = file.create<nif::NiPSysPositionModifier>();
				if (!pm)
					throw std::runtime_error("Failed to create NiPSysPositionModifier");
				pm->active.set(true);
			}

			if (!ctlr) {
				ctlr = file.create<nif::NiPSysUpdateCtlr>();
				if (!ctlr)
					throw std::runtime_error("Failed to create NiPSysUpdateCtlr");

				ctlr->flags.raise(DEFAULT_CTLR_FLAGS);
				ctlr->frequency.set(DEFAULT_FREQUENCY);
				ctlr->phase.set(DEFAULT_PHASE);
				ctlr->startTime.set(DEFAULT_STARTTIME);
				ctlr->stopTime.set(DEFAULT_STOPTIME);
			}

			return std::make_unique<ParticleSystem>(psys, data, alpha, adm, bum, pm, ctlr);
		}
	};

	template<>
	class Connector<NiParticleSystem> : public VerticalTraverser<NiParticleSystem, Connector>
	{
	public:
		template<typename C>
		bool operator() (NiParticleSystem& obj, C& ctor)
		{
			std::vector<NiPSysModifier*> mods;
			mods.reserve(obj.modifiers.size());
			for (auto&& mod : obj.modifiers)
				mods.push_back(mod.get());
			ctor.addModConnections(&obj, std::move(mods));

			//We expect that shader and alpha properties have already been added if missing.
			assert(obj.shaderProperty.assigned() && obj.alphaProperty.assigned());
			ctor.addConnection(ConnectionInfo{ 
				&obj, 
				obj.shaderProperty.assigned().get(), 
				ParticleSystem::SHADER, 
				EffectShader::GEOMETRY });

			ctor.addConnection(ConnectionInfo{
				&obj,
				obj.alphaProperty.assigned().get(),
				ParticleSystem::ALPHA,
				"" });

			return true;
		}
	};
	
	template<>
	class Factory<NiParticleSystem> : public VerticalTraverser<NiParticleSystem, Factory>
	{
	public:
		struct Compare
		{
			bool operator() (const ni_ptr<NiPSysModifier>& lhs, const ni_ptr<NiPSysModifier>& rhs)
			{
				assert(lhs && rhs);
				return lhs->order.get() < rhs->order.get();
			}
		};

		template<typename C>
		bool operator() (NiParticleSystem& obj, C& ctor)
		{
			if (ni_ptr<NiParticleSystem> ptr = std::static_pointer_cast<NiParticleSystem>(ctor.getObject()); ptr.get() == &obj) {
				//Sort mods
				std::sort(obj.modifiers.begin(), obj.modifiers.end(), Compare{});
				//Locate our static mods, if we have them
				ni_ptr<NiPSysAgeDeathModifier> adm;
				ni_ptr<NiPSysBoundUpdateModifier> bum;
				ni_ptr<NiPSysPositionModifier> pm;
				for (auto&& mod : obj.modifiers) {
					if (ni_type type = mod->type(); type == NiPSysAgeDeathModifier::TYPE)
						adm = std::static_pointer_cast<NiPSysAgeDeathModifier>(mod);
					else if (type == NiPSysBoundUpdateModifier::TYPE)
						bum = std::static_pointer_cast<NiPSysBoundUpdateModifier>(mod);
					else if (type == NiPSysPositionModifier::TYPE)
						pm = std::static_pointer_cast<NiPSysPositionModifier>(mod);
				}

				//and our update ctlr
				ni_ptr<NiPSysUpdateCtlr> uc;
				for (auto&& ctlr : obj.controllers) {
					if (ni_type type = ctlr->type(); type == NiPSysUpdateCtlr::TYPE)
						uc = std::static_pointer_cast<NiPSysUpdateCtlr>(ctlr);
				}

				ctor.addNode(&obj, Default<ParticleSystem>{}.create(
					ctor.getFile(), ptr, obj.data.assigned(), obj.alphaProperty.assigned(), adm, bum, pm, uc));
			}
			return false;
		}
	};

	template<>
	class Forwarder<NiParticleSystem> : public VerticalTraverser<NiParticleSystem, Forwarder>
	{
	public:
		template<typename C>
		bool operator() (NiParticleSystem& obj, C& ctor)
		{
			//Forward to modifiers, shader and alpha
			for (auto&& mod : obj.modifiers) {
				assert(mod);
				ctor.pushObject(mod);
				mod->receive(ctor);
				ctor.popObject();
			}
			if (auto&& shader = obj.shaderProperty.assigned()) {
				ctor.pushObject(shader);
				shader->receive(ctor);
				ctor.popObject();
			}
			if (auto&& alpha = obj.alphaProperty.assigned()) {
				ctor.pushObject(alpha);
				alpha->receive(ctor);
				ctor.popObject();
			}

			return true;
		}
	};
}
