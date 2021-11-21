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
