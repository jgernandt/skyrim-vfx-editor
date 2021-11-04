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
#include <string>
#include "Composition.h"
#include "UniqueLabel.h"

namespace gui
{
    class PopupBase :
        public Composite
    {
    public:
        PopupBase(const std::string& title) : m_id(title) {}
        virtual ~PopupBase() = default;
        virtual void frame(FrameDrawer& fd) override;

        virtual void onOpen() {}
        virtual void onClose() {}

        void open();
        bool isOpen() const { return m_isOpen; }

    protected:
        UniqueLabel<1> m_id;
        bool m_isOpen{ false };

    private:
        Floats<2> m_lastGlobalPos{ 0.0f, 0.0f };
        bool m_firstFrame{ false };
        bool m_shouldOpen{ false };
    };

    class Popup :
        public PopupBase
    {
    public:
        Popup() : PopupBase(std::string()) {}
        virtual ~Popup() {}
        virtual void frame(FrameDrawer& fd) override;

    };

    class Modal :
        public PopupBase
    {
    public:
        Modal(const std::string& title) : PopupBase(title) {}
        virtual ~Modal() {}
        virtual void frame(FrameDrawer& fd) override;

        virtual void close();
    };

    //Temporary modal popup showing a message
    class MessageBox :
        public Modal
    {
    public:
        MessageBox(const std::string& title, const std::string& msg);
        virtual ~MessageBox() {}
        virtual void frame(FrameDrawer& fd) override;

        virtual void close() override;

    };
}
