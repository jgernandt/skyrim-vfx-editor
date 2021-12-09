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
#include "nif_backend.h"
#include "nif_types.h"
#include "type_conversion.h"

namespace nif
{
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
	struct NifConverter<unsigned int>
	{
		constexpr static unsigned int convert(Niflib::BillboardMode val)
		{
			return static_cast<unsigned int>(val);
		}
		constexpr static unsigned int convert(Niflib::SkyrimShaderPropertyFlags1 val)
		{
			return static_cast<unsigned int>(val);
		}
		constexpr static unsigned int convert(Niflib::SkyrimShaderPropertyFlags2 val)
		{
			return static_cast<unsigned int>(val);
		}
		constexpr static unsigned int convert(Niflib::KeyType val)
		{
			return static_cast<unsigned int>(val);
		}
		constexpr static unsigned int convert(Niflib::ForceType val)
		{
			return static_cast<unsigned int>(val);
		}
	};
}

template<typename T>
using nif_type_conversion = util::type_conversion<T, nif::NifConverter<T>>;
