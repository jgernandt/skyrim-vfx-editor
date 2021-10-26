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
#include <map>
#include <memory>
#include <string>
#include "BaseWindow.h"
#include "GUIEngine.h"
#include "Menu.h"

namespace app
{
	class AboutBox;
	class Document;

	class VFXEditor final :
		public D3D10Window
	{
	public:
		VFXEditor(HINSTANCE hInstance, int nCmdShow);
		~VFXEditor();

		int run();

	protected:
		virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void frame();
		void quit();

		void append() {}
		void newDoc();
		void open();
		void save();
		void saveAs();

		void about();

		void undo();
		void redo();

	private:
		void setFilePath(Document& doc);

		int frameErrorMessage(const char* what);
		void fatalError(const wchar_t* what);

		//class AboutBox;

	private:
		gui::MainMenu m_fileMenu;
		gui::MainMenu m_helpMenu;
		std::unique_ptr<Document> m_current;
		std::unique_ptr<AboutBox> m_aboutBox;

		gui::backend::ImGuiWinD3D10 m_guiEngine;
	};
}
