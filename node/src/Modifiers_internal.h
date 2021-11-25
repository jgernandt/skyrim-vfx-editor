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
		std::unique_ptr<PlanarForceField> create(File& file)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			if (!obj)
				throw std::runtime_error("Failed to create NiPSysGravityModifier");

			obj->active.set(true);
			obj->forceType.set(FORCE_PLANAR);
			obj->gravityAxis.set({ 0.0f, 0.0f, 1.0f });

			return std::make_unique<PlanarForceField>(obj);
		}
	};

	template<>
	class Default<SphericalForceField>
	{
	public:
		std::unique_ptr<SphericalForceField> create(File& file)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			if (!obj)
				throw std::runtime_error("Failed to create NiPSysGravityModifier");

			obj->active.set(true);
			obj->forceType.set(FORCE_SPHERICAL);

			return std::make_unique<SphericalForceField>(obj);
		}
	};

	template<>
	class Default<RotationModifier>
	{
	public:
		std::unique_ptr<RotationModifier> create(File& file)
		{
			auto obj = file.create<NiPSysRotationModifier>();
			if (!obj)
				throw std::runtime_error("Failed to create NiPSysRotationModifier");

			obj->active.set(true);

			return std::make_unique<RotationModifier>(obj);
		}
	};

	template<>
	class Default<ScaleModifier>
	{
	public:
		std::unique_ptr<ScaleModifier> create(File& file)
		{
			auto obj = file.create<BSPSysScaleModifier>();
			if (!obj)
				throw std::runtime_error("Failed to create BSPSysScaleModifier");

			obj->active.set(true);
			obj->scales.set({ 0.0f, 1.0f });

			return std::make_unique<ScaleModifier>(obj);
		}
	};

	template<>
	class Default<SimpleColourModifier>
	{
	public:
		std::unique_ptr<SimpleColourModifier> create(File& file)
		{
			auto obj = file.create<BSPSysSimpleColorModifier>();
			if (!obj)
				throw std::runtime_error("Failed to create BSPSysSimpleColorModifier");

			obj->active.set(true);

			obj->col1. value.set({ 1.0f, 0.0f, 0.0f, 0.0f });
			obj->col2.value.set(COL_GREEN);
			obj->col3.value.set({ 0.0f, 0.0f, 1.0f, 0.0f });

			obj->col1.RGBend.set(0.2f);
			obj->col2.RGBbegin.set(0.4f);
			obj->col2.RGBend.set(0.6f);
			obj->col3.RGBbegin.set(0.8f);

			obj->col2.Abegin.set(0.1f);
			obj->col2.Aend.set(0.9f);

			return std::make_unique<SimpleColourModifier>(obj);
		}
	};
}
