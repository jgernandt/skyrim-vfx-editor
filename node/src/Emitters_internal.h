#pragma once
#include "Emitters.h"
#include "Constructor.h"
#include "Controllers_internal.h"

namespace node
{
	using namespace nif;

	constexpr ColRGBA DEFAULT_VCOLOUR = COL_WHITE;
	constexpr math::degf DEFAULT_ELEVATION = math::degf{ 0.0f };

	template<>
	class Default<Emitter>
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
	};

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
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<BoxEmitter>(obj, ctlr, iplr, visIplr);
		}

		void setDefaults(NiPSysEmitter& obj) {
			obj.colour.set(DEFAULT_VCOLOUR);
			obj.elevation.set(DEFAULT_ELEVATION);
		}
	};

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
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<CylinderEmitter>(obj, ctlr, iplr, visIplr);
		}
	};

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
			}
			getIplrs(file, ctlr, iplr, visIplr);

			return std::make_unique<SphereEmitter>(obj, ctlr, iplr, visIplr);
		}
	};
}
