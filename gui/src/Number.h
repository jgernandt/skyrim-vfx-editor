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
#include "gui_types.h"
#include "Widget.h"

namespace gui
{
	class NumberBase :
		public Component
	{
	public:

		virtual void frame() override;
		virtual Floats<2> getSizeHint() const override;
		virtual void print(char* buf, size_t N) const = 0;
	};

	template<typename T, typename PropertyType, template<typename> typename ConverterType = GuiConverter>
	class Number final :
		public NumberBase
	{
	public:
		Number(const PropertyType& p, const char* format) :
			m_property{ p }, m_format{ format } {}

		virtual void print(char* buf, size_t N) const final override
		{
			T value = util::type_conversion<T, ConverterType<T>>::from(util::property_traits<PropertyType>::get(m_property));
			snprintf(buf, N, m_format, value);
		}

	private:
		const PropertyType& m_property;
		const char* m_format;
	};
}
