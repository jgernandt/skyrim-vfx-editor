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

#pragma once
#include "gui_types.h"

#include "Widget.h"
#include "UniqueLabel.h"


namespace gui
{
	namespace backend
	{
		unsigned int TextInput(const std::string& label, std::string* str);
	}

	template<typename PropertyType>
	class TextInput final :
		public Component
	{
	public:
		TextInput(PropertyType& prop, const std::string& label = std::string()) :
			m_property{ prop }, m_label(label) {}

		//We only work with std::string, for now
		static_assert(std::is_assignable<std::string&, typename util::property_traits<PropertyType>::get_type>::value,
			"TextInput needs a std::string");

		virtual void frame() override
		{
			std::string s = util::property_traits<PropertyType>::get(m_property);

			unsigned int result = backend::TextInput(m_label[0], &s);
			if (result & WIDGET_ACTIVATED)
				m_tmp = s;
			if (result & WIDGET_RELEASED)
				asyncInvoke<SetProperty<std::string, PropertyType>>(m_property, s, m_tmp, true);
		}

		virtual Floats<2> getSizeHint() const override { return { -1.0f, getDefaultHeight() }; }

	private:
		PropertyType& m_property;
		UniqueLabel<1> m_label;
		std::string m_tmp;
	};
}

