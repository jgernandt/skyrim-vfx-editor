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
#include <memory>
#include <string>
#include "traits.h"
#include "type_conversion.h"
#include "Rotation.h"

namespace Niflib
{
	struct Color3;
	struct Color4;
	struct Vector3;
	struct Vector4;
	struct Matrix33;
	struct Quaternion;
	enum BillboardMode;
	enum ForceType;
	enum KeyType;

	class NiObject;
	class NiObjectNET;
	class NiAVObject;

	class NiTimeController;
	class NiSingleInterpController;
	class NiInterpolator;
	class NiBoolInterpolator;
	class NiBoolData;
	class NiFloatInterpolator;
	class NiFloatData;
	class NiBlendBoolInterpolator;
	class NiBlendFloatInterpolator;

	class NiNode;
	class BSFadeNode;
	class NiBillboardNode;

	class NiParticleSystem;
	class NiPSysData;

	class NiPSysModifier;
	class NiPSysModifierCtlr;

	class BSPSysScaleModifier;
	class BSPSysSimpleColorModifier;
	class NiPSysGravityModifier;
	class NiPSysRotationModifier;

	class NiPSysEmitter;
	class NiPSysVolumeEmitter;
	class NiPSysBoxEmitter;
	class NiPSysCylinderEmitter;
	class NiPSysSphereEmitter;
	class NiPSysEmitterCtlr;

	class NiPSysPositionModifier;
	class NiPSysAgeDeathModifier;

	class NiPSysBoundUpdateModifier;
	class NiPSysUpdateCtlr;

	class NiProperty;
	class BSEffectShaderProperty;
	class NiAlphaProperty;

	class NiExtraData;
	class NiStringExtraData;

	template<typename T> class Ref;
}

namespace nif
{
	template<size_t N>
	using Floats = std::array<float, N>;
	template<size_t N>
	using Ints = std::array<int, N>;

	using ColRGBA = Floats<4>;
	constexpr ColRGBA COL_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr ColRGBA COL_RED = { 1.0f, 0.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };

	using SubtextureCount = Ints<2>;
	using SubtextureOffset = Floats<4>;

	using translation_t = Floats<3>;
	using rotation_t = math::Rotation;
	using scale_t = float;

	using BillboardMode = uint_fast16_t;
	constexpr BillboardMode ALWAYS_FACE_CAMERA = 0;
	constexpr BillboardMode ROTATE_ABOUT_UP = 1;
	constexpr BillboardMode RIGID_FACE_CAMERA = 2;
	constexpr BillboardMode ALWAYS_FACE_CENTER = 3;
	constexpr BillboardMode RIGID_FACE_CENTER = 4;
	constexpr BillboardMode BS_ROTATE_ABOUT_UP = 5;
	constexpr BillboardMode ROTATE_ABOUT_UP2 = 9;

	using ForceType = uint_fast32_t;
	constexpr ForceType FORCE_PLANAR = 0;
	constexpr ForceType FORCE_SPHERICAL = 1;
	constexpr ForceType FORCE_UNKNOWN = 2;

	enum class KeyType : uint_fast32_t
	{
		LINEAR			= 1,
		QUADRATIC		= 2,
		TBC				= 3,
		XYZ_ROTATION	= 4,
		CONSTANT		= 5,
	};

	template<typename T>
	struct Key
	{
		float key;
		T value;
	};

	template<typename T>
	struct Tangent
	{
		T forward;
		T backward;
	};

	struct TBC
	{
		float tension;
		float bias;
		float continuity;
	};

	template<typename T> constexpr bool operator==(const Key<T>& lhs, const Key<T>& rhs)
	{
		return lhs.key == rhs.key && lhs.value == rhs.value;
	}
	template<typename T> constexpr bool operator!=(const Key<T>& lhs, const Key<T>& rhs) { return !(lhs == rhs); }
	template<typename T> constexpr bool operator==(const Tangent<T>& lhs, const Tangent<T>& rhs)
	{
		return lhs.forward == rhs.forward && lhs.backward == rhs.backward;
	}
	template<typename T> constexpr bool operator!=(const Tangent<T>& lhs, const Tangent<T>& rhs) { return !(lhs == rhs); }
	constexpr bool operator==(const TBC& lhs, const TBC& rhs)
	{
		return lhs.tension == rhs.tension && lhs.bias == rhs.bias && lhs.continuity == rhs.continuity;
	}
	constexpr bool operator!=(const TBC& lhs, const TBC& rhs) { return !(lhs == rhs); }

	namespace native
	{
		using ColRGBA = Niflib::Color4;

		using NiObject = Niflib::NiObject;
		using NiObjectNET = Niflib::NiObjectNET;
		using NiAVObject = Niflib::NiAVObject;

		using NiTimeController = Niflib::NiTimeController;
		using NiSingleInterpController = Niflib::NiSingleInterpController;
		using NiInterpolator = Niflib::NiInterpolator;
		using NiBoolInterpolator = Niflib::NiBoolInterpolator;
		using NiBoolData = Niflib::NiBoolData;
		using NiFloatInterpolator = Niflib::NiFloatInterpolator;
		using NiFloatData = Niflib::NiFloatData;
		using NiBlendBoolInterpolator = Niflib::NiBlendBoolInterpolator;
		using NiBlendFloatInterpolator = Niflib::NiBlendFloatInterpolator;

		using NiNode = Niflib::NiNode;
		using BSFadeNode = Niflib::BSFadeNode;
		using NiBillboardNode = Niflib::NiBillboardNode;

		using NiParticleSystem = Niflib::NiParticleSystem;
		using NiPSysData = Niflib::NiPSysData;

		using NiPSysModifier = Niflib::NiPSysModifier;
		using NiPSysModifierCtlr = Niflib::NiPSysModifierCtlr;

		using BSPSysScaleModifier = Niflib::BSPSysScaleModifier;
		using BSPSysSimpleColorModifier = Niflib::BSPSysSimpleColorModifier;
		using NiPSysGravityModifier = Niflib::NiPSysGravityModifier;
		using NiPSysRotationModifier = Niflib::NiPSysRotationModifier;

		using NiPSysEmitter = Niflib::NiPSysEmitter;
		using NiPSysVolumeEmitter = Niflib::NiPSysVolumeEmitter;
		using NiPSysBoxEmitter = Niflib::NiPSysBoxEmitter;
		using NiPSysCylinderEmitter = Niflib::NiPSysCylinderEmitter;
		using NiPSysSphereEmitter = Niflib::NiPSysSphereEmitter;
		using NiPSysEmitterCtlr = Niflib::NiPSysEmitterCtlr;

		using NiPSysPositionModifier = Niflib::NiPSysPositionModifier;
		using NiPSysAgeDeathModifier = Niflib::NiPSysAgeDeathModifier;

		using NiPSysBoundUpdateModifier = Niflib::NiPSysBoundUpdateModifier;
		using NiPSysUpdateCtlr = Niflib::NiPSysUpdateCtlr;

		using NiProperty = Niflib::NiProperty;
		using BSEffectShaderProperty = Niflib::BSEffectShaderProperty;
		using NiAlphaProperty = Niflib::NiAlphaProperty;

		using NiExtraData = Niflib::NiExtraData;
		using NiStringExtraData = Niflib::NiStringExtraData;

		using ForceType = Niflib::ForceType;
		using float3_t = Niflib::Vector3;

		using translation_t = Niflib::Vector3;
		using rotation_t = Niflib::Matrix33;
		using scale_t = float;

		using KeyType = Niflib::KeyType;
	}

	template<typename T>
	struct NifConverter
	{
		template<typename From>
		static T convert(From&& f) { return util::DefaultConverter<T>::convert(f); }
	};

	template<typename T>
	using nif_type_conversion = util::type_conversion<T, NifConverter<T>>;

	template<>
	struct NifConverter<std::array<float, 3>>
	{
		static std::array<float, 3> convert(const Niflib::Color3& f);
		static std::array<float, 3> convert(const Niflib::Vector3& f);
	};

	template<>
	struct NifConverter<std::array<float, 4>>
	{
		static std::array<float, 4> convert(const Niflib::Color4& f);
		static std::array<float, 4> convert(const Niflib::Vector4& f);
	};

	template<>
	struct NifConverter<math::Rotation>
	{
		static math::Rotation convert(const Niflib::Matrix33& f);
	};

	template<>
	struct NifConverter<math::Quaternion>
	{
		static math::Quaternion convert(const Niflib::Quaternion& f);
	};

	template<>
	struct NifConverter<Niflib::Color3>
	{
		static Niflib::Color3 convert(const std::array<float, 3>& f);
	};

	template<>
	struct NifConverter<Niflib::Color4>
	{
		static Niflib::Color4 convert(const std::array<float, 4>& f);
	};

	template<>
	struct NifConverter<Niflib::Vector3>
	{
		static Niflib::Vector3 convert(const std::array<float, 3>& f);
	};

	template<>
	struct NifConverter<Niflib::Vector4>
	{
		static Niflib::Vector4 convert(const std::array<float, 4>& f);
	};

	template<>
	struct NifConverter<Niflib::Matrix33>
	{
		static Niflib::Matrix33 convert(const math::Rotation& f);
	};

	template<>
	struct NifConverter<Niflib::Quaternion>
	{
		static Niflib::Quaternion convert(const math::Quaternion& f);
	};

	template<>
	struct NifConverter<Niflib::BillboardMode>
	{
		static Niflib::BillboardMode convert(BillboardMode f);
	};

	template<>
	struct NifConverter<Niflib::ForceType>
	{
		static Niflib::ForceType convert(ForceType f);
	};

	template<>
	struct NifConverter<Niflib::KeyType>
	{
		static Niflib::KeyType convert(KeyType f);
	};

	template<>
	struct NifConverter<KeyType>
	{
		static KeyType convert(Niflib::KeyType f);
	};

	template<>
	struct NifConverter<std::array<int, 2>>
	{
		static std::array<int, 2> convert(const std::vector<nif::SubtextureOffset>& offsets);
	};

	template<>
	struct NifConverter<std::vector<nif::SubtextureOffset>>
	{
		static std::vector<nif::SubtextureOffset> convert(const std::array<int, 2>& count);
	};
}


template<>
struct util::array_traits<Niflib::Vector3>
{
	using array_type = Niflib::Vector3;
	using element_type = float;
	constexpr static size_t size = 3;
	static element_type& at(array_type& t, size_t i);
};
