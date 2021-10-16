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
		NiObjectNET(native::NiObjectNET* obj);
		NiObjectNET(const NiObjectNET& other) = delete;

		virtual ~NiObjectNET() = default;

		NiObjectNET& operator=(const NiObjectNET&) = delete;

		native::NiObjectNET& getNative() const;

		IProperty<std::string>& name() { return m_name; }
		ISet<NiExtraData>& extraData() { return m_extraData; }
		ISequence<NiTimeController>& controllers() { return m_controllers; }

	private:
		Property<std::string> m_name;

		struct ExtraData final : SetBase<NiExtraData>
		{
			ExtraData(NiObjectNET& super) : m_super{ super } {}

			virtual void add(const NiExtraData& obj) override;
			virtual void remove(const NiExtraData& obj) override;
			virtual bool has(const NiExtraData& obj) const override;
			virtual size_t size() const override;

			NiObjectNET& m_super;

		} m_extraData;

		struct Controllers final : SequenceBase<NiTimeController>
		{
			Controllers(NiObjectNET& super) : m_super{ super } {}

			virtual size_t insert(size_t pos, const NiTimeController& obj) override;
			virtual size_t erase(size_t pos) override;
			virtual size_t find(const NiTimeController& obj) const override;
			virtual size_t size() const override;

			NiObjectNET& m_super;

		} m_controllers;

	};
}