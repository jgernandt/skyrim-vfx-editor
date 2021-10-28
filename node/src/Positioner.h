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
#include "Eigen\Core"

namespace node
{
	//Responsible for positioning nodes after loading a file
	class Positioner
	{
	public:
		Positioner() = default;
		~Positioner() = default;

		Eigen::VectorXf solve(const Eigen::MatrixXf& connectivity);

	private:
		void distance(const Eigen::VectorXf& x, Eigen::ArrayXXf& r_r2, Eigen::ArrayXXf& r_r2inv);
		float eval(const Eigen::MatrixXf& r2, const Eigen::MatrixXf& r2inv, const Eigen::MatrixXf& C);
		float feval(const Eigen::VectorXf& x, const Eigen::MatrixXf& C);
		Eigen::VectorXf gradient(const Eigen::VectorXf& x, const Eigen::ArrayXXf& r2inv, const Eigen::MatrixXf& C);
		float lineSearch(const Eigen::VectorXf& x, const Eigen::VectorXf& s, const Eigen::MatrixXf& C, float& fval);

	};
}