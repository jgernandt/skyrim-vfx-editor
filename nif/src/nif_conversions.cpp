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
#include "nif_conversions.h"

std::array<float, 3> nif::NifConverter<std::array<float, 3>>::convert(const Niflib::Color3& f)
{
	return { f.r, f.g, f.b };
}

std::array<float, 3> nif::NifConverter<std::array<float, 3>>::convert(const Niflib::Vector3& f)
{
	return { f.x, f.y, f.z };
}

std::array<float, 4> nif::NifConverter<std::array<float, 4>>::convert(const Niflib::Color4& f)
{
	return { f.r, f.g, f.b, f.a };
}

std::array<float, 4> nif::NifConverter<std::array<float, 4>>::convert(const Niflib::Vector4& f)
{
	return { f.x, f.y, f.z, f.w };
}

math::Rotation nif::NifConverter<math::Rotation>::convert(const Niflib::Matrix33& f)
{
	math::Rotation r;
	return r.setQuaternion(util::type_conversion<math::Quaternion, NifConverter<math::Quaternion>>::from(f.AsQuaternion()));
}

math::Rotation nif::NifConverter<math::Rotation>::convert(const Niflib::Quaternion& q)
{
	math::Rotation r;
	return r.setQuaternion(util::type_conversion<math::Quaternion, NifConverter<math::Quaternion>>::from(q));
}

math::Quaternion nif::NifConverter<math::Quaternion>::convert(const Niflib::Quaternion& q)
{
	return math::Quaternion(q.w, q.x, q.y, q.z);
}

Niflib::Color3 nif::NifConverter<Niflib::Color3>::convert(const std::array<float, 3>& f)
{
	return Niflib::Color3(f[0], f[1], f[2]);
}

Niflib::Color4 nif::NifConverter<Niflib::Color4>::convert(const std::array<float, 4>& f)
{
	return Niflib::Color4(f[0], f[1], f[2], f[3]);
}

Niflib::Vector3 nif::NifConverter<Niflib::Vector3>::convert(const std::array<float, 3>& f)
{
	return Niflib::Vector3(f[0], f[1], f[2]);
}

Niflib::Vector4 nif::NifConverter<Niflib::Vector4>::convert(const std::array<float, 4>& f)
{
	return Niflib::Vector4(f[0], f[1], f[2], f[3]);
}

Niflib::Matrix33 nif::NifConverter<Niflib::Matrix33>::convert(const math::Rotation& r)
{
	Niflib::Quaternion Q = util::type_conversion<Niflib::Quaternion, NifConverter<Niflib::Quaternion>>::from(r.getQuaternion());
	//Not sure why, but we need to transpose here. I'm probably just confused about the direction of the rotation
	//(it's the same if we go the matrix way directly).
	return Q.Inverse().AsMatrix();
	//return Q.AsMatrix();
}

Niflib::Quaternion nif::NifConverter<Niflib::Quaternion>::convert(const math::Quaternion& q)
{
	return Niflib::Quaternion(q.s, q.v[0], q.v[1], q.v[2]);
}
