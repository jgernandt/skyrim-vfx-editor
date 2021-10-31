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
#include <atomic>
#include <mutex>
#include <thread>
#include "Eigen\Core"
#include "Composition.h"

namespace node
{
	class NodeBase;

	//Responsible for positioning nodes after loading a file
	class Positioner final : public gui::Composite
	{
	public:
		Positioner(std::vector<std::unique_ptr<NodeBase>>&& nodes, const Eigen::MatrixXf& connectivity);
		~Positioner();

		virtual void frame(gui::FrameDrawer& fd) override;
		virtual gui::ComponentPtr removeChild(gui::IComponent*) override;

	private:
		void solve();

	private:
		Eigen::MatrixXf m_C;
		int m_N;
		std::mutex m_mutex;//protects access to result vector
		Eigen::VectorXd m_x;

		std::thread m_thread;
		std::atomic_bool m_cancel{ false };
		std::atomic_bool m_done{ false };

		std::vector<NodeBase*> m_nodes;
		std::vector<gui::IComponent*> m_removed;

	};
}