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
#include "gui_types.h"

#include "Widget.h"
#include "UniqueLabel.h"


namespace gui
{
    namespace backend
    {
        bool Checkbox(const std::string& label, bool* v);

        //until we can be bothered to introduce layout operators for checkbox:
        Floats<2> getCheckboxSize(const std::string& label);
    }

    //ImGui-style checkbox
    class SimpleCheckbox : public Component
    {
    public:
        SimpleCheckbox(bool* ptr, const std::string& label) : m_ptr{ ptr }, m_labels{ label } {}
        virtual void frame(FrameDrawer& fd) override
        {
            backend::Checkbox(m_labels[0], m_ptr);
        }

        virtual Floats<2> getSizeHint() const override { return backend::getCheckboxSize(m_labels[0]); }

    private:
        bool* m_ptr;
        UniqueLabel<1> m_labels;
    };

    //More flexible checkbox
    template<typename T, size_t N, typename PropertyType, template<typename> typename ConverterType = GuiConverter>
    class Checkbox final : public Component
    {
    private:
        static_assert(!std::is_reference<T>::value, "value type is a reference type");

        static_assert(util::array_traits<T>::size >= N, "array is too small");

        using element_type = typename util::array_traits<T>::element_type;
        static_assert(std::is_integral<element_type>::value, "element is not integral");

        using get_type = typename util::property_traits<PropertyType>::get_type;
        using converted_type = decltype(util::type_conversion<T, ConverterType<T>>::from(std::declval<get_type>()));
        static_assert(std::is_assignable<T&, converted_type>::value, "property return cannot be assigned to value type");

    public:
        Checkbox(const PropertyType& p, const std::string& label) : m_property{ p }, m_labels(label) {}
        Checkbox(const PropertyType& p, const std::array<std::string, N>& labels) : m_property{ p }, m_labels(labels) {}

        virtual void frame(FrameDrawer& fd) override
        {
            T data = util::type_conversion<T, ConverterType<T>>::from(
                util::property_traits<PropertyType>::get(m_property));

            for (size_t i = 0; i < N; i++) {
                if (backend::Checkbox(m_labels[i], &util::array_traits<T>::at(data, i)))
                    asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, data, true);
            }
        }

        //Will not be valid for arrays until we introduce layouts
        virtual Floats<2> getSizeHint() const override { return backend::getCheckboxSize(m_labels[0]); }

    protected:
        PropertyType m_property;
        UniqueLabel<N> m_labels;
    };

    

    /*template<size_t N>
    class CheckboxH final :
        public Component, Controller<bool>
    {
    public:
        CheckboxH(std::string* labels, BoolProperty& p) :
            Controller<bool>(p)
        {
            for (int i = 0; i < N; i++) {
                if (labels)
                    m_labels[i] = labels[i] + "##" + std::to_string(reinterpret_cast<uintptr_t>(this)) + std::to_string(i);
            }
        }

        virtual void frame() override
        {
            using namespace ImGui;

            bool data[N];
            this->m_source.produce(data, N);

            ImVec2 pos = GetCursorPos();

            for (int i = 0; i < N; i++) {

                SetCursorPos(pos);

                if (ImGui::Checkbox(m_labels[i].c_str(), &data[i])) {
                    this->m_sink.consume(data, N);
                }

                ImVec2 label_size = CalcTextSize(m_labels[i].c_str(), nullptr, true);
                pos.x += GetFrameHeight() +
                    (label_size.x > 0.0f ? GetStyle().ItemInnerSpacing.x + label_size.x : 0.0f) +
                    GetStyle().ItemSpacing.x;
            }
        }
        virtual float getWidth() const override
        {
            using namespace ImGui;

            float square_sz = GetFrameHeight();
            float width = N * square_sz + (N - 1) * GetStyle().ItemSpacing.x;

            for (int i = 0; i < N; i++) {
                ImVec2 label_size = CalcTextSize(m_labels[i].c_str(), nullptr, true);
                width += label_size.x > 0.0f ? GetStyle().ItemInnerSpacing.x + label_size.x : 0.0f;
            }

            return width;
        }
        virtual ImVec2 getSize() const override
        {
            return { CheckboxH::getWidth(), ImGui::GetFrameHeight() };
        }

    private:
        std::string m_labels[N];
    };*/

}
