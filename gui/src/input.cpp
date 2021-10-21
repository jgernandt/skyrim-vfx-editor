#include "pch.h"
#include "input.h"

int gui::guiToImGuiButton(MouseButton button)
{
	switch (button) {
	case MouseButton::LEFT:
		return ImGuiMouseButton_Left;
	case MouseButton::MIDDLE:
		return ImGuiMouseButton_Middle;
	case MouseButton::RIGHT:
		return ImGuiMouseButton_Right;
	default:
		return -1;
	}
}
