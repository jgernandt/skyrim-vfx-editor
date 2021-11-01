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

#include "Composition.h"

namespace gui
{
	namespace backend
	{
		unsigned int text(const std::string& str);
		Floats<2> textSize(const std::string& str);
	}

	class Text final : public Component
	{
	public:
		Text(const std::string& text) : m_text{ text } {}
		virtual void frame(FrameDrawer& fd) override;
		virtual Floats<2> getSizeHint() const override;

		void setWrap(bool wrap = true) { m_wrap = wrap; }

	private:
		std::string m_text;
		bool m_wrap{ false };
	};

	//A string literal with frame padding
	class Label final : public Component
	{

	};

	template<typename PropertyType>
	class TextProperty final : public Component
	{
	public:
		TextProperty(const PropertyType& prop) : m_property{ prop } {}

		//We only work with std::string, for now
		static_assert(std::is_assignable<std::string&, typename util::property_traits<PropertyType>::get_type>::value,
			"TextProperty needs a std::string");

		virtual void frame(FrameDrawer& fd) override
		{
			std::string s = util::property_traits<PropertyType>::get(m_property);

			backend::text(s);
		}

		virtual Floats<2> getSizeHint() const override
		{ 
			return backend::textSize(util::property_traits<PropertyType>::get(m_property));
		}

	private:
		const PropertyType& m_property;
	};
}
