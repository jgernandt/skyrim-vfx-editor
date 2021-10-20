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
#include <filesystem>
#include "IInvoker.h"
#include "Composition.h"
#include "File.h"

namespace node
{
	class Editor;
}

namespace app
{
	class Document final :
		public gui::Composite
	{
	public:
		Document();
		Document(const std::filesystem::path& path);
		virtual void frame(gui::FrameDrawer& fd) override;
		virtual void setSize(const gui::Floats<2>& size) override;
		virtual gui::IInvoker* getInvoker() override { return &m_invoker; }

		const std::filesystem::path& getFilePath() const { return m_targetPath; }
		void setFilePath(const std::filesystem::path& path);

		void undo() { m_invoker.undo(); }
		void redo() { m_invoker.redo(); }

		void write();

	private:
		class Invoker final :
			public gui::IInvoker
		{
			using CommandPtr = std::unique_ptr<gui::ICommand>;
		public:
			virtual void invoke() override;
			virtual void queue(CommandPtr&& a) override;
			virtual void undo() override;
			virtual void redo() override;

			bool empty() const { return m_pending.empty(); }
			void flush();

		private:
			std::deque<CommandPtr> m_pending;
			std::list<CommandPtr> m_history;
			std::list<CommandPtr>::iterator m_next{ m_history.begin() };
		};

		nif::File m_file;
		Invoker m_invoker;
		std::filesystem::path m_targetPath;
		node::Editor* m_nodeEditor{ nullptr };

	};
}

