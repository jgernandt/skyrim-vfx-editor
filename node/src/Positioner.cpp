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

node::Positioner::Positioner(std::vector<std::unique_ptr<NodeBase>>&& nodes, const Eigen::MatrixXf& connectivity) :
	m_C{ connectivity }, m_N{ static_cast<int>(nodes.size()) }, m_x{ Eigen::VectorXd::Zero(2 * m_N - 2) }
{
	//Add nodes as children
	m_nodes.reserve(m_N);
	for (auto&& node : nodes) {
		m_nodes.push_back(node.get());
		addChild(std::move(node));
	}
	//Start solving in m_thread
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
	void eval(const Eigen::VectorXd& x, double& r_f, Eigen::VectorXd& r_grad) 
	{
		using namespace Eigen;

		assert(x.size() % 2 == 0);
		int N = x.size() / 2 + 1;
		assert(C.rows() == N && C.cols() == N);

		VectorXd X(2 * N);
		X << 0.0, x.head(N - 1), 0.0, x.tail(N - 1);

		ArrayXXd r2(N, N);
		ArrayXXd rm2(N, N);

		//We do double the required work, since this matrix is symmetric. Optimise later.
		for (int i = 0; i < N; i++) {
			r2.row(i) = (X(i) - X.head(N).array()).square() + (X(i + N) - X.tail(N).array()).square();
			rm2.row(i) = 1.0f / r2.row(i);
		}
		//This is just destroying terms that should be excluded (but we include anyway for vectorisation purposes)
		rm2.matrix().diagonal() = ArrayXd::Zero(N);

		double repulsion = 10.0;

		//Function value
		r_f = 0.0f;
		for (int i = 0; i < N; i++) {
			r_f += C(i, seq(i, N - 1)) * r2(seq(i, N - 1), i).matrix() + repulsion * rm2(i, seq(i, N - 1)).sum();
		}

		//gradient
		r_grad.resize(2 * N - 2);
		MatrixXd K = C - repulsion * rm2.square().matrix();
		for (int i = 1; i < N; i++) {
			r_grad(i - 1) = 2.0f * (X(i) - X.head(N).array()).matrix().transpose() * K.col(i);
			r_grad(i + N - 2) = 2.0f * (X(i + N) - X.tail(N).array()).matrix().transpose() * K.col(i);
		}

	}
	void fval(const Eigen::VectorXd& x, double& r_f) {}
	void grad(const Eigen::VectorXd& x, Eigen::VectorXd& r_grad) {}

	Eigen::MatrixXd C;
};

void node::Positioner::solve()
{
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

	OurFunction fcn;
	fcn.C = m_C.cast<double>();
	math::opt::SyncMultiMin minimiser(fcn, m_x, m_mutex);

	int count = 0;
	math::opt::Status status = math::opt::Status::SUCCESS;
	do {
		//iterate
		count++;
		status = minimiser.iterate();

		if (status == math::opt::Status::SUCCESS) {
			//Suitable test of convergence? This seems completely arbitrary.
			if (double gnorm2 = minimiser.grad().squaredNorm(); gnorm2 > 1.0e-4)
				status = math::opt::Status::CONTINUE;
		}
		else {
			//deal with problem (= ignore it)
		}
	} 
	while (status == math::opt::Status::CONTINUE && count < 200 && !m_cancel.load());

	double gnorm = minimiser.grad().norm();
	m_done.store(true);
}
