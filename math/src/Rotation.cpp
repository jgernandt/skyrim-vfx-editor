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

#include "pch.h"
#include "Rotation.h"
#include "constants.h"

constexpr std::array<size_t, 3> axisOrder(math::EulerOrder order)
{
	switch (order) {
	case math::EulerOrder::XYZ:
		return { 0, 1, 2 };
	case math::EulerOrder::YZX:
		return { 1, 2, 0 };
	case math::EulerOrder::ZXY:
		return { 2, 0, 1 };
	case math::EulerOrder::XZY:
		return { 0, 2, 1 };
	case math::EulerOrder::YXZ:
		return { 1, 0, 2 };
	case math::EulerOrder::ZYX:
		return { 2, 1, 0 };
	default:
		return { 0, 0, 0 };
	}
}
constexpr int handedness(math::EulerOrder order)
{
	switch (order) {
	case math::EulerOrder::XYZ:
		return -1;
	case math::EulerOrder::YZX:
		return -1;
	case math::EulerOrder::ZXY:
		return -1;
	case math::EulerOrder::XZY:
		return 1;
	case math::EulerOrder::YXZ:
		return 1;
	case math::EulerOrder::ZYX:
		return 1;
	default:
		return 1;
	}
}

math::Rotation::euler_type math::Rotation::getEuler(EulerOrder order) const
{
	euler_type result;
	result.order = order;
	auto axis = axisOrder(order);
	int sign = handedness(order);

	float k = 180.0f / pi<float>;
	float a = 2.0f * (Q.s * Q.v[axis[1]] + sign * Q.v[axis[0]] * Q.v[axis[2]]);

	//Machine rounding is an issue here. A robust solution would be to check the resulting angles afterward,
	//but this arbitrary tolerance works for now.
	if (std::abs(a) >= 1.0f || 1.0f - std::abs(a) < 1.0e-5f) {
		//singularity
		result[axis[0]] = degf(2.0f * k * std::atan2(Q.v[axis[0]], Q.s));//someone wanted +/- here, but that seems wrong
		result[axis[1]] = degf(std::copysign(90.0f, a));
		result[axis[2]] = degf(0.0f);
	}
	else {
		result[axis[0]] = degf(k * std::atan2(
			2.0f * (Q.s * Q.v[axis[0]] - sign * Q.v[axis[1]] * Q.v[axis[2]]),
			1.0f - 2.0f * (Q.v[axis[0]] * Q.v[axis[0]] + Q.v[axis[1]] * Q.v[axis[1]])));
		result[axis[1]] = degf(k * std::asin(a));
		result[axis[2]] = degf(k * std::atan2(
			2.0f * (Q.s * Q.v[axis[2]] - sign * Q.v[axis[0]] * Q.v[axis[1]]),
			1.0f - 2.0f * (Q.v[axis[1]] * Q.v[axis[1]] + Q.v[axis[2]] * Q.v[axis[2]])));
	}

	return result;
}

math::Rotation& math::Rotation::setEuler(const euler_type& v)
{
	auto axis = axisOrder(v.order);
	int sign = handedness(v.order);

	float c1 = std::cos(0.5f * radf(v[axis[0]]).value);
	float c2 = std::cos(0.5f * radf(v[axis[1]]).value);
	float c3 = std::cos(0.5f * radf(v[axis[2]]).value);
	float s1 = std::sin(0.5f * radf(v[axis[0]]).value);
	float s2 = std::sin(0.5f * radf(v[axis[1]]).value);
	float s3 = std::sin(0.5f * radf(v[axis[2]]).value);
	/*float k = pi<float> / 360.0f;
	float c1 = std::cos(k * v[axis[0]]);
	float c2 = std::cos(k * v[axis[1]]);
	float c3 = std::cos(k * v[axis[2]]);
	float s1 = std::sin(k * v[axis[0]]);
	float s2 = std::sin(k * v[axis[1]]);
	float s3 = std::sin(k * v[axis[2]]);*/

	Q.s = c3 * c2 * c1 - sign * s3 * s2 * s1;
	Q.v[axis[0]] = c3 * c2 * s1 + sign * s3 * s2 * c1;
	Q.v[axis[1]] = c3 * s2 * c1 - sign * s3 * c2 * s1;
	Q.v[axis[2]] = s3 * c2 * c1 + sign * c3 * s2 * s1;

	return *this;
}

math::Rotation::matrix_type math::Rotation::getMatrix() const
{
	assert(false && "Not implemented yet");
	//return Eigen::Matrix3f::Identity();
	return matrix_type();
}

math::Rotation& math::Rotation::setMatrix(const matrix_type& A)
{
	assert(false && "Not implemented yet");
	return *this;
}

math::Rotation::quat_type math::Rotation::getQuaternion() const
{
	return Q;
}

math::Rotation& math::Rotation::setQuaternion(const quat_type& q)
{
	Q = q;
	return *this;
}
