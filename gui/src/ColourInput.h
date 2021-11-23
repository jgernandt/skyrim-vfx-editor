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
#include "Widget.h"
#include "UniqueLabel.h"


namespace gui
{
	namespace backend
	{
		unsigned int ColourInput(const std::string& label, float* arr);
	}

	template<typename T, typename PropertyType, template<typename> typename ConverterType = GuiConverter>
	class ColourInput final : public Component
	{
	private:
		using element_type = typename util::colour_traits<T>::component_type;

		//The type returned by our property's get function
		using get_type = typename util::property_traits<PropertyType>::get_type;
		//A type accepted by our property's set function
		using set_type = typename util::property_traits<PropertyType>::value_type;

		//We need a 4-channel array colour type
		static_assert(util::colour_traits<T>::is_array);
		static_assert(util::colour_traits<T>::channels >= 4);

		//The components must convert to float
		static_assert(std::is_convertible<element_type, float>::value);

		//The property's get must be assignable to T
		using converted_type = decltype(util::type_conversion<T, ConverterType<T>>::from(std::declval<get_type>()));
		static_assert(std::is_assignable<T&, converted_type>::value, "property return cannot be assigned to value type");

	public:
		ColourInput(const PropertyType& p, const std::string& label) : m_property{ p }, m_label(label) {}

		virtual void frame(FrameDrawer& fd) override
		{
			//Read from the property, convert if needed
			T col = util::type_conversion<T, ConverterType<T>>::from(util::property_traits<PropertyType>::get(m_property));

			//Ugly solution, but when you click a colour widget imgui immediately writes that value to our variable.
			//We never get a chance to save the original. There has to be some better way, though.
			T tmp = col;

			unsigned int result = backend::ColourInput(m_label[0], &util::colour_traits<T>::R(col));

			//Should these be virtual functions, and we decide the invocation procedure on a higher level?
			if (result & WIDGET_ACTIVATED) {
				//edit started, store original value
				m_tmp = tmp;
			}
			if (result & WIDGET_EDITED) {
				//Edit in progress, send irreversible Action.
				this->asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, col, false);
			}
			if (result & WIDGET_RELEASED) {
				//edit finished, send complete reversible Action
				this->asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, col, m_tmp, true);
			}
		}

		virtual Floats<2> getSizeHint() const override
		{ 
			float h = getDefaultHeight();
			return { h, h }; 
		}

	private:
		PropertyType m_property;
		UniqueLabel<1> m_label;
		T m_tmp{ T() };
	};
}