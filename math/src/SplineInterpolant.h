//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include "Eigen\Dense"

namespace math
{
	//This is currently the very specific case of a cubic spline on evenly spaced knots in 
	//the interval [0, 1]. We could make it more general if we ever need to.
	class SplineInterpolant
	{
	public:
		SplineInterpolant(const Eigen::VectorXf& y);

		Eigen::VectorXf eval(const Eigen::VectorXf& x);

	private:
		void calculate();

	private:
		Eigen::VectorXf m_y;
		Eigen::VectorXf m_coefs;
	};
}

