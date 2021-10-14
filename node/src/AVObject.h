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
#include "node_concepts.h"
#include "node_traits.h"
#include "DeviceImpl.h"
#include "NodeBase.h"

#include "NiNode.h"

namespace node
{
	class ObjectNET : public NodeBase
	{
	protected:
		ObjectNET(std::unique_ptr<nif::NiObjectNET>&& obj);

	public:
		virtual ~ObjectNET() = default;
		virtual nif::NiObjectNET& object() override;

	public:
		constexpr static const char* OBJECT = "References";
		constexpr static const char* NAME = "Name";
		constexpr static const char* EXTRA_DATA = "Extra data";

	protected:
		template<typename T>
		class ObjectField final : public Field
		{
		public:
			ObjectField(const std::string& name, ObjectNET& node, T& obj) : 
				Field(name), m_rvr(obj)
			{
				connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::MultiConnector>(m_sdr, m_rvr));
			}

		private:
			AssignableReceiver<T> m_rvr;
			Sender<void> m_sdr;
		};
		class NameField final : public Field
		{
		public:
			NameField(const std::string& name, ObjectNET& node);
		};

		class ExtraDataField final : public Field
		{
		public:
			ExtraDataField(const std::string& name, ObjectNET& node);

		private:
			Receiver<void> m_rvr;
			Sender<ISet<nif::NiExtraData>> m_sdr;
		};

	};

	class AVObject : public ObjectNET
	{
	protected:
		AVObject(std::unique_ptr<nif::NiAVObject>&& obj);

	public:
		virtual ~AVObject() = default;
		virtual nif::NiAVObject& object() override;

	public:
		constexpr static const char* PARENT = "Parent";
		constexpr static const char* TRANSFORM = "Transform";

	protected:
		class ParentField final : public Field
		{
		public:
			ParentField(const std::string& name, AVObject& node);

		private:
			SetReceiver<nif::NiAVObject> m_rvr;
			Sender<void> m_sdr;
		};

		class TransformField final : public Field
		{
		public:
			TransformField(const std::string& name, AVObject& node);
		};

	};

	class DummyAVObject final : public AVObject
	{
	public:
		DummyAVObject(std::unique_ptr<nif::NiAVObject>&& obj);

	};
}