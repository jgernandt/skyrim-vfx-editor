#pragma once
#include "GravityModifier.h"
#include "RotationModifier.h"
#include "ScaleModifier.h"
#include "SimpleColourModifier.h"
#include "Constructor.h"

namespace node
{
	using namespace nif;

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
