//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
#include <string>

namespace gui
{
	//Append a unique tag to labels, as required by dear imgui
	template<size_t N>
	class UniqueLabel
	{
	public:
		UniqueLabel(const std::string& label = std::string())
		{
			reset(label);
		}
		UniqueLabel(const std::array<std::string, N>& labels)
		{
			reset(labels);
		}

		void reset(const std::string& label)
		{
			for (size_t i = 0; i < N; i++)
				at[i] = label + "##" + std::to_string(reinterpret_cast<uintptr_t>(this)) + std::to_string(i);
		}

		void reset(const std::array<std::string, N>& labels)
		{
			for (size_t i = 0; i < N; i++)
				at[i] = labels[i] + "##" + std::to_string(reinterpret_cast<uintptr_t>(this)) + std::to_string(i);
		}

		const std::string& operator[](size_t i) const { return at[i]; }

	private:
		std::string at[N];
	};
}