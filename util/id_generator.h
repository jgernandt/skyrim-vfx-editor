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
#include <cassert>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace util
{
	template<typename int_type>
	class IDGenerator
	{
		static_assert(std::is_integral<int_type>::value);
	public:
		int_type get()
		{
			if (m_exhausted)
				throw std::runtime_error("IDGenerator exhausted");

			if (m_next == std::numeric_limits<int_type>::max()) {
				m_exhausted = true;
				return m_next;
			}
			else
				return m_next++;
		}

	private:
		int_type m_next = 0;
		bool m_exhausted = false;
	};
}