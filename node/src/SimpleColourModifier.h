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
#include "Modifier.h"

namespace node
{
	class SimpleColourModifier final : public Modifier
	{
	public:
		SimpleColourModifier();
		SimpleColourModifier(std::unique_ptr<nif::BSPSysSimpleColorModifier>&& obj);

	public:
		virtual nif::BSPSysSimpleColorModifier& object() override;

	private:
		class ColourField;

	};
}
