//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "Composition.h"
#include "Activatable.h"
#include "UniqueLabel.h"

namespace gui
{
    class Button :
        public Activatable, public Component
    {
    public:
        Button(const std::string& label, const action_type& action = action_type()) :
            Activatable(action), m_label(label) {}
        virtual ~Button() = default;

        virtual void frame() override;
        virtual Floats<2> getSizeHint() const override;

    private:
        UniqueLabel<1> m_label;
    };
}