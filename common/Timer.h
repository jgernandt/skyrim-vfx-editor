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

#include <chrono>

template<typename tick_t = long long, typename period_t = std::nano>
class Timer
{
private:
	using clock_t = std::chrono::steady_clock;
	using duration_t = std::chrono::duration<tick_t, period_t>;
	using time_t = std::chrono::time_point<clock_t>;
public:
	Timer()	{ m_startTime = clock_t::now(); }
	~Timer() {}

	tick_t elapsed() const { return std::chrono::duration_cast<duration_t>(clock_t::now() - m_startTime).count();	}
	void reset() { m_startTime = clock_t::now(); }

private:

	time_t m_startTime;
};