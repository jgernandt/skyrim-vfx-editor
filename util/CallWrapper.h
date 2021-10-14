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
#include <functional>

namespace util
{
	//Wrap a future call in an exception-safe manner.
	//Note: std::bind may itself throw, if a constructor of any of the decayed types throws. But when would that happen?
	template<typename... Args>
	class CallWrapper
	{
	public:
		CallWrapper() {}

		//Disallow copying
		CallWrapper(const CallWrapper&) = delete;
		CallWrapper& operator=(const CallWrapper&) = delete;

		//Allow moving
		CallWrapper(CallWrapper&& other)
		{
			m_call.swap(other.m_call);
		}
		CallWrapper& operator=(CallWrapper&& other)
		{
			m_call = std::move(other.m_call);
			return *this;
		}

		template<typename call_type>
		CallWrapper(call_type call, Args&&... args) : m_call{ std::bind(call, std::forward<Args>(args)...) } {}

		~CallWrapper() { if (m_call) m_call(); }

		explicit operator bool() const { return static_cast<bool>(m_call); }

	private:
		std::function<void()> m_call;
	};
}