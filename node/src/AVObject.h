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
#include "NodeBase.h"

namespace node
{
	using namespace nif;

	class ObjectNET : public NodeBase
	{
	protected:
		ObjectNET();

	public:
		virtual ~ObjectNET() = default;

	public:
		constexpr static const char* OBJECT = "References";
		constexpr static const char* NAME = "Name";
		constexpr static const char* EXTRA_DATA = "Extra data";

	protected:
		template<typename T>
		class ObjectField final : public Field
		{
		public:
			ObjectField(const std::string& name, ObjectNET& node, const ni_ptr<T>& obj) : 
				Field(name), m_rvr(obj)
			{
				connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
			}

		private:
			PtrReceiver<T> m_rvr;
			Sender<void> m_sdr;
		};
		class NameField final : public Field
		{
		public:
			NameField(const std::string& name, NodeBase& node, ni_ptr<Property<std::string>>&& name2);
		};

		class ExtraDataField final : public Field
		{
		public:
			ExtraDataField(const std::string& name, NodeBase& node, ni_ptr<Set<NiExtraData>>&& extraData);

		private:
			Receiver<void> m_rvr;
			Sender<Set<NiExtraData>> m_sdr;
		};

	protected:
		std::unique_ptr<Field> m_name;
		std::unique_ptr<Field> m_extraData;
		std::unique_ptr<Field> m_references;
	};

	class AVObject : public ObjectNET
	{
	protected:
		AVObject(const ni_ptr<NiAVObject>& obj);

	public:
		virtual ~AVObject() = default;

	public:
		constexpr static const char* PARENT = "Parent";
		constexpr static const char* TRANSFORM = "Transform";

	protected:
		//these dummies are used to make RotationAdapter three distinct property types
		class DummyClass1 {};
		class DummyClass2 {};

		//Calculates the values to be displayed by the rotation widget.
		class RotationAdapter final : 
			public PropertyListener<rotation_t>, public DummyClass1, public DummyClass2
		{
		public:
			RotationAdapter(const ni_ptr<NiAVObject>& obj);
			~RotationAdapter();

			//Recalculate our stored value
			virtual void onSet(const math::Rotation& rot) override;

			//Calculate and set backend property
			void setEuler(const math::Rotation::euler_type& euler);

			//Set a different display format. Recalulates stored values and updates
			//the UI, as appropriate.
			void setEulerOrder(math::EulerOrder order);
			void setIntrinsic(bool intrinsic);

			void updateUI(gui::Composite* root);

			ni_ptr<Property<math::Rotation>> m_backend;
			gui::Composite* m_guiRoot{ nullptr };
			math::Rotation::euler_type m_current;
			bool m_intrinsic{ false };
		};
		friend struct util::property_traits<DummyClass1*>;
		friend struct util::property_traits<DummyClass2*>;
		friend struct util::property_traits<RotationAdapter*>;

		class ParentField final : public Field
		{
		public:
			ParentField(const std::string& name, NodeBase& node, const ni_ptr<NiAVObject>& object);

		private:
			SetReceiver<NiAVObject> m_rvr;
			Sender<void> m_sdr;
		};

		class TransformField final : public Field
		{
		public:
			TransformField(const std::string& name, AVObject& node, ni_ptr<Transform>&& transform);
		};

		RotationAdapter m_rotAdapter;
		std::unique_ptr<Field> m_parent;
		std::unique_ptr<Field> m_transform;
	};

	class DummyAVObject final : public AVObject
	{
	public:
		DummyAVObject(const ni_ptr<NiAVObject>& obj);
		~DummyAVObject();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 105.0f;
	};
}