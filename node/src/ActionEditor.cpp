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

#include "pch.h"
#include "ActionEditor.h"

#include "widget_types.h"

node::ActionEditor::ActionEditor(const ni_ptr<NiControllerSequence>& action)
{
	//Set up the layout
	setSize({ 942.0f, 480.0f });

	auto plotPanel = newChild<gui::Subwindow>();
	plotPanel->setSize({ 640.0f, 464.0f });
	plotPanel->setTranslation({ 150.0f, 16.0f });
	auto plot = plotPanel->newChild<gui::Plot>();

	auto blocksPanel = newChild<BlockPanel>(action, plot);
	blocksPanel->setSize({ 142.0f, 464.0f });
	blocksPanel->setTranslation({ 8.0f, 16.0f });

	auto sidePanel = newChild<PropertyPanel>(
		make_ni_ptr(action, &NiControllerSequence::cycleType),
		make_ni_ptr(action, &NiControllerSequence::frequency),
		make_ni_ptr(action, &NiControllerSequence::startTime),
		make_ni_ptr(action, &NiControllerSequence::stopTime));
	sidePanel->setSize({ 142.0f, 200.0f });
	sidePanel->setTranslation({ 790.0f, 16.0f });

	auto activePanel = newChild<gui::Subwindow>();
	activePanel->setSize({ 142.0f, 200.0f });
	activePanel->setTranslation({ 790.0f, 220.0f });
}

node::ActionEditor::BlockPanel::BlockPanel(const ni_ptr<NiControllerSequence>& action, gui::Plot* plot)
{
	//Display a list of blocks and a visibility toggle for each one
	//Add/remove an AnimationCurve to plot for each visible block

	//Placeholder
	for (auto&& block : action->blocks) {
		std::string displayName = block.nodeName.get() + ':' + block.ctlrType.get();
		newChild<gui::Text>(displayName);
	}
}

node::ActionEditor::PropertyPanel::PropertyPanel(
	const ni_ptr<Property<CycleType>>& flags, 
	const ni_ptr<Property<float>>& freq, 
	const ni_ptr<Property<float>>& tStart, 
	const ni_ptr<Property<float>>& tStop)
{
	using selector_type = gui::Selector<CycleType, ni_ptr<Property<CycleType>>>;
	newChild<selector_type>(
		flags,
		std::string(),
		selector_type::ItemList{ { CycleType::REPEAT, "Repeat" }, { CycleType::REVERSE, "Reverse" }, { CycleType::CLAMP, "Clamp" } });

	auto fr = newChild<DragFloat>(freq, "Frequency");
	fr->setSensitivity(0.1f);
	fr->setLowerLimit(0.01f);
	fr->setUpperLimit(100.0f);
	fr->setLogarithmic();
	fr->setNumberFormat("%.2f");

	auto st = newChild<DragFloat>(tStart, "Start time");
	st->setSensitivity(0.01f);
	st->setNumberFormat("%.2f");

	auto sp = newChild<DragFloat>(tStop, "Stop time");
	sp->setSensitivity(0.01f);
	sp->setNumberFormat("%.2f");
}
