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
#include <exception>
#include <map>

#include "Connector.h"
#include "Window.h"

#include "NiController.h"

//We need to rework the ownership of resources.
//The current design was intended to make safe destruction simple (and it does),
//but resource ownership is hard to setup and even harder to overview.

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

	class NodeBase :
		public gui::Window, public IObservable<ISet<nif::NiTimeController>>
	{
	public:
		class failed_construction final :
			public std::exception
		{
		public:
			failed_construction() : exception("Failed Node construction") {}
		};

	public:
		NodeBase(std::unique_ptr<nif::NiObject>&& obj);
		NodeBase(const NodeBase&) = delete;

		virtual ~NodeBase();

		NodeBase& operator=(const NodeBase&) = delete;

		virtual void accept(gui::Visitor& v) override;
		virtual void onClose() override;

		virtual nif::NiObject& object();

		gui::Connector* addConnector(const std::string& label, ConnectorType type, std::unique_ptr<gui::Connector>&& obj);

		Field* getField(const std::string& name);

		template<typename T, typename... Args>
		T* newField(const std::string& name, Args&&... args)
		{
			//ctor of T should add any components to us and fill itself out
			auto obj = std::make_unique<T>(name, std::forward<Args>(args)...);
			T* ptr = obj.get();
			auto result = m_fields.insert({ name, std::move(obj) });
			//result is pair<iterator, bool>
			return result.second ? ptr : nullptr;
		}

		void addObject(std::unique_ptr<nif::NiObject>&& obj) { m_objects.push_back(std::move(obj)); }
		const std::vector<std::unique_ptr<nif::NiObject>>& getObjects() const { return m_objects; }

		//We keep the controllers separate from other objects since we expect them to be dynamic
		void addController(std::unique_ptr<nif::NiTimeController>&& obj) { m_controllers.push_back(std::move(obj)); }
		void removeController(nif::NiTimeController* obj);
		const std::vector<std::unique_ptr<nif::NiTimeController>>& getControllers() const { return m_controllers; }

		virtual void addListener(SetListener<nif::NiTimeController>& l) override { m_obsImpl.addListener(l); }
		virtual void removeListener(SetListener<nif::NiTimeController>& l) override { m_obsImpl.removeListener(l); }

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
		std::vector<std::unique_ptr<nif::NiObject>> m_objects;
		std::vector<std::unique_ptr<nif::NiTimeController>> m_controllers;
		ObservableImpl<ISet<nif::NiTimeController>> m_obsImpl;
		std::map<std::string, std::unique_ptr<Field>> m_fields;
	};

	/*template<typename ObjectType>
	class SimpleNode : public NodeBase
	{
	public:
		SimpleNode(std::unique_ptr<ObjectType>&& obj) : m_object{ std::move(obj) } {}
		virtual ~SimpleNode()
		{
			//Disconnect all our fields
			for (auto&& field : m_fields) {
				assert(field.second);
				if (field.second->connector)
					field.second->connector->disconnect();
			}

			//Shouldn't be needed, but might as well:
			m_fields.clear();

			//To be very safe, we should (1) remove the Connectors, (2) clear the Fields, (3) let m_object expire.
			//This is the reverse order of their construction, and their chain of dependence.
			//However, just disconnecting should be fine.
		}

		ObjectType& object() { assert(m_object); return *m_object; }

	protected:
		std::unique_ptr<ObjectType> m_object;
	};

	//Drop SimpleNode and merge this functionality into NodeBase?
	class CompoundNode : public NodeBase
	{
	public:
		CompoundNode() = default;

		virtual ~CompoundNode()
		{
			for (auto&& field : m_fields) {
				assert(field.second);
				if (field.second->connector)
					field.second->connector->disconnect();
			}
			m_fields.clear();
		}

	protected:
		void addObject(std::unique_ptr<nif::NiObject>&& obj) { m_objects.push_back(std::move(obj)); }
		const std::vector<std::unique_ptr<nif::NiObject>>& getObjects() const { return m_objects; }

	private:
		std::vector<std::unique_ptr<nif::NiObject>> m_objects;
	};*/
}