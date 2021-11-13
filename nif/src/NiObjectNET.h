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
#include "NiExtraData.h"

namespace nif
{
	class NiTimeController;
	class NiObjectNET : public NiObject
	{
	public:
		using native_type = native::NiObjectNET;

	protected:
		friend class File;
		NiObjectNET(native_type* obj);

	public:
		virtual ~NiObjectNET() = default;

		native_type& getNative() const;

		Property<std::string>& name() { return m_name; }
		Set<NiExtraData>& extraData() { return m_extraData; }
		Sequence<NiTimeController>& controllers() { return m_controllers; }

	private:
		PropertyFcn<std::string, NiObjectNET> m_name;

		struct ExtraData final : SetBase<NiExtraData, NiObjectNET>
		{
			ExtraData(NiObjectNET& block) : SetBase<NiExtraData, NiObjectNET>{ block } {}

			virtual void add(const NiExtraData& obj) override;
			virtual void remove(const NiExtraData& obj) override;
			virtual bool has(const NiExtraData& obj) const override;
			virtual size_t size() const override;

		} m_extraData;

		struct Controllers final : SequenceBase<NiTimeController, NiObjectNET>
		{
			Controllers(NiObjectNET& block) : SequenceBase<NiTimeController, NiObjectNET>{ block } {}

			virtual size_t insert(size_t pos, const NiTimeController& obj) override;
			virtual size_t erase(size_t pos) override;
			virtual size_t find(const NiTimeController& obj) const override;
			virtual size_t size() const override;

		} m_controllers;

	};
}