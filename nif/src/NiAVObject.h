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
#include "NiObjectNET.h"

namespace nif
{
	class NiAVObject : public NiObjectNET
	{
	public:
		NiAVObject(native::NiAVObject* obj);
		NiAVObject(const NiAVObject& other) = delete;

		virtual ~NiAVObject() = default;

		NiAVObject& operator=(const NiAVObject&) = delete;

		native::NiAVObject& getNative() const;

		Transformable& transform() { return m_transform; }

	private:
		struct AVObjectTransform final : Transformable
		{
			AVObjectTransform(NiAVObject& super);
			
			virtual IProperty<translation_t>& translation() final override { return m_T; }
			virtual IProperty<rotation_t>& rotation() final override { return m_R; }
			virtual IProperty<scale_t>& scale() final override { return m_S; }

		private:
			Property<translation_t, native::translation_t> m_T;
			Property<rotation_t, native::rotation_t> m_R;
			Property<scale_t> m_S;

		} m_transform;
	};
}
