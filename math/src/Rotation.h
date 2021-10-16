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
#include <array>
#include "Angles.h"
//#include "Eigen/Core"//we'll introduce this later

namespace math
{
	struct Quaternion
	{
		Quaternion(float s = 1.0f, float vx = 0.0f, float vy = 0.0f, float vz = 0.0f) :
			s{ s }, v{ vx, vy, vz } {}
		float s;
		std::array<float, 3> v;
		//Eigen::Vector3f v{ 0.0f, 0.0f, 0.0f };

		friend bool operator==(const Quaternion& l, const Quaternion& r) { return l.s == r.s && l.v == r.v; }
		friend bool operator!=(const Quaternion& l, const Quaternion& r) { return !(l == r); }
	};

	enum class EulerOrder
	{
		XYZ,//RH (-1)
		XZY,//LH (+1)
		YXZ,//LH (+1)
		YZX,//RH (-1)
		ZXY,//RH (-1)
		ZYX,//LH (+1)
	};

	//Opaque type that represents a pure rotation
	class Rotation
	{
	public:
		struct EulerAngles
		{
			std::array<deg, 3> value{ 0.0f, 0.0f, 0.0f };
			EulerOrder order{ EulerOrder::XYZ };

			constexpr deg& operator[](size_t i) { return value[i]; }
			constexpr const deg& operator[](size_t i) const { return value[i]; }

			constexpr static size_t size() { return 3; }

			friend bool operator==(const EulerAngles& l, const EulerAngles& r)
			{
				return l.order == r.order && l.value == r.value;
			}
			friend bool operator!=(const EulerAngles& l, const EulerAngles& r) { return !(l == r); }
		};
		using euler_type = EulerAngles;
		using matrix_type = std::array<std::array<float, 3>, 3>;
		using quat_type = Quaternion;
		//using euler_type = Eigen::Vector3f;
		//using matrix_type = Eigen::Matrix3f;

	public:
		Rotation() {}
		Rotation(const euler_type& euler) { setEuler(euler); }

		//Euler arrays denote angles around x, y, z in the reference frame.
		//However, the order in which those rotations are applied may be specified separately.
		euler_type getEuler(EulerOrder order = EulerOrder::XYZ) const;
		Rotation& setEuler(const euler_type& v);

		matrix_type getMatrix() const;
		Rotation& setMatrix(const matrix_type& A);

		quat_type getQuaternion() const;
		Rotation& setQuaternion(const quat_type& q);

		friend bool operator==(const Rotation& l, const Rotation& r) { return l.Q == r.Q; }
		friend bool operator!=(const Rotation& l, const Rotation& r) { return !(l == r); }

	private:
		quat_type Q;
	};


}

#include "traits.h"
template<>
struct util::array_traits<math::Rotation::EulerAngles>
{
	using array_type = math::Rotation::EulerAngles;
	using element_type = float;
	constexpr static size_t size = math::Rotation::EulerAngles::size();
	constexpr static element_type& at(array_type& t, size_t i) { return t.value[i].value; }
	constexpr static const element_type& at(const array_type& t, size_t i) { return t.value[i].value; }
};