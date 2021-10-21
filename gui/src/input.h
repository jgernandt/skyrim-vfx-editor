#pragma once

namespace gui
{
	enum class MouseButton
	{
		NONE,
		LEFT,
		MIDDLE,
		RIGHT,
		EXTRA1,
		EXTRA2,
	};

	int guiToImGuiButton(gui::MouseButton button);
}