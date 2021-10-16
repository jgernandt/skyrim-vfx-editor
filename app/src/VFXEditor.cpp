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
#include "VFXEditor.h"
#include "Document.h"
#include "GUIEngine.h"
#include "Timer.h"
#include "CallWrapper.h"
#include "version.h"

#undef min
#undef max
#include "widgets.h"

app::VFXEditor::VFXEditor(HINSTANCE hInstance, int nCmdShow) : 
    D3D10Window(hInstance, nCmdShow), m_fileMenu("File"), m_helpMenu("Help")
{
    assert(m_hwnd && m_d3dDevice);//guaranteed by true return from D3D10Window::initD3D
    SetWindowText(m_hwnd, APP_WNAME);

    m_guiEngine.initWin32Window(m_hwnd);
    m_guiEngine.initDX10Window(m_d3dDevice);

    //Load fonts from Windows installation
    PWSTR wpath;
    HRESULT res = SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &wpath);
    if (SUCCEEDED(res)) {
        //This is not failsafe. The fonts may not exist or may be named differently in some locales (?).
        //However, finding a failsafe method seems unreasonably complicated at this point.
        //Try a couple of fonts and fall back to the built-in if they can't be loaded.
        std::filesystem::path path(wpath, std::filesystem::path::native_format);
        if (!m_guiEngine.loadFont(path / "calibri.ttf"))
            if (!m_guiEngine.loadFont(path / "arial.ttf"))
                if (!m_guiEngine.loadFont())
                    throw std::runtime_error("Failed to load any fonts.");
    }
    CoTaskMemFree(wpath);

    m_guiEngine.setStyleColours();

    m_fileMenu.addChild(std::make_unique<gui::MenuItem>("New", std::bind(&VFXEditor::newDoc, this)));
    m_fileMenu.addChild(std::make_unique<gui::MenuItem>("Open...", std::bind(&VFXEditor::open, this)));
    //m_fileMenu.addChild(std::make_unique<gui::MenuItem>("Append...", std::bind(&VFXEditor::append, this)));//TODO
    m_fileMenu.addChild(std::make_unique<gui::Separator>());
    m_fileMenu.addChild(std::make_unique<gui::MenuItem>("Save", std::bind(&VFXEditor::save, this)));
    m_fileMenu.addChild(std::make_unique<gui::MenuItem>("Save as...", std::bind(&VFXEditor::saveAs, this)));
    m_fileMenu.addChild(std::make_unique<gui::Separator>());
    m_fileMenu.addChild(std::make_unique<gui::MenuItem>("Exit", std::bind(&VFXEditor::quit, this)));

    m_helpMenu.addChild(std::make_unique<gui::Separator>());
    m_helpMenu.addChild(std::make_unique<gui::MenuItem>(std::string("About ") + APP_NAME, std::bind(&VFXEditor::about, this)));
}

app::VFXEditor::~VFXEditor()
{
}

int app::VFXEditor::run()
{
    newDoc();

    // Main loop
    MSG msg;

    //This is pretty bad message handling, but imgui more or less needs it this way
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) && msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message != WM_QUIT)
            frame();
    }

    /*This would be nicer
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_PAINT) {
            frame();
        }
    }*/

    return static_cast<int>(msg.wParam);
}

void app::VFXEditor::frame()
{
    m_guiEngine.beginFrame();//frame N

    try {
        m_fileMenu.frame();
        if (m_current)
            m_current->frame();
        m_helpMenu.frame();
        if (m_aboutBox) {
            if (!m_aboutBox->isOpen())
                m_aboutBox->open();
            m_aboutBox->frame();
            if (!m_aboutBox->isOpen())
                m_aboutBox.reset();
        }
    }
    catch (const std::exception& e) {
        switch (frameErrorMessage(e.what()))
        {
        case IDCANCEL:
            quit();
            return;
        case IDTRYAGAIN:
            break;
        case IDCONTINUE:
            break;
        }
    }

    present();//frame N - 1
    clear({ 0.0f, 0.5f, 0.0f, 1.0f });
    m_guiEngine.endFrame();//frame N
    //(CPU will be working on frame N + 1 while GPU is working on frame N)
}

void app::VFXEditor::quit()
{
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
}

void app::VFXEditor::newDoc()
{
    m_current.reset();
    m_current = std::make_unique<Document>();
    RECT rect;
    if (GetClientRect(m_hwnd, &rect))
        m_current->setSize({ static_cast<float>(rect.right), static_cast<float>(rect.bottom) });
}
void app::VFXEditor::open()
{
    //ask to save current?

    //show open dialog window

    CComPtr<IFileOpenDialog> obj;
    if (FAILED(obj.CoCreateInstance(__uuidof(FileOpenDialog))))
        throw std::runtime_error("Failed to create Open dialog");

    COMDLG_FILTERSPEC filter{ L"NetImmerse file", L"*.nif" };
    if (FAILED(obj->SetFileTypes(1, &filter)))
        throw std::runtime_error("Failed to set filter");

    if (SUCCEEDED(obj->Show(NULL))) {
        CComPtr<IShellItem> item;
        if (FAILED(obj->GetResult(&item)))
            throw std::runtime_error("Failed to get result from Open dialog.");

        PWSTR wpath;
        HRESULT res = item->GetDisplayName(SIGDN_FILESYSPATH, &wpath);
        if (SUCCEEDED(res)) {
            m_current.reset();
            m_current = std::make_unique<Document>(std::filesystem::path(wpath, std::filesystem::path::native_format));
            RECT rect;
            if (GetClientRect(m_hwnd, &rect))
                m_current->setSize({ static_cast<float>(rect.right), static_cast<float>(rect.bottom) });
            CoTaskMemFree(wpath);
        }
    }
    else {
        //cancelled by user
    }

}

void app::VFXEditor::save()
{
    if (m_current) {
        if (m_current->getFilePath().empty())
            setFilePath(*m_current);
        m_current->write();
    }
}

void app::VFXEditor::saveAs()
{
    if (m_current) {
        setFilePath(*m_current);
        m_current->write();
    }
}

void app::VFXEditor::about()
{
    if (!m_aboutBox)
        m_aboutBox = std::make_unique<AboutBox>();
}

void app::VFXEditor::undo()
{
    if (m_current)
        m_current->undo();
}

void app::VFXEditor::redo()
{
    if (m_current)
        m_current->redo();
}

void app::VFXEditor::setFilePath(Document& doc)
{
    CComPtr<IFileSaveDialog> obj;
    if (FAILED(obj.CoCreateInstance(__uuidof(FileSaveDialog))))
        throw std::runtime_error("Failed to create Save dialog");

    COMDLG_FILTERSPEC filter{ L"NetImmerse file", L"*.nif" };
    if (FAILED(obj->SetFileTypes(1, &filter)))
        throw std::runtime_error("Failed to set filter.");

    if (FAILED(obj->SetDefaultExtension(L"nif")))
        throw std::runtime_error("Failed to set default extension.");

    if (SUCCEEDED(obj->Show(NULL))) {
        CComPtr<IShellItem> item;
        if (FAILED(obj->GetResult(&item)))
            throw std::runtime_error("Failed to get result from Save dialog.");

        PWSTR wpath;
        HRESULT res = item->GetDisplayName(SIGDN_FILESYSPATH, &wpath);
        if (SUCCEEDED(res)) {
            doc.setFilePath(std::filesystem::path(wpath, std::filesystem::path::native_format));
            CoTaskMemFree(wpath);
        }
    }
    else {
        //cancelled by user
    }
}

int app::VFXEditor::frameErrorMessage(const char* what)
{
    std::wstring s;
    int len = MultiByteToWideChar(CP_UTF8, 0, what, -1, nullptr, 0);
    if (len > 0) {
        s.resize(len);
        MultiByteToWideChar(CP_UTF8, 0, what, -1, s.data(), len);
    }

    std::wstring msg = L"Unhandled exception:\n";
    msg += s;

    int msgboxID = MessageBox(
        m_hwnd,
        msg.c_str(),
        nullptr,
        MB_ICONERROR | MB_CANCELTRYCONTINUE | MB_DEFBUTTON1
    );

    return msgboxID;
}

void app::VFXEditor::fatalError(const wchar_t* what)
{
    int msgboxID = MessageBox(
        m_hwnd,
        what,
        nullptr,
        MB_ICONERROR | MB_OK | MB_DEFBUTTON1
    );
}

LRESULT app::VFXEditor::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_guiEngine.handle(m_hwnd, uMsg, wParam, lParam))
        return true;
    
    switch (uMsg)
    {
    case WM_SIZE:
        if (m_current)
            m_current->setSize({ static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam)) });
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (!m_guiEngine.isCapturingKeyboard() && !(HIWORD(lParam) & KF_REPEAT)) {
            switch (wParam) {
            case 'Z':
                if (GetKeyState(VK_CONTROL) & 0x8000) {
                    if (GetKeyState(VK_SHIFT) & 0x8000)
                        redo();
                    else
                        undo();
                }
                break;
            case 'Y':
                if (GetKeyState(VK_CONTROL) & 0x8000)
                    redo();
                break;
            }
        }
        break;
    }
    return D3D10Window::wndProc(uMsg, wParam, lParam);
}
