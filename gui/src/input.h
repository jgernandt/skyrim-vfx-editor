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
#include "Observable.h"
#include "gui_types.h"

namespace gui
{
	class IComponent;

	using key_t = int;
	constexpr key_t KEY_SHIFT = 0x10;
	constexpr key_t KEY_CTRL = 0x11;
	constexpr key_t KEY_ALT = 0x12;

	constexpr key_t KEY_DEL = 0x2E;

	class Keyboard
	{
	public:
		static bool isDown(key_t key);

		static IComponent* getCapture() { return s_capturing; }
		static void setCapture(IComponent* c) { s_capturing = c; }

	private:
		static IComponent* s_capturing;
	};

	using KeyListener = IListener<Keyboard>;

	class Mouse
	{
	public:
		enum class Button
		{
			NONE,
			LEFT,
			MIDDLE,
			RIGHT,
			EXTRA1,
			EXTRA2,
		};

	public:
		static bool isButtonDown(Button btn) { return false; }
		static Floats<2> getPosition();
		static void setPosition(const Floats<2>& pos) {}
		static float getWheelDelta() { return 0.0f; }

		static IComponent* getCapture() { return s_capturing; }
		static void setCapture(IComponent* c) { s_capturing = c; }

	private:
		static IComponent* s_capturing;
	};

	class MouseHandler
	{
	public:
		virtual ~MouseHandler() = default;

		virtual bool onMouseDown(Mouse::Button) { return false; }
		virtual bool onMouseUp(Mouse::Button) { return false; }
		virtual bool onMouseWheel(float) { return false; }

		virtual void onMouseMove(const Floats<2>& pos) {}
		virtual void onMouseEnter() {}
		virtual void onMouseLeave() {}

	};


	int guiToImGuiButton(Mouse::Button button);
}

template<>
struct Event<gui::Keyboard>
{
	enum {
		DOWN,
		UP,

	} type{ DOWN };

	gui::key_t key{ 0 };
};

template<>
class IListener<gui::Keyboard>
{
public:
	virtual ~IListener() = default;

	void receive(const ::Event<gui::Keyboard>& e, ::Observable<gui::Keyboard>&)
	{
		switch (e.type) {
		case ::Event<gui::Keyboard>::DOWN:
			onKeyDown(e.key);
			break;
		case ::Event<gui::Keyboard>::UP:
			onKeyUp(e.key);
			break;
		}
	}

	virtual void onKeyDown(gui::key_t key) {}
	virtual void onKeyUp(gui::key_t key) {}
};
