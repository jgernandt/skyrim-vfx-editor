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
#include <map>

#include "gui_types.h"

#include "CallWrapper.h"
#include "Widget.h"
#include "UniqueLabel.h"


namespace gui
{
	namespace backend
	{
		util::CallWrapper Selector(const std::string& label, const std::string& preview);
		bool SelectableItem(const std::string& label, bool selected);
		bool ToggleableItem(const std::string& label, bool toggled);
	}

	template<typename T, typename PropertyType, template<typename> typename ConverterType = GuiConverter>
	class Selector final :
		public Component
	{
	public:
		typedef std::map<T, std::string> ItemList;

	public:
		Selector(PropertyType& prop, const std::string& label, ItemList&& items) :
			m_property{ prop }, m_label(label), m_items{ std::move(items) } 
		{
			setSizeHint({ -1.0f, -1.0f });
		}

		virtual void frame(FrameDrawer& fd) override
		{
			T data = util::type_conversion<T, ConverterType<T>>::from(util::property_traits<PropertyType>::get(m_property));

			auto it = m_items.find(data);//Get preview string

			if (auto&& result = backend::Selector(m_label[0], it != m_items.end() ? it->second : std::string())) {
				for (auto&& item : m_items) {
					if (backend::SelectableItem(item.second, data == item.first))
						asyncInvoke<SetProperty<T, PropertyType, ConverterType>>(m_property, item.first, true);
				}
			}
		}
		virtual Floats<2> getSizeHint() const override { return { m_sizeHint[0], getDefaultHeight() }; }

	private:
		PropertyType& m_property;
		UniqueLabel<1> m_label;
		ItemList m_items;
	};

	//We might be able to merge them, but no hurry
	template<typename PropertyType>
	class StringSelector final :
		public Component
	{
	public:
		typedef std::set<std::string> ItemList;

		//We only work with std::string, for now
		static_assert(std::is_assignable<std::string&, typename util::property_traits<PropertyType>::get_type>::value,
			"StringSelector needs a std::string");

	public:
		StringSelector(PropertyType& prop, const std::string& label, ItemList&& items) :
			m_property{ prop }, m_label(label), m_items{ std::move(items) }
		{
			setSizeHint({ -1.0f, -1.0f });
		}

		virtual void frame(FrameDrawer& fd) override
		{
			std::string str = util::property_traits<PropertyType>::get(m_property);

			auto it = m_items.find(str);//Get preview string

			if (auto&& result = backend::Selector(m_label[0], it != m_items.end() ? *it : std::string())) {
				for (auto&& item : m_items) {
					if (backend::SelectableItem(item, str == item)) {
						util::property_traits<PropertyType>::set(m_property, str);
					}
				}
			}
		}
		virtual Floats<2> getSizeHint() const override { return { m_sizeHint[0], getDefaultHeight() }; }

	private:
		PropertyType& m_property;
		UniqueLabel<1> m_label;
		ItemList m_items;
	};


	template<typename FieldType>
	class FlagSelector final :
		public Component
	{
	private:
		using index_type = typename util::field_traits<FieldType>::index_type;
		using value_type = typename util::field_traits<FieldType>::value_type;//would this ever not be bool?
		static_assert(std::is_assignable<bool&, value_type>::value);

	public:
		typedef std::map<index_type, std::string> ItemList;

	public:
		FlagSelector(FieldType& prop, const std::string& label, ItemList&& items) :
			m_property{ prop }, m_label{ "" }, m_preview{ label }, m_items{ std::move(items) }
		{
			setSizeHint({ -1.0f, -1.0f });
		}

		virtual void frame(FrameDrawer& fd) override
		{
			if (auto&& result = backend::Selector(m_label[0], m_preview)) {
				for (auto&& item : m_items) {
					bool set = util::field_traits<FieldType>::get(m_property, item.first);
					if (backend::ToggleableItem(item.second, set)) {
						util::field_traits<FieldType>::set(m_property, item.first, static_cast<value_type>(!set));
					}
				}
			}
		}
		virtual Floats<2> getSizeHint() const override { return { m_sizeHint[0], getDefaultHeight() }; }

	private:
		FieldType& m_property;
		UniqueLabel<1> m_label;
		std::string m_preview;
		ItemList m_items;
	};
}

