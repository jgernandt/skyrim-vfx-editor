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
#include "NodeBase.h"
#include "CompositionActions.h"

#include "Optimisation.h"

constexpr float SCALE = 100.0f;
constexpr float k = 10.0f;

node::Positioner::Positioner(std::vector<std::unique_ptr<NodeBase>>&& nodes, std::vector<Positioner::LinkInfo>&& links) :
	m_N{ static_cast<int>(nodes.size()) }, m_x{ Eigen::VectorXd::Zero(2 * m_N - 2) }
{
	//Add nodes as children
	m_nodes.reserve(m_N);
	for (auto&& node : nodes) {
		m_nodes.push_back(node.get());
		addChild(std::move(node));
	}

	m_links = std::move(links);

	m_thread = std::thread(&Positioner::solve, this);
}

node::Positioner::~Positioner()
{
	//Join thread, if still running
	if (m_thread.joinable()) {
		m_cancel.store(true);
		m_thread.join();
	}
}

void node::Positioner::frame(gui::FrameDrawer& fd)
{
	Composite::frame(fd);

	//Update positions (if our algorithm gives thread-safe access to the current result vector, else move into conditional)
	assert(m_nodes.size() == m_N);
	Eigen::VectorXf x(2 * m_N);
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		assert(m_x.size() == 2 * m_N - 2);
		x << 0.0f, SCALE * m_x.head(m_N - 1).cast<float>(), 0.0f, SCALE * m_x.tail(m_N - 1).cast<float>();
	}
	for (int i = 0; i < m_N; i++) {
		if (std::find(m_removed.begin(), m_removed.end(), m_nodes[i]) == m_removed.end()) {
			assert(m_nodes[i]);
			m_nodes[i]->setTranslation({ x(i), x(i + m_N) });
		}
	}

	if (m_done.load()) {
		//Join thread
		m_thread.join();

		//transfer our children to parent and retire
		if (IComponent* parent = getParent()) {
			for (auto&& child : getChildren())
				asyncInvoke<gui::MoveChild>(child.get(), this, parent, false);
			asyncInvoke<gui::RemoveChild>(this, parent, false);
		}
	}
}

//If the search takes a long time, someone might remove a node before we're done. Track that here.
gui::ComponentPtr node::Positioner::removeChild(gui::IComponent* c)
{
	m_removed.push_back(c);
	return Composite::removeChild(c);
}

struct OurFunction
{
	OurFunction(const std::vector<node::Positioner::LinkInfo>& links) : links{ links } {}

	void eval(const Eigen::VectorXd& x, double& r_f, Eigen::VectorXd& r_grad) 
	{
		using namespace Eigen;

		assert(x.size() % 2 == 0);
		int N = x.size() / 2 + 1;
		m_r2.resize(N, N);

		double repulsion = 20.0;
		double baseStiffness = 1.0;

		//Only calc the upper triangle of r2 (ignore main diagonal)
		//Store 1/r2 in the lower triangle
		m_r2(0, seq(1, N - 1)) = x.head(N - 1).array().square() + x.tail(N - 1).array().square();
		m_r2(seq(1, N - 1), 0) = 1.0 / m_r2(0, seq(1, N - 1));
		for (int i = 1; i < N - 1; i++) {
			m_r2(i, seq(i + 1, N - 1)) = (x(i - 1) - x.segment(i, N - i - 1).array()).square() + (x(i + N - 2) - x.tail(N - i - 1).array()).square();
			m_r2(seq(i + 1, N - 1), i) = 1.0f / m_r2(i, seq(i + 1, N - 1));
		}

		r_f = repulsion * (m_r2(seq(1, N - 1), 0).sum());
		r_grad = VectorXd::Zero(2 * N - 2);

		for (auto&& link : links) {
			int i1 = link.node1;
			int i2 = link.node2;
			assert(i1 >= 0 && i1 < N&& i2 >= 0 && i2 < N);
			if (i1 != i2) {
				double ox = link.offset[0] / SCALE;
				double oy = link.offset[1] / SCALE;

				//make sure we use upper triangle
				assert(i1 < i2);//must be guaranteed by the Constructor

				//i2 != 0, since i1 < i2 and i1 >= 0
				if (i1 != 0) {
					//Modify squared distance to account for offset
					m_r2(i1, i2) += ox * (ox - 2.0 * (x(i1 - 1) - x(i2 - 1)));
					m_r2(i1, i2) += oy * (oy - 2.0 * (x(i1 + N - 2) - x(i2 + N - 2)));

					//Add contribution to gradient
					double tmp = 2.0 * baseStiffness * link.stiffness * (x(i1 - 1) - x(i2 - 1) - ox);
					r_grad(i1 - 1) += tmp;
					r_grad(i2 - 1) -= tmp;
					tmp = 2.0 * baseStiffness * link.stiffness * (x(i1 + N - 2) - x(i2 + N - 2) - oy);
					r_grad(i1 + N - 2) += tmp;
					r_grad(i2 + N - 2) -= tmp;
				}
				else {
					//Modify squared distance to account for offset
					m_r2(i1, i2) += ox * (ox + 2.0 * x(i2 - 1));
					m_r2(i1, i2) += oy * (oy + 2.0 * x(i2 + N - 2));

					//Add contribution to gradient
					r_grad(i2 - 1) += 2.0 * baseStiffness * link.stiffness * (x(i2 - 1) + ox);
					r_grad(i2 + N - 2) += 2.0 * baseStiffness * link.stiffness * (x(i2 + N - 2) + oy);
				}

				//Add contribution to function value
				r_f += baseStiffness * link.stiffness * m_r2(i1, i2);
			}
			//else ignore
		}

		for (int i = 1; i < N; i++) {
			r_f += repulsion * (m_r2(seq(i + 1, N - 1), i).sum());

			//store 1/r4 temporarily
			m_tmp.resize(N);
			m_tmp << m_r2(i, seq(0, i - 1)).square().transpose(), 0.0, m_r2(seq(i + 1, N - 1), i).square();

			r_grad(i - 1) -= 2.0 * repulsion * (m_tmp(0) * x(i - 1) + m_tmp.tail(N - 1).transpose() * (x(i - 1) - x.head(N - 1).array()).matrix());
			//r_grad(i - 1) += m_tmp.tail(N - 1).transpose() * (x(i - 1) - x.head(N - 1).array()).matrix();
			//r_grad(i - 1) *= -2.0 * repulsion;

			r_grad(i + N - 2) -= 2.0 * repulsion * (m_tmp(0) * x(i + N - 2) + m_tmp.tail(N - 1).transpose() * (x(i + N - 2) - x.tail(N - 1).array()).matrix());
			//r_grad(i + N - 2) += m_tmp.tail(N - 1).transpose() * (x(i + N - 2) - x.tail(N - 1).array()).matrix();
			//r_grad(i + N - 2) *= -2.0 * repulsion;
		}
	}
	void fval(const Eigen::VectorXd& x, double& r_f) {}
	void grad(const Eigen::VectorXd& x, Eigen::VectorXd& r_grad) {}

	const std::vector<node::Positioner::LinkInfo>& links;

	private:
		Eigen::ArrayXXd m_r2;
		Eigen::VectorXd m_tmp;
};

void node::Positioner::solve()
{
	//This is an entry-point function
	try {
		//initial guess
		{
			std::mt19937 mt;
			std::uniform_real_distribution<float> D(0.0f, 10.0f);

			std::lock_guard<std::mutex> lock(m_mutex);
			m_x.resize(2 * m_N - 2);
			for (int i = 0; i < m_N - 1; i++) {
				m_x(i) = D(mt);
				m_x(i + m_N - 1) = D(mt);
			}
		}

		OurFunction fcn(m_links);
		math::opt::SyncMultiMin minimiser(fcn, m_x, m_mutex);

		int count = 0;
		math::opt::Status status = math::opt::Status::SUCCESS;
		do {
			//iterate
			count++;
			status = minimiser.iterate();

			if (status == math::opt::Status::SUCCESS) {
				//Suitable test of convergence? This seems completely arbitrary.
				if (double gnorm2 = minimiser.grad().squaredNorm(); gnorm2 > 1.0e-6)
					status = math::opt::Status::CONTINUE;
			}
			else {
				//deal with problem (= ignore it)
			}
		} while (status == math::opt::Status::CONTINUE && count < 200 && !m_cancel.load());
	}
	catch (...) {}

	m_done.store(true);
}
