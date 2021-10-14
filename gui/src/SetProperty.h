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
#include "ICommand.h"

namespace gui
{
	template<typename T, typename PropertyType, template<typename> typename ConverterType = GuiConverter>
	class SetProperty final : public ICommand
	{
	private:
		//A type accepted by our property's set function
		using set_type = typename util::property_traits<PropertyType>::value_type;

	public:
		//If reversible, reverts to the value of prop at the time of execution
		SetProperty(PropertyType& prop, const T& val, bool reversible) :
			m_property{ prop }, m_value{ val }, m_oldValue{ T() }, m_reversible{ reversible }, m_async{ false } {}

		//Reverts to the value old, regardless of the current value of the property
		//(wouldn't really make sense to set reversible = false here, but requiring the argument makes for a convenient
		//way to disambiguate the call when T = bool)
		SetProperty(PropertyType& prop, const T& val, const T& old, bool reversible) :
			m_property{ prop }, m_value{ val }, m_oldValue{ old }, m_reversible{ reversible }, m_async{ true } {}

		virtual void execute() override
		{
			if (m_reversible && !m_async)
				//Read old value from the property (convert if needed)
				m_oldValue = util::type_conversion<T, ConverterType<T>>::from(
					util::property_traits<PropertyType>::get(m_property));

			//Set new value
			util::property_traits<PropertyType>::set(m_property,
				util::type_conversion<set_type, ConverterType<set_type>>::from(m_value));
		}
		virtual void reverse() override
		{
			//Set old value
			util::property_traits<PropertyType>::set(m_property,
				util::type_conversion<set_type, ConverterType<set_type>>::from(m_oldValue));
		}
		virtual bool reversible() const override
		{
			return m_reversible && m_value != m_oldValue;
		}

	protected:
		PropertyType& m_property;
		T m_value;
		T m_oldValue;
		bool m_reversible;
		bool m_async;
	};
}