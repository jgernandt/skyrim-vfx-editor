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
#include "ConnectionHandler.h"
#include "NiNode.h"
#include "File.h"
#include "widgets.h"

namespace node
{
	class Root;

	class Editor final :
		public gui::Composite
	{
	public:
		Editor();
		Editor(const nif::File& file);
		~Editor();

		virtual void frame(gui::FrameDrawer& fd) override;

		void setProjectName(const std::string& name);

	private:
		//static std::unique_ptr<IComponent> createAddMenu(IComponent& parent);
		Root* findRootNode() const;


	private:
		class NodeRoot final : public gui::ConnectionHandler
		{
		public:
			NodeRoot();
			virtual void frame(gui::FrameDrawer& fd) override;

			std::unique_ptr<IComponent> createAddMenu();
			template<typename T> void addNode();
		};

		const nif::File::Version m_niVersion;
	};
}

