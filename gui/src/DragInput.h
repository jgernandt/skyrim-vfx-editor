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
#include <cassert>
#include <limits>

#include "gui_types.h"

#include "Widget.h"
#include "UniqueLabel.h"

//An unreasonable amount of work to keep backend headers out of my dependent projects?
namespace gui
{
    template<typename T>
    struct DragInputDataType
    {
        using type = T;
        constexpr static int id = -1;
    };
    template<> struct DragInputDataType<int>
    {
        using type = int;
        static const int id;
    };
    template<> struct DragInputDataType<float>
    {
        using type = float;
        static const int id;
    };
    template<> struct DragInputDataType<unsigned short>
    {
        using type = unsigned short;
        static const int id;
    };

    template<typename T>
    struct DragInputParameters
    {
        float speed{ std::is_floating_point<T>::value ? 0.1f : 1.0f };
        T min{ std::numeric_limits<T>::lowest() };
        T max{ std::numeric_limits<T>::max() };
        unsigned int flags{ 0U };
        const char* format{ std::is_floating_point<T>::value ? "%.1f" : "%d" };
    };

    namespace backend
    {
        unsigned int DragInputImpl(
            const std::string& label,
            int data_type,
            void* p_data,
            float v_speed,
            const void* p_min,
            const void* p_max,
            const char* format,
            int flags);

        //template<typename T>
        //unsigned int DragInput(const std::string& label, T* value, const gui::DragInputParameters<T>& params) 
        //{ 
        //    return DragInputImpl(label, DragInputDataType<T>::id, value, params.speed, &params.min, &params.max, params.format, params.flags);
        //}

        extern const unsigned int DragInput_AlwaysClamp;
        extern const unsigned int DragInput_Logarithmic;
    }

    template<typename NumberType, size_t N>
    class DragInputBase : public Component
    {
        static_assert(std::is_arithmetic<NumberType>::value, "DragInput requires an (array of an) arithmetic type");
    public:
        DragInputBase(const std::string& label) : m_labels(label)
        { 
            m_sizeHint[0] = -1.0f; 
            for (size_t i = 0; i < N; i++) m_permutation[i] = i;
        }
        DragInputBase(const std::array<std::string, N>& labels) : m_labels(labels)
        {
            m_sizeHint[0] = -1.0f;
            for (size_t i = 0; i < N; i++) m_permutation[i] = i;
        }
        virtual ~DragInputBase() = default;

        void setPermutation(const std::array<size_t, N>& p) { m_permutation = p; }

        void setSensitivity(float f, int i = -1) 
        { 
            assert(i < static_cast<int>(N)); 
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    m_params[j].speed = f;
            }
            else
                m_params[i].speed = f;
        }
        void setLowerLimit(NumberType min, int i = -1) 
        { 
            assert(i < static_cast<int>(N));
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    m_params[j].min = min;
            }
            else
                m_params[i].min = min; 
        }
        void setUpperLimit(NumberType max, int i = -1) 
        { 
            assert(i < static_cast<int>(N));
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    m_params[j].max = max;
            }
            else
                m_params[i].max = max; 
        }
        void setNumberFormat(const char* format, int i = -1) 
        { 
            assert(i < static_cast<int>(N));
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    m_params[j].format = format;
            }
            else
                m_params[i].format = format; 
        }
        void setAlwaysClamp(bool set = true, int i = -1) 
        { 
            assert(i < static_cast<int>(N));
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    setFlag(backend::DragInput_AlwaysClamp, set, j);
            }
            else
                setFlag(backend::DragInput_AlwaysClamp, set, i); 
        }
        void setLogarithmic(bool set = true, int i = -1)
        {
            assert(i < static_cast<int>(N));
            if (i < 0) {
                for (size_t j = 0; j < N; j++)
                    setFlag(backend::DragInput_Logarithmic, set, j);
            }
            else
                setFlag(backend::DragInput_Logarithmic, set, i);
        }
        //Makes more sense to set the layout here than in the template?
        //void setLayout(ILayout*) {}

    private:
        void setFlag(unsigned int flag, bool set, size_t i)
        {
            if (set)
                m_params[i].flags |= flag;
            else
                m_params[i].flags &= ~flag;
        }

    protected:
        UniqueLabel<N> m_labels;
        DragInputParameters<NumberType> m_params[N];
        std::array<size_t, N> m_permutation;
    };

    
    class ILayout
    {
    public:
        virtual ~ILayout() {}
        virtual void begin(size_t) = 0;
        virtual void next(size_t) = 0;

        virtual float height(size_t N) const = 0;
    };

    class DefaultLayout final : public ILayout
    {
    public:
        virtual void begin(size_t) override {}
        virtual void next(size_t) override {}

        virtual float height(size_t N) const override;
    };

    class HorizontalLayout final : public ILayout
    {
    public:
        virtual void begin(size_t N) override;
        virtual void next(size_t i) override;

        virtual float height(size_t N) const override;

        Floats<2> m_pos;
        float m_width;
    };

    template<
        typename T, 
        size_t N, 
        typename PropertyType, 
        template<typename> typename ConverterType = GuiConverter, 
        typename WidgetLayout = DefaultLayout>
    class DragInput : public DragInputBase<typename util::array_traits<T>::element_type, N>
    {
    private:
        //The property may return by reference, but T should not be a reference type (for simplicity - we could probably make that work too)
        static_assert(!std::is_reference<T>::value, "value type is a reference type");

        //Our data must have at least N elements
        static_assert(util::array_traits<T>::size >= N, "array is too small");

        using element_type = typename util::array_traits<T>::element_type;
        //The element type must be arithmetic (also asserted by DragInputBase, but we don't need that to be a separate class anymore)
        static_assert(std::is_arithmetic<element_type>::value, "element is not arithmetic");

        //The type returned by our property's get function
        using get_type = typename util::property_traits<PropertyType>::get_type;
        //A type accepted by our property's set function
        using set_type = typename util::property_traits<PropertyType>::value_type;

        //The type returned by a conversion from get_type to value_type (could be a reference)
        using converted_type = decltype(util::type_conversion<T, ConverterType<T>>::from(std::declval<get_type>()));
        //We need to make a local copy of the data
        static_assert(std::is_assignable<T&, converted_type>::value, "property return cannot be assigned to value type");

        static_assert(std::is_default_constructible<WidgetLayout>::value, "cannot construct layout object");

    public:
        DragInput(PropertyType& p, const std::string& label) :
            DragInputBase<element_type, N>(label), m_property{ p } {}
        DragInput(PropertyType& p, const std::array<std::string, N>& labels) :
            DragInputBase<element_type, N>(labels), m_property{ p } {}
        virtual ~DragInput() = default;

        virtual void frame() override
        {
            //Read from the property, convert if needed
            T data = util::type_conversion<T, ConverterType<T>>::from(util::property_traits<PropertyType>::get(m_property));
            
            WidgetLayout layout;
            layout.begin(N);

            for (size_t i = 0; i < N; i++) {
                size_t p_i = this->m_permutation[i];
                assert(p_i < N);

                layout.next(i);

                //unsigned int result = backend::DragInput<element_type>(this->m_labels[i], &util::array_traits<T>::at(data, i), this->m_params[i]);
                unsigned int result = backend::DragInputImpl(
                    this->m_labels[p_i],
                    DragInputDataType<element_type>::id, 
                    &util::array_traits<T>::at(data, p_i),
                    this->m_params[p_i].speed,
                    &this->m_params[p_i].min,
                    &this->m_params[p_i].max,
                    this->m_params[p_i].format,
                    this->m_params[p_i].flags);

                //Should these be virtual functions, and we decide the invocation procedure on a higher level?
                if (result & WIDGET_ACTIVATED) {
                    //edit started, store original value
                    m_tmp = data;
                }
                if (result & WIDGET_EDITED) {
                    //Edit in progress, send irreversible Action.
                    //Should not send Action during text input (it currently does!). <- fix this!
                    this->asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, data, false);
                }
                if (result & WIDGET_RELEASED) {
                    //edit finished, send complete reversible Action
                    this->asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, data, m_tmp, true);
                }
            }
        }

        virtual Floats<2> getSizeHint() const override { return { this->m_sizeHint[0], WidgetLayout().height(N) }; }

    private:
        PropertyType& m_property;
        T m_tmp{ T() };
    };
}
