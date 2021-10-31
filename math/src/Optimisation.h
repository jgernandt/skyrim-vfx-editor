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


//This is an adaptation of an algorithm from GSL by Brian Gough

#pragma once
#include <memory>
#include <mutex>
#include "Eigen/Core"

namespace math
{
	class Quadratic
	{
	public:
		//Construct from coefficients
		Quadratic(double c0 = 1.0, double c1 = 1.0, double c2 = 1.0) : m_c{ c0, c1, c2 } {}

		//Construct from value and derivative at 0 and value at 1
		Quadratic(const Eigen::Vector2d& p0, double f1) : m_c{ p0[0], p0[1], f1 - p0[0] - p0[1] } {}

		double eval(double x) const { return m_c[0] + x * (m_c[1] + x * m_c[2]); }
		double curvature() const { return 2.0 * m_c[2]; }
		double extremum() const { return -m_c[1] / curvature(); }
		int roots(double& r1, double& r2) const;

		//Return the location of the local minimum on [a, b]
		double localMin(double a, double b) const;

	private:
		double m_c[3];
	};

	class Cubic
	{
	public:
		//Construct from coefficients
		Cubic(double c0 = 1.0, double c1 = 1.0, double c2 = 1.0, double c3 = 1.0) : m_c{ c0, c1, c2, c3 } {}

		//Construct from value and derivative at 0 and 1
		Cubic(const Eigen::Vector2d& p0, const Eigen::Vector2d& p1);

		double eval(double x) const { return m_c[0] + x * (m_c[1] + x * (m_c[2] + x * m_c[3])); }
		Quadratic derivative() const { return Quadratic(m_c[1], 2.0 * m_c[2], 3.0 * m_c[3]); }

		//Return the location of the local minimum on [a, b]
		double localMin(double a, double b) const;

	private:
		double m_c[4];
	};

	namespace opt
	{

		enum class Status
		{
			SUCCESS,
			CONTINUE,
			NO_PROGRESS,
		};

		//patterns
		struct OneDimFunction
		{
			void eval(double x, double& r_f, double& r_df) {}
			void fval(double x, double& r_f) {}
			void grad(double x, double& r_df) {}
		};
		struct MultiDimFunction
		{
			void eval(const Eigen::VectorXd& x, double& r_f, Eigen::VectorXd& r_grad) {}
			void fval(const Eigen::VectorXd& x, double& r_f) {}
			void grad(const Eigen::VectorXd& x, Eigen::VectorXd& r_grad) {}
		};

		//Wraps a mutli-dim function in the interface of a single-dim function
		template<typename MultiDimType>
		class OneDimWrapper
		{
		public:
			OneDimWrapper(MultiDimType& fcn, const Eigen::VectorXd& x, const Eigen::VectorXd& g, 
				const Eigen::VectorXd& s, double f, double df, Eigen::VectorXd& x_alpha, Eigen::VectorXd& g_alpha) :
				m_fcn{ fcn }, m_x{ x }, m_g{ g }, m_s{ s }, m_fCache{ f }, m_dfCache{ df }, m_xCache{ x_alpha }, m_gCache{ g_alpha }
			{
				m_xCache = m_x;
				m_gCache = m_g;
			}

			void eval(double x, double& r_f, double& r_df) 
			{
				//if just one matches we should recalc just the other
				//(not going to happen unless we add fval and grad)
				if (x != m_fCacheKey || x != m_dfCacheKey) {
					//move to x
					if (x != m_xCacheKey) {
						m_xCache = x * m_s + m_x;
						m_xCacheKey = x;
					}
					m_fcn.eval(m_xCache, m_fCache, m_gCache);
					m_dfCache = m_gCache.transpose() * m_s;

					m_fCacheKey = x;
					m_dfCacheKey = x;
					m_gCacheKey = x;
				}
				r_f = m_fCache;
				r_df = m_dfCache;
			}
			//add if we need
			void fval(double x, double& r_f) { assert(false); }
			void grad(double x, double& r_df) { assert(false); }

			void newDirection(double df)
			{
				//We expect the cache to be up to date, so the only thing to change is df (which has a new direction).
				//We could calculate this ourselves. 
				m_dfCache = df;
				m_fCacheKey = 0.0;
				m_dfCacheKey = 0.0;
				m_xCacheKey = 0.0;
				m_gCacheKey = 0.0;
			}

		private:
			MultiDimType& m_fcn;

			const Eigen::VectorXd& m_x;//point
			const Eigen::VectorXd& m_g;//gradient
			const Eigen::VectorXd& m_s;//step dir

			//Cache the most recent values
			double m_fCache;
			double m_dfCache;
			Eigen::VectorXd& m_xCache;//refs to InternalState
			Eigen::VectorXd& m_gCache;

			//Cache "keys"
			double m_fCacheKey{ 0.0 };
			double m_dfCacheKey{ 0.0 };
			double m_xCacheKey{ 0.0 };
			double m_gCacheKey{ 0.0 };
		};

		//Minimise in single dimension. Used for line search in multimin.
		template<typename FcnType>
		class SingleMin
		{
		public:
			SingleMin(FcnType& fcn) : m_fcn{ fcn } {}

			//alpha should contain initial guess. Will be overwritten on success, unchanged otherwise.
			Status minSearch(double& r_alpha)
			{
				//evaluate starting point
				double alpha_prev = 0.0;
				double f0;
				double fp0;
				m_fcn.eval(alpha_prev, f0, fp0);
				double falpha_prev = f0;
				double fpalpha_prev = fp0;

				//currently tested point
				double alpha = r_alpha;
				double falpha, fpalpha;

				int i_max = 100;

				//Bracket minimum
				int i = 0;
				double a, b;//interval lims
				double fa, fb;//fvals
				double fpa, fpb;//derivs
				while (i++ < i_max) {
					//We evaluate the derivative immediately, since our specific function benefits from it.
					//In general, it could wait until after the first condition has been tested.
					m_fcn.eval(alpha, falpha, fpalpha);

					if (falpha > f0 + alpha * m_params.rho * fp0 || falpha >= falpha_prev) {
						a = alpha_prev; fa = falpha_prev; fpa = fpalpha_prev;
						//I suppose we could use our calced df here?
						b = alpha; fb = falpha; fpb = std::numeric_limits<double>::quiet_NaN();
						break;                /* goto sectioning */
					}

					if (std::abs(fpalpha) <= -m_params.sigma * fp0) {
						r_alpha = alpha;
						return Status::SUCCESS;
					}

					if (fpalpha >= 0) {
						a = alpha; fa = falpha; fpa = fpalpha;
						b = alpha_prev; fb = falpha_prev; fpb = fpalpha_prev;
						break;                /* goto sectioning */
					}

					//extend bracket
					double delta = alpha - alpha_prev;
					double alpha_next = estimateMin(alpha_prev, falpha_prev, fpalpha_prev,
						alpha, falpha, fpalpha, alpha + delta, alpha + m_params.tau1 * delta);

					alpha_prev = alpha;
					falpha_prev = falpha;
					fpalpha_prev = fpalpha;
					alpha = alpha_next;
				}

				//Section bracket
				i = 0;
				while (i++ < i_max) {
					//predict location of minimum and evaluate it
					double delta = b - a;
					alpha = estimateMin(a, fa, fpa, b, fb, fpb, a + m_params.tau2 * delta, b - m_params.tau3 * delta);
					m_fcn.eval(alpha, falpha, fpalpha);

					if ((a - alpha) * fpa <= std::numeric_limits<double>::epsilon()) {
						//roundoff prevents progress
						return Status::NO_PROGRESS;
					}

					if (falpha > f0 + m_params.rho * alpha * fp0 || falpha >= fa) {
						//keep a
						b = alpha; 
						fb = falpha; 
						fpb = std::numeric_limits<double>::quiet_NaN();
					}
					else
					{
						if (std::abs(fpalpha) <= -m_params.sigma * fp0) {
							r_alpha = alpha;
							return Status::SUCCESS;
						}
						
						if ((delta >= 0.0) == (fpalpha >= 0.0)) {
							b = a; 
							fb = fa; 
							fpb = fpa;
						}
						a = alpha;
						fa = falpha;
						fpa = fpalpha;
					}
				}

				//Should we really return success here? Maybe we expect to never reach this point?
				return Status::SUCCESS;
			}

		private:
			//Guess the location of the local minimum on [xmin, xmax] by polynomial interpolation
			double estimateMin(double a, double fa, double fpa,
				double b, double fb, double fpb, double xmin, double xmax)
			{
				/* Map [a,b] to [0,1] */
				double zmin = (xmin - a) / (b - a);
				double zmax = (xmax - a) / (b - a);

				if (zmin > zmax)
					std::swap(zmin, zmax);

				//Fit a polynomial to the (rescaled) data and return its local minimum on [zmin, zmax]
				double z;
				if (std::isfinite(fpb)) {
					Cubic polynomial({ fa, fpa * (b - a) }, { fb, fpb * (b - a) });
					z = polynomial.localMin(zmin, zmax);
				}
				else {
					Quadratic polynomial({ fa, fpa * (b - a) }, fb);
					z = polynomial.localMin(zmin, zmax);
				}

				return a + z * (b - a);
			}

		private:
			FcnType& m_fcn;

			struct Params 
			{
				double rho{ 0.01 };
				double sigma{ 1.0e-4 };
				double tau1{ 9.0 };
				double tau2{ 0.05 };
				double tau3{ 0.5 };
			};

			Params m_params;
		};

		//Stores state and performs iterations.
		//The result vector (and ONLY the result vector) may be accessed synchronously.
		template<typename FcnType>
		class SyncMultiMin
		{
		private:
			//Mimic GSL for now - I think we can drop some of this
			struct InternalState
			{
				double step{ 0.01 };
				double g0norm;
				double pnorm;
				double delta_f{ 0.0 };
				double fp0;                   /* f'(0) for f(x-alpha*p) */
				Eigen::VectorXd x0;
				Eigen::VectorXd g0;
				Eigen::VectorXd p;
				/* work space */
				Eigen::VectorXd dx0;
				Eigen::VectorXd dg0;
				Eigen::VectorXd x_alpha;
				Eigen::VectorXd g_alpha;
				/* wrapper function */
				std::unique_ptr<OneDimWrapper<FcnType>> wrapper;
			};

		public:
			SyncMultiMin(FcnType& fcn, Eigen::VectorXd& r_x, std::mutex& mutex) :
				m_fcn{ fcn }, m_out{ r_x }, m_mutex{ mutex }
			{
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_x = m_out;
				}
				m_N = m_x.size();

				//evaluate initial point
				m_fcn.eval(m_x, m_fval, m_grad);
				assert(m_grad.size() == m_N);
				m_dx = Eigen::VectorXd::Zero(m_N);

				m_state.x0 = m_x;
				m_state.g0 = m_grad;
				m_state.g0norm = m_state.g0.norm();

				//Use negative gradient as initial step dir
				m_state.p = -1.0 / m_state.g0norm * m_grad;
				m_state.pnorm = m_state.p.norm();//should be 1
				m_state.fp0 = -m_state.g0norm;

				m_state.wrapper = std::make_unique<OneDimWrapper<FcnType>>(
					m_fcn, m_state.x0, m_state.g0, m_state.p, m_fval, m_state.fp0, m_state.x_alpha, m_state.g_alpha);
			}

			Status iterate()
			{ 
				if (m_state.pnorm == 0.0 || m_state.g0norm == 0.0 || m_state.fp0 == 0) {
					m_dx = Eigen::VectorXd::Zero(m_N);
					return Status::NO_PROGRESS;
				}

				double alpha;

				if (m_state.delta_f < 0) {
					double del = std::max(-m_state.delta_f, 10 * std::numeric_limits<double>::epsilon() * std::abs(m_fval));
					alpha = std::min(1.0, 2.0 * del / (-m_state.fp0));
				}
				else {
					alpha = std::abs(m_state.step);
				}

				//Determine step length
				assert(m_state.wrapper);
				SingleMin<OneDimWrapper<FcnType>> lineSearcher(*m_state.wrapper);
				Status status = lineSearcher.minSearch(alpha);

				if (status != Status::SUCCESS)
					return status;

				//Update our current position (this is the only time we write to m_x)
				double temp;//unused
				m_state.wrapper->eval(alpha, m_fval, temp);//make sure the right point is cached
				m_x = m_state.x_alpha;//wrapper is updating x_alpha/g_alpha
				m_grad = m_state.g_alpha;
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_out = m_x;
				}

				//Choose new direction
				//Note: We do not actually accumulate any information on the Hessian. 
				//This might be good enough, but might also be worth storing some history.

				m_state.dx0 = m_x - m_state.x0;
				m_dx = m_state.dx0;//why do we need a copy?

				m_state.dg0 = m_grad - m_state.g0;

				double dxg = m_state.dx0.transpose() * m_grad;
				double dgg = m_state.dg0.transpose() * m_grad;
				double dxdg = m_state.dx0.transpose() * m_state.dg0;

				double A, B;
				if (dxdg != 0) {
					B = dxg / dxdg;
					A = -(1.0 + m_state.dg0.squaredNorm() / dxdg) * B + dgg / dxdg;
				}
				else {
					B = 0;
					A = 0;
				}

				m_state.p = m_grad - A * m_state.dx0 - B * m_state.dg0;

				//Prepare for next iteration
				//(Could this not be done at the start, and it can be local?)
				m_state.g0 = m_grad;
				m_state.x0 = m_x;
				m_state.g0norm = m_state.g0.norm();
				m_state.pnorm = m_state.p.norm();

				double pg = m_state.p.transpose() * m_state.g0;
				double dir = pg > 0.0 ? -1.0 : 1.0;
				m_state.p *= dir / m_state.pnorm;
				m_state.pnorm = m_state.p.norm();
				m_state.fp0 = m_state.p.transpose() * m_state.g0;//I think we can skip this. Leave it to the wrapper.
				m_state.wrapper->newDirection(m_state.fp0);//maybe just construct a new wrapper every iteration?

				return Status::SUCCESS;
			}

			double fval() const { return m_fval; }
			const Eigen::VectorXd& grad() const { return m_grad; }
			const Eigen::VectorXd& dx() const { return m_dx; }

		private:
			FcnType& m_fcn;
			Eigen::VectorXd& m_out;
			std::mutex& m_mutex;

			int m_N;

			Eigen::VectorXd m_x;
			double m_fval;
			Eigen::VectorXd m_grad;
			Eigen::VectorXd m_dx;

			InternalState m_state;
		};
	}
}