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
#include "node_concepts.h"
#include "node_traits.h"
#include "DeviceImpl.h"
#include "NodeBase.h"

#include "NiExtraData.h"

namespace node
{
	class ExtraData : public NodeBase
	{
	protected:
		ExtraData(std::unique_ptr<nif::NiExtraData>&& obj);

	public:
		virtual ~ExtraData() = default;
		virtual nif::NiExtraData& object() override;

	public:
		constexpr static const char* TARGET = "Targets";
		constexpr static const char* NAME = "Name";

	protected:
		class TargetField final : public Field
		{
		public:
			TargetField(const std::string& name, ExtraData& node);

		private:
			SetReceiver<nif::NiExtraData> m_rvr;
			Sender<void> m_sdr;
		};

		class NameField final : public Field
		{
		public:
			NameField(const std::string& name, ExtraData& node);
		};
	};

	class StringDataShared : public ExtraData
	{
	protected:
		StringDataShared(std::unique_ptr<nif::NiStringExtraData>&& obj);

	public:
		virtual ~StringDataShared() = default;
		virtual nif::NiStringExtraData& object() override;

	public:
		constexpr static const char* VALUE = "Value";
	};

	class StringData final : public StringDataShared
	{
	public:
		StringData();
		StringData(std::unique_ptr<nif::NiStringExtraData>&& obj);
	};

	class WeaponTypeData final : public StringDataShared
	{
	public:
		WeaponTypeData();
		WeaponTypeData(std::unique_ptr<nif::NiStringExtraData>&& obj);

	public:
		constexpr static const char* TYPE = "Type";
	};

	class DummyExtraData final : public ExtraData
	{
	public:
		DummyExtraData(std::unique_ptr<nif::NiExtraData>&& obj);
	};
}