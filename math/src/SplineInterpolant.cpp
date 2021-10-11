//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "SplineInterpolant.h"

/*namespace general
{
	float v(int k, size_t i, float x, const Eigen::VectorXf& t)
	{
		return (x - t[i]) / (t[i + k] - t[i]);
	}

	float BSPLV(int k, size_t i, float x, const Eigen::VectorXf& t)
	{
		//t must be padded with k arbitrary ascending values in both ends
		assert(t.size() > k + 1 && i < t.size() - 1 - k);//probably messed up...
		i = t.size() - k - 2;
		if (k == 0)
			return x >= t[i] && x < t[i + 1] ? 1.0f : 0.0f;
		else
			return v(k, i, x, t) * BSPLV(k - 1, i, x, t) + (1 - v(k, i + 1, x, t)) * BSPLV(k - 1, i + 1, x, t);
	}
}*/

namespace special//assume constant knot spacing h (x is now t - t_i)
{
	constexpr float v(int k, float x, float h)
	{
		return x / (k * h);
	}

	float BSPLV(int k, float t, float h)
	{
		if (k == 0)
			return t >= 0.0f && t < h ? 1.0f : 0.0f;
		else
			return v(k, t, h)* BSPLV(k - 1, t, h) + (1 - v(k, t - h, h)) * BSPLV(k - 1, t - h, h);
	}
}

math::SplineInterpolant::SplineInterpolant(const Eigen::VectorXf& y) : m_y{ y }
{
	calculate();
}

void math::SplineInterpolant::calculate()
{
	int N = static_cast<int>(m_y.size());

	Eigen::VectorXf y(N + 2);
	//y << m_y.front(), m_y, m_y.back();
	y << 0.0f, m_y, 0.0f;

	Eigen::MatrixXf A = Eigen::MatrixXf::Zero(y.size(), y.size());
	A.diagonal() = Eigen::VectorXf::Constant(y.size(), 0.666667f);
	A.diagonal(1) = Eigen::VectorXf::Constant(y.size() - 1, 0.166667f);
	A.diagonal(-1) = Eigen::VectorXf::Constant(y.size() - 1, 0.166667f);

	//This is the requirement of zero curvature at the endpoints
	float f = static_cast<float>((N - 1) * (N - 1));
	A(0, 0) = f;
	A(0, 1) = -2.0f * f;
	A(0, 2) = f;
	A(y.size() - 1, y.size() - 1) = f;
	A(y.size() - 1, y.size() - 2) = -2.0f * f;
	A(y.size() - 1, y.size() - 3) = f;

	m_coefs = A.colPivHouseholderQr().solve(y);
}

Eigen::VectorXf math::SplineInterpolant::eval(const Eigen::VectorXf& x)
{
	assert(m_coefs.size() > 3);

	//Step length in the interpolant
	float h = 1.0f / (m_coefs.size() - 3);

	//knot vector (implied)
	Eigen::VectorXf t = Eigen::VectorXf::LinSpaced(m_coefs.size() - 2, 0.0f, 1.0f);

	Eigen::VectorXf ret = Eigen::VectorXf::Zero(x.size());
	for (int i = 0; i < ret.size(); i++) {
		assert(x[i] >= 0.0f && x[i] <= 1.0f);

		if (x[i] == 0.0f)
			ret[i] = m_y[0];
		else if (x[i] == 1.0f)
			ret[i] = m_y[m_y.size() - 1];
		else {
			int first_spline_index = static_cast<int>(x[i] / h);//we want this and the following three
			assert(first_spline_index < m_coefs.size() - 3);

			float f = std::fmod(x[i], h);
			for (int j = 0; j < 4; j++)
				ret[i] += m_coefs[first_spline_index + j] * special::BSPLV(3, f + (3 - j) * h, h);
		}
	}

	return std::move(ret);
}
