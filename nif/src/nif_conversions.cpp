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

static bool equalToWithin(float l, float r, float relTol)
{
	float absTol = std::min(abs(l), abs(r)) * relTol;
	return abs(l - r) <= absTol;
}


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


std::array<int, 2> nif::NifConverter<std::array<int, 2>>::convert(const std::vector<nif::SubtextureOffset>& offsets)
{
	std::array<int, 2> result{ 0, 0 };

	if (offsets.empty())
		result = { 1, 1 };
	else {
		//Use the dims of first rectangle to determine what numbers we expect. Then verify that they add up.
		//offsets[0][1];//width
		//offsets[0][3];//height

		//First, make sure the dims are rationals, to within some tolerance.
		//We cannot have high expectations about precision. The data may have been filled out by hand.
		float tol = 1e-2f;
		float x_raw = 1.0f / offsets[0][1];
		float y_raw = 1.0f / offsets[0][3];
		float x_rounded = std::round(x_raw);
		float y_rounded = std::round(y_raw);

		if (equalToWithin(x_raw, x_rounded, tol) && equalToWithin(y_raw, y_rounded, tol)) {
			//We know what subtexture count to expect. Now test if the offsets match it.
			int x = static_cast<int>(x_rounded);
			int y = static_cast<int>(y_rounded);
			if (offsets.size() == x * y) {
				//We cannot assume anything about the order of the offset vector.
				//Fill out another vector with the offsets we expect:
				auto exp = nif_type_conversion<std::vector<nif::SubtextureOffset>>::from(std::array<int, 2>{ x, y });

				//Then search this vector for each offset, removing it if found
				for (auto& offset : offsets) {
					auto pred = [&offset, tol](const nif::SubtextureOffset& v) {
						return equalToWithin(v[0], offset[0], tol) &&
							equalToWithin(v[1], offset[1], tol) &&
							equalToWithin(v[2], offset[2], tol) &&
							equalToWithin(v[3], offset[3], tol);
					};
					if (auto it = std::find_if(exp.begin(), exp.end(), pred); it != exp.end())
						exp.erase(it);
				}

				//The vector of expected offsets will be empty iff it was an exact match (to within a tolerance)
				if (exp.empty())
					result = { x, y };
			}
		}
	}

	return result;
}

std::vector<nif::SubtextureOffset> nif::NifConverter<std::vector<nif::SubtextureOffset>>::convert(const std::array<int, 2>& count)
{
	std::vector<nif::SubtextureOffset> result;

	if (count[0] > 0 && count[1] > 0 && !(count[0] == 1 && count[1] == 1)) {
		result.resize(count[0] * count[1]);
		for (size_t i = 0; i < result.size(); i++) {
			int col = i % count[0];
			int row = i / count[0];
			float w = 1.0f / count[0];
			float h = 1.0f / count[1];
			result[i] = { col * w, w, row * h, h };
		}
	}

	return result;
}
