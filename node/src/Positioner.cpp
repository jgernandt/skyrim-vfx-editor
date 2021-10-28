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
#include <random>
#include "Positioner.h"

constexpr float k = 10.0f;

Eigen::VectorXf node::Positioner::solve(const Eigen::MatrixXf& C)
{
	using namespace Eigen;

	int N = C.rows();

	VectorXf x(2 * N);
	std::mt19937 mt;
	std::uniform_real_distribution<float> D(0.0f, 10.0f);
	x(0) = 0.0f;
	x(N) = 0.0f;
 	for (int i = 1; i < N; i++) {
		x(i) = D(mt);
		x(i + N) = D(mt);
	}

	//When solving, we should disregard "variable" 0 and N (leave them at 0.0).

	//Initial approximation of the Hessian
	MatrixXf B = MatrixXf::Identity(2 * N - 2, 2 * N - 2);

	ArrayXXf r2(N, N);
	ArrayXXf rm2(N, N);
	distance(x, r2, rm2);

	VectorXf grad = gradient(x, rm2, C);

	//Function value
	float f = eval(r2, rm2, C);

	int count = 0;
	while (count < 1000) {
	//for (int n = 0; n < 10; n++) {
		count++;

		//Optimisations:
		//*Update the factorisation instead of recalcing
		//*Faster line search method
		//*Exploit symmetry of distance matrix

		//Solve for direction
		VectorXf s = B.llt().solve(-grad);

		//Perform line search
		VectorXf S(2 * N);
		S << 0.0f, s.head(N - 1), 0.0f, s.tail(N - 1);
		float alpha = lineSearch(x, S, C, f);
		s *= alpha;

		//Update solution
		x(seq(1, N - 1)) += s.head(N - 1);
		x(seq(N + 1, 2 * N - 1)) += s.tail(N - 1);

		distance(x, r2, rm2);
		float nextF = eval(r2, rm2, C);
		VectorXf nextGrad = gradient(x, rm2, C);
		VectorXf y = nextGrad - grad;

		float denom = s.transpose() * B * s;
		if (s.squaredNorm() < f * 1.0e-6f / denom)
			break;
		MatrixXf nextB = B + (y * y.transpose()) / (y.transpose() * s) - (B * s * s.transpose() * B) / denom;

		grad = nextGrad;
		B = nextB;
	}

	return 100.0f * x;
}

void node::Positioner::distance(const Eigen::VectorXf& x, Eigen::ArrayXXf& r_r2, Eigen::ArrayXXf& r_r2inv)
{
	using namespace Eigen;

	//We do double the required work, since this matrix is symmetric. Optimise later.
	int N = x.size() / 2;
	for (int i = 0; i < N; i++) {
		r_r2.row(i) = (x(i) - x.head(N).array()).square() + (x(i + N) - x.tail(N).array()).square();
		r_r2inv.row(i) = 1.0f / r_r2.row(i);
	}
	//This is just destroying terms that should be excluded (but we include anyway for vectorisation purposes)
	r_r2inv.matrix().diagonal() = ArrayXf::Zero(N);
}

float node::Positioner::eval(const Eigen::MatrixXf& r2, const Eigen::MatrixXf& r2inv, const Eigen::MatrixXf& C)
{
	using namespace Eigen;

	float result = 0.0f;
	int N = C.rows();
	for (int i = 0; i < N; i++)
		result += C(i, seq(i, N - 1)) * r2(seq(i, N - 1), i).matrix() + k * r2inv(i, seq(i, N - 1)).sum();
	return result;
}

float node::Positioner::feval(const Eigen::VectorXf& x, const Eigen::MatrixXf& C)
{
	using namespace Eigen;

	int N = C.rows();
	ArrayXXf r2(N, N);
	ArrayXXf rm2(N, N);
	distance(x, r2, rm2);
	return eval(r2, rm2, C);
}

Eigen::VectorXf node::Positioner::gradient(const Eigen::VectorXf& x, const Eigen::ArrayXXf& r2inv, const Eigen::MatrixXf& C)
{
	using namespace Eigen;

	int N = C.rows();
	VectorXf grad(2 * N - 2);
	MatrixXf K = C - k * r2inv.square().matrix();
	for (int i = 1; i < N; i++) {
		grad(i - 1) = 2.0f * (x(i) - x.head(N).array()).matrix().transpose() * K.col(i);
		grad(i + N - 2) = 2.0f * (x(i + N) - x.tail(N).array()).matrix().transpose() * K.col(i);
	}
	return grad;
}

float node::Positioner::lineSearch(const Eigen::VectorXf& x, const Eigen::VectorXf& s, const Eigen::MatrixXf& C, float& fval)
{
	float tau = 0.61803399f;
	float a = 0.0f;
	float fa = fval;
	float b = 1.0f;
	float fb = feval(x + s, C);

	float x1 = a + (1 - tau) * (b - a);
	float f1 = feval(x + x1 * s, C);
	float x2 = a + tau * (b - a);
	float f2 = feval(x + x2 * s, C);

	while ((b - a) > 0.01f) {
		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = a + tau * (b - a);
			f2 = feval(x + x2 * s, C);
		}
		else {
			b = x2;
			x2 = x1;
			f2 = f1;
			x1 = a + (1 - tau) * (b - a);
			f1 = feval(x + x1 * s, C);
		}
	}

	if (f1 < f2) {
		fval = f1;
		return x1;
	}
	else {
		fval = f2;
		return x2;
	}
}
