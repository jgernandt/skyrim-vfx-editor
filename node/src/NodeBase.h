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

#include "Connector.h"
#include "Window.h"

#include "nif.h"

#include "node_concepts.h"
#include "node_devices.h"
#include "node_traits.h"

namespace node
{
	struct Field
	{
		Field(const std::string& name) : name{ name } {}
		virtual ~Field() = default;

		std::string name;
		gui::IComponent* widget{ nullptr };//or gui::Widget?
		gui::Connector* connector{ nullptr };
	};

	//We should think of connectors as leading UP or DOWN the ni hierarchy, rather than being input or output.
	//The latter may not always make sense.
	enum class ConnectorType
	{
		UP,
		DOWN
	};

	class NodeBase : public gui::Window
	{
	protected:
		NodeBase();
		NodeBase(const NodeBase&) = delete;

	public:
		virtual ~NodeBase();

		NodeBase& operator=(const NodeBase&) = delete;

		virtual void onClose() override;

		gui::Connector* addConnector(const std::string& label, ConnectorType type, std::unique_ptr<gui::Connector>&& obj);

		//Fields systems is undergoing major revision. May be removed altogether.
		//Currently we keep pointers here, but our derived classes own the resources.
		//We do not touch them during destruction.
		Field* getField(const std::string& name);

		template<typename T, typename... Args>
		[[nodiscard]] std::unique_ptr<T> newField(const std::string& name, Args&&... args)
		{
			//ctor of T should add any components to us and fill itself out
			auto obj = std::make_unique<T>(name, std::forward<Args>(args)...);
			auto result = m_fields.insert({ name, obj.get() });
			//result is pair<iterator, bool>
			assert(result.second);
			return obj;
		}

	protected:
		//quick workaround to avoid relying on our destructor for safe disconnection
		void disconnect();

	private:
		class LeftController final :
			public gui::IPlacementController
		{
		public:
			LeftController(const gui::Window& window) : m_window{ window } {}
			virtual gui::Floats<2> place(const gui::Floats<2>& hint) override;

		private:
			const gui::Window& m_window;
		};

		class RightController final :
			public gui::IPlacementController
		{
		public:
			RightController(const gui::Window& window) : m_window{ window } {}
			virtual gui::Floats<2> place(const gui::Floats<2>& hint) override;

		private:
			const gui::Window& m_window;
		};

		LeftController m_leftCtlr;
		RightController m_rightCtlr;
		std::map<std::string, Field*> m_fields;
	};

	//Used to create nodes, or parts of nodes, using default objects.
	//Define the type default_object on any default-constructible node,
	//or specialise the template.
	template<typename T>
	class Default
	{
	public:
		std::unique_ptr<T> create(nif::File& file, 
			const ni_ptr<typename T::default_object>& obj = ni_ptr<typename T::default_object>())
		{ 
			if (obj)
				return std::make_unique<T>(obj);
			else
				return std::make_unique<T>(file.create<typename T::default_object>());
		}
	};

	//Called when loading a file to identify connections between nodes
	template<typename T>
	class Connector : public VerticalTraverser<T, Connector>
	{
	public:
		template<typename ConstructorType>
		bool operator() (T&, ConstructorType&) { return true; }
	};

	//Called when loading a file to create a new node of the appropriate type
	template<typename T>
	class Factory : public VerticalTraverser<T, Factory>
	{
	public:
		template<typename ConstructorType>
		bool operator() (T&, ConstructorType&) { return true; }
	};
	template<typename T>
	class RootFactory : public VerticalTraverser<T, RootFactory>
	{
	public:
		template<typename ConstructorType>
		bool operator() (T&, ConstructorType&) { return true; }
	};

	//Called when loading a file to forward to subnodes
	template<typename T>
	class Forwarder : public VerticalTraverser<T, Forwarder>
	{
	public:
		template<typename ConstructorType>
		bool operator() (T&, ConstructorType&) { return true; }
	};
}