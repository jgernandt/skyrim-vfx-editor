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
#include "NiObjectNET.h"

namespace nif
{
	class NiAVObject : public NiObjectNET
	{
	public:
		using native_type = native::NiAVObject;

	protected:
		friend class File;
		NiAVObject(native_type* obj);

	public:
		virtual ~NiAVObject() = default;

		native_type& getNative() const;

		Transformable& transform();
		std::shared_ptr<Transformable> transform_ptr();

	private:
		struct AVObjectTransform final : Transformable
		{
			AVObjectTransform(NiAVObject& super);

			virtual IProperty<translation_t>& translation() override;
			virtual IProperty<rotation_t>& rotation() override;
			virtual IProperty<scale_t>& scale() override;

			virtual std::shared_ptr<IProperty<translation_t>> translation_ptr() override;
			virtual std::shared_ptr<IProperty<rotation_t>> rotation_ptr() override;
			virtual std::shared_ptr<IProperty<scale_t>> scale_ptr() override;

		private:
			Property<translation_t, native::translation_t> m_T;
			Property<rotation_t, native::rotation_t> m_R;
			Property<scale_t> m_S;

			NiAVObject& m_super;

		} m_transform;
	};
}
