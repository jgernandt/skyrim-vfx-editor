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

	using KeyType = uint_fast32_t;
	constexpr KeyType KEY_LINEAR		= 1;
	constexpr KeyType KEY_QUADRATIC		= 2;
	constexpr KeyType KEY_TBC			= 3;
	constexpr KeyType KEY_XYZ_ROTATION	= 4;
	constexpr KeyType KEY_CONSTANT		= 5;

	using ShaderFlags = std::uint_fast32_t;
	constexpr ShaderFlags SF1_PALETTE_COLOUR = 0x00000010;
	constexpr ShaderFlags SF1_PALETTE_ALPHA = 0x00000020;
	constexpr ShaderFlags SF1_ZBUFFER_TEST = 0x80000000;

	constexpr ShaderFlags SF2_DOUBLE_SIDED = 0x00000010;
	constexpr ShaderFlags SF2_VERTEX_COLOUR = 0x00000020;

	//Forward declare all object types
	struct NiObject;
	struct NiObjectNET;
	struct NiAVObject;
	struct NiNode;
	struct BSFadeNode;

	struct NiExtraData;
	struct NiStringExtraData;

	struct NiTimeController;

	struct NiProperty;
	struct NiAlphaProperty;
	struct BSEffectShaderProperty;

	struct NiBoolData;
	struct NiFloatData;
	//etc.

	template<typename T>
	struct NifConverter
	{
		template<typename From>
		static T convert(From&& f) { return util::DefaultConverter<T>::convert(f); }
	};

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
		static math::Rotation convert(const Niflib::Quaternion& f);
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
		constexpr static Niflib::BillboardMode convert(BillboardMode val)
		{
			return static_cast<Niflib::BillboardMode>(val);
		}
	};

	template<>
	struct NifConverter<Niflib::ForceType>
	{
		constexpr static Niflib::ForceType convert(ForceType val)
		{
			return static_cast<Niflib::ForceType>(val);
		}
	};

	template<>
	struct NifConverter<Niflib::KeyType>
	{
		constexpr static Niflib::KeyType convert(KeyType val)
		{
			return static_cast<Niflib::KeyType>(val);
		}
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

	template<>
	struct NifConverter<Niflib::SkyrimShaderPropertyFlags1>
	{
		constexpr static Niflib::SkyrimShaderPropertyFlags1 convert(ShaderFlags flags)
		{
			return static_cast<Niflib::SkyrimShaderPropertyFlags1>(flags);
		}
	};
	template<>
	struct NifConverter<Niflib::SkyrimShaderPropertyFlags2>
	{
		constexpr static Niflib::SkyrimShaderPropertyFlags2 convert(ShaderFlags flags)
		{
			return static_cast<Niflib::SkyrimShaderPropertyFlags2>(flags);
		}
	};
	template<>
	struct NifConverter<std::uint_fast32_t>
	{
		constexpr static std::uint_fast32_t convert(Niflib::SkyrimShaderPropertyFlags1 val)
		{
			return static_cast<std::uint_fast32_t>(val);
		}
		constexpr static std::uint_fast32_t convert(Niflib::SkyrimShaderPropertyFlags2 val)
		{
			return static_cast<std::uint_fast32_t>(val);
		}
		constexpr static std::uint_fast32_t convert(Niflib::KeyType val)
		{
			return static_cast<std::uint_fast32_t>(val);
		}
		constexpr static std::uint_fast32_t convert(Niflib::ForceType val)
		{
			return static_cast<std::uint_fast32_t>(val);
		}
	};
}

template<typename T>
using nif_type_conversion = util::type_conversion<T, nif::NifConverter<T>>;

template<>
struct util::array_traits<Niflib::Vector3>
{
	using array_type = Niflib::Vector3;
	using element_type = float;
	constexpr static size_t size = 3;
	static element_type& at(array_type& t, size_t i);
};
