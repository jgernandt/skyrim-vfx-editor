// Copyright 2021 Jonas Gernandt
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
#include "ICommand.h"
#include "Composition.h"
#include "traits.h"

namespace gui
{
	//The required format our widgets' input event sinks
    template<typename PropertyType>
    struct InputEventSinkTemplate
    {
		using value_type = typename util::property_traits<PropertyType>::value_type;

        void begin(const PropertyType& p, IComponent* source) {}
        void update(PropertyType& p, IComponent* source, const value_type& val) {}
        void end(PropertyType& p, IComponent* source) {}
    };

	template<typename PropertyType>
	class DefaultSetCommand final : public ICommand
	{
		using value_type = typename util::property_traits<PropertyType>::value_type;

	public:
		DefaultSetCommand(const PropertyType& prop, const value_type& to, const value_type& from) :
			m_property{ prop }, m_to{ to }, m_from{ from } {}

		virtual void execute() override
		{
			util::property_traits<PropertyType>::set(m_property, m_to);
		}
		virtual void reverse() override
		{
			util::property_traits<PropertyType>::set(m_property, m_from);
		}
		virtual bool reversible() const override
		{
			return m_to != m_from;
		}

	protected:
		PropertyType m_property;
		value_type m_to;
		value_type m_from;
	};

    template<typename PropertyType>
    struct DefaultEventSink
    {
        using value_type = typename util::property_traits<PropertyType>::value_type;

        void begin(const PropertyType& p, IComponent* source)
        {
            m_init = util::property_traits<PropertyType>::get(p);
        }
        void update(PropertyType& p, IComponent* source, const value_type& val)
        {
			static_assert(SINGLE_THREAD);
            util::property_traits<PropertyType>::set(p, val);
        }
        void end(PropertyType& p, IComponent* source)
        {
			if (IInvoker* inv = source->getInvoker())
				inv->queue(std::make_unique<DefaultSetCommand<PropertyType>>(
					p, util::property_traits<PropertyType>::get(p), m_init));
        }

        value_type m_init{ value_type() };
    };
}
