//Copyright 2022 Jonas Gernandt
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

#include "node_concepts.h"
#include "widgets.h"

namespace node
{
	using namespace nif;

	//Essentially a FloatKeyEditor that has multiple AnimationCurves, 
	// as well as a panel that lists the components of the action.
	//We'll make it a Popup for now, but I think this would be nicer as a full or split screen panel
	class ActionEditor :
		public gui::Popup,
		public gui::MouseHandler,
		public gui::KeyListener
	{
	public:
		//This panel should display the components of the Action and allow toggling visibility
		class BlockPanel : public gui::Subwindow
		{
		public:
			BlockPanel(const ni_ptr<NiControllerSequence>& action, gui::Plot* plot);
		};

		//This panel should have controls for Action-wide properties: cycle type, frequency, time limits
		class PropertyPanel : public gui::Subwindow
		{
		public:
			PropertyPanel(
				const ni_ptr<Property<CycleType>>& flags,
				const ni_ptr<Property<float>>& freq, 
				const ni_ptr<Property<float>>& tStart, 
				const ni_ptr<Property<float>>& tStop);
		};

		//We also have an Active component panel, but that is provided by said component itself

	public:
		ActionEditor(const ni_ptr<NiControllerSequence>& action);
	};
}
