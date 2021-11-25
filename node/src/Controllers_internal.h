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
			ni_ptr<NiFloatData> data = ni_ptr<NiFloatData>(),
			const NiTimeController* ctlr = nullptr)
		{
			if (!iplr) {
				iplr = file.create<nif::NiFloatInterpolator>();
				if (!iplr)
					throw std::runtime_error("Failed to create NiFloatInterpolator");
			}
			if (!data) {
				//We don't necessarily need a data block. Should we always have one regardless?
				data = file.create<NiFloatData>();
				if (!data)
					throw std::runtime_error("Failed to create NiFloatData");

				data->keyType.set(KEY_LINEAR);

				data->keys.push_back();
				data->keys.back().time.set(ctlr ? ctlr->startTime.get() : 0.0f);
				data->keys.back().value.set(0.0f);

				data->keys.push_back();
				data->keys.back().time.set(ctlr ? ctlr->stopTime.get() : 1.0f);
				data->keys.back().value.set(0.0f);
			}

			//Let the node decide if it wants the data assigned or not
			auto node = std::make_unique<FloatController>(iplr, data);

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
}
