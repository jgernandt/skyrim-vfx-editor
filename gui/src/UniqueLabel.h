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
#include <string>
#include "id_generator.h"

namespace gui
{
	//Append a unique tag to labels, as required by dear imgui
	template<int N>
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
				at[i] = label + "##" + std::to_string(s_generator.get()) + "N" + std::to_string(N);
		}

		void reset(const std::array<std::string, N>& labels)
		{
			for (size_t i = 0; i < N; i++)
				at[i] = labels[i] + "##" + std::to_string(s_generator.get()) + "N" + std::to_string(N);
		}

		const std::string& operator[](size_t i) const { return at[i]; }

	private:
		std::string at[N];

		static util::IDGenerator<int> s_generator;
	};

	template<int N>
	util::IDGenerator<int> UniqueLabel<N>::s_generator{};
}