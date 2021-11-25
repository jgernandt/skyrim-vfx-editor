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

	class ExtraData : public NodeBase
	{
	protected:
		ExtraData(const ni_ptr<NiExtraData>& obj);

	public:
		virtual ~ExtraData() = default;

		constexpr static const char* TARGET = "Targets";
		constexpr static const char* NAME = "Name";

	protected:
		class TargetField final : public Field
		{
		public:
			TargetField(const std::string& name, NodeBase& node, const ni_ptr<NiExtraData>& obj);

		private:
			SetReceiver<NiExtraData> m_rvr;
			Sender<void> m_sdr;
		};

		std::unique_ptr<Field> m_targetField;
	};

	class StringDataShared : public ExtraData
	{
	protected:
		StringDataShared(const ni_ptr<NiStringExtraData>& obj);

	public:
		virtual ~StringDataShared() = default;

		constexpr static const char* VALUE = "Value";
	};

	class StringData final : public StringDataShared
	{
	public:
		using default_object = NiStringExtraData;

		StringData(ni_ptr<NiStringExtraData>&& obj);
		~StringData();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 140.0f;
	};

	class WeaponTypeData final : public StringDataShared
	{
	public:
		using default_object = NiStringExtraData;

		WeaponTypeData(ni_ptr<NiStringExtraData>&& obj);
		~WeaponTypeData();

		constexpr static const char* TYPE = "Type";
		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 90.0f;
	};

	class DummyExtraData final : public ExtraData
	{
	public:
		DummyExtraData(ni_ptr<NiExtraData>&& obj);
		~DummyExtraData();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 90.0f;
	};
}