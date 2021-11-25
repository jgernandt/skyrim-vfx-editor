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
			ni_ptr<NiParticleSystem>&& psys = ni_ptr<NiParticleSystem>(),
			ni_ptr<NiPSysData>&& data = ni_ptr<NiPSysData>(),
			ni_ptr<NiAlphaProperty>&& alpha = ni_ptr<NiAlphaProperty>(),
			ni_ptr<NiPSysAgeDeathModifier>&& adm = ni_ptr<NiPSysAgeDeathModifier>(),
			ni_ptr<NiPSysBoundUpdateModifier>&& bum = ni_ptr<NiPSysBoundUpdateModifier>(),
			ni_ptr<NiPSysPositionModifier>&& pm = ni_ptr<NiPSysPositionModifier>(),
			ni_ptr<NiPSysUpdateCtlr>&& ctlr = ni_ptr<NiPSysUpdateCtlr>())
		{
			if (!psys) {
				psys = file.create<NiParticleSystem>();
				if (!psys)
					throw std::runtime_error("Failed to create NiParticleSystem");
			}

			setDefaults(*psys);

			static int n = 0;
			psys->name.set(std::string("ParticleSystem") + std::to_string(++n));

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

			return std::make_unique<ParticleSystem>(std::move(psys), std::move(data), 
				std::move(alpha), std::move(adm), std::move(bum), std::move(pm), std::move(ctlr));
		}
	};
}
