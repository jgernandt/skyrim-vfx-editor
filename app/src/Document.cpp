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

#include "pch.h"
#include "Document.h"
#include "Editor.h"
#include "File.h"
#include "widgets.h"

constexpr size_t UNDO_LIMIT = 100;

void app::Document::Invoker::invoke()
{
	while (!m_pending.empty()) {
		auto&& a = m_pending.front();
		assert(a);
		a->execute();
		if (a->reversible()) {
			//erase undone actions
			m_next = m_history.erase(m_next, m_history.end());//no-op if next == end
			//push new action
			m_next = ++m_history.insert(m_next, std::move(a));
			//impose an arbitrary limit on history size?
			if (m_history.size() > UNDO_LIMIT)
				//there is no way m_next can be at the front now
				m_history.pop_front();
		}
		m_pending.pop_front();
	}
}

void app::Document::Invoker::queue(CommandPtr&& a)
{
	m_pending.push_back(std::move(a));
}

void app::Document::Invoker::undo()
{
	if (m_next != m_history.begin()) {
		--m_next;
		(*m_next)->reverse();
	}
}

void app::Document::Invoker::redo()
{
	if (m_next != m_history.end()) {
		(*m_next)->execute();
		++m_next;
	}
}

void app::Document::Invoker::flush()
{
	m_pending.clear();
}

app::Document::Document()
{
	m_file = nif::File(nif::File::Version::SKYRIM_SE);
	m_file.addFadeNode();
	m_nodeEditor = newChild<node::Editor>(m_file);

	//If, for any reason, we add a message box here and the file is empty, the popups break.
	//Doing the exact same thing on a doc created later works fine.
	//I have no idea why. Maybe ImGui popups need a window to exist or have existed at some point in the past?
	//I don't really care anymore. Just make sure this works 100% and forget about it.
}

app::Document::Document(const std::filesystem::path& path) : m_targetPath{ path }
{
	//Load the file (via a backend io object)
	//Verify version
	//Verify validity?
	//If somehow invalid: construct a modal describing the error and an empty node editor.
	//We want the program to have the same appearance regardless.
	//When the modal is closed, normal control should resume (but nothing to edit, only file menu showing).

	//If all looks ok, add a node editor for the file. It will attempt to create nodes.
	//It may fail, or warn about problems. If the errors were serious (indicated by an exception coming back to us), the file may have 
	//to be unloaded. Otherwise, it can be edited, with a warning to the user that the result may be broken.
	//General appearance should be the same regardless.

	//Summary: we should catch all exceptions. Anything that goes through is, as far as we are concerned, a fatal error.
	//Any exception that we DO catch (from the file io or the node editor constructor) 
	//will result in an error message (modal) and an empty file.

	try {
		//If our backend nif io is robust we don't really need to check the file status here.
		//But, as a general idea:
		auto fileStatus = std::filesystem::status(path);
		if (fileStatus.type() == std::filesystem::file_type::not_found)
			throw std::runtime_error("File not found");
		//(let's leave it at that for now)

		m_file = nif::File(path);//may throw
		m_nodeEditor = newChild<node::Editor>(m_file);//should catch warnings, may throw serious errors
	}
	catch (const std::exception& e) {
		m_file = nif::File();
		m_nodeEditor = newChild<node::Editor>();
		newChild<gui::MessageBox>("Error", e.what());
	}
}

void app::Document::frame()
{
	Composite::frame();
	m_invoker.invoke();
}

void app::Document::setSize(const gui::Floats<2>& size)
{
	//If we had multiple views, we would keep track of what view would occupy what area.
	//Fow now, just pass it on.
	if (m_nodeEditor)
		m_nodeEditor->setSize(size);
}

void app::Document::setFilePath(const std::filesystem::path& path)
{
	m_targetPath = path;
	if (m_nodeEditor)
		m_nodeEditor->setProjectName(path.filename().u8string());
}

void app::Document::write()
{
	try {
		m_file.write(m_targetPath);
	}
	catch (const std::exception& e) {
		addChild(std::make_unique<gui::MessageBox>("Error", e.what()));
	}
}
