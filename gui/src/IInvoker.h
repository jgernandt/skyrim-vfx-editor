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
#include <memory>
#include <utility>//std::forward
#include "ICommand.h"

namespace gui
{
	class IInvoker
	{
	public:
		virtual ~IInvoker() {}

		virtual void invoke() = 0;
		virtual void queue(std::unique_ptr<ICommand>&&) = 0;
		virtual void undo() = 0;
		virtual void redo() = 0;

		template<typename T, typename ...Args>
		void queue(Args&& ...args)
		{
			queue(std::make_unique<T>(std::forward<Args>(args) ...));
		}
	};
}