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

namespace gui
{
	class ICommand
	{
	public:
		virtual ~ICommand() {}

		virtual void execute() = 0;
		virtual void reverse() = 0;
		virtual bool reversible() const = 0;//guaranteed to be called AFTER execute
		//virtual bool linked() = 0;//should this action be linked to the preceding? Bad idea?
	};
}