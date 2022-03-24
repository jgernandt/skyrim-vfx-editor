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
#include "AnimationCurve.h"
#include "widget_types.h"

node::ActionEditor::ActionEditor(File& file, const ni_ptr<NiControllerSequence>& action)
{
	//Set up the layout
	setSize({ 942.0f, 480.0f });

	auto plotPanel = newChild<gui::Subwindow>();
	plotPanel->setSize({ 640.0f, 464.0f });
	plotPanel->setTranslation({ 150.0f, 16.0f });
	auto plot = plotPanel->newChild<gui::Plot>();

	auto blocksPanel = newChild<BlockPanel>(file, action, plot);
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

node::ActionEditor::BlockPanel::BlockPanel(File& file, const ni_ptr<NiControllerSequence>& action, gui::Plot* plot) :
	m_file{ file }, m_action{ action }
{
	assert(m_action && plot);

	m_animationCurves.reserve(m_action->blocks.size());

	AnimationCurveFactory acf(
		m_file,
		make_ni_ptr(m_action, &NiControllerSequence::cycleType),
		make_ni_ptr(m_action, &NiControllerSequence::startTime),
		make_ni_ptr(m_action, &NiControllerSequence::stopTime));

	//Add a component that carries clip-space transforms
	auto clipTransform = std::make_unique<ClipTransformer>(nullptr, make_ni_ptr(m_action, &NiControllerSequence::frequency));
	m_curveRoot = clipTransform.get();
	plot->getPlotArea().getAxes().addChild(std::move(clipTransform));

	for (auto&& block : m_action->blocks) {
		//Placeholder. We'll want a more user-friendly display name later.
		std::string displayName = block.nodeName.get() + ':' + block.ctlrType.get();
		newChild<gui::Text>(displayName);

		//Add an AnimationCurve. Will have to be delegated, since we don't know the type of interpolator.
		//The exact type of the AnimationCurve is also unknown.
		std::unique_ptr<gui::IComponent> curve;
		if (auto&& iplr = block.interpolator.assigned()) {
			assert(!acf.animationCurve);
			iplr->receive(acf);
			curve = std::move(acf.animationCurve);
		}
		m_animationCurves.push_back(curve.get());//may be null
		if (curve) {
			//This is a supported type. We should add it to the plot and allow its visibility to be toggled.

			m_curveRoot->addChild(std::move(curve));

			//Add a visibility toggle widget. How this could work:
			//The widget property (define a custom one) has a pointer or index to its AnimationCurve and to us.
			//On toggle, call a function on us with the curve as argument. This function adds/removes it from the plot.
		}
		else {
			//This type of interpolator is not supported. We should still list it, but not allow any interaction.
		}
	}

	//Add clip limit indicators
	m_curveRoot->addChild(
		std::make_unique<ClipLimits>(
			make_ni_ptr(m_action, &NiControllerSequence::startTime),
			make_ni_ptr(m_action, &NiControllerSequence::stopTime)));

	//Listen to action->blocks. On insert/erase, add/remove an AnimationCurve and item to the list.
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

node::AnimationCurveFactory::AnimationCurveFactory(
	File& file,
	const ni_ptr<Property<CycleType>>& cycleType, 
	const ni_ptr<Property<float>>& tStart, 
	const ni_ptr<Property<float>>& tStop) :
	m_file{ file }, m_cycleType{ cycleType }, m_tStart{ tStart }, m_tStop{ tStop }
{
}

template<>
void node::AnimationCurveFactory::invoke(NiFloatInterpolator& iplr)
{
	//If we're missing data, add a single key at the value of the interpolator
	if (!iplr.data.assigned()) {
		auto data = m_file.create<NiFloatData>();
		data->keyType.set(KEY_CONSTANT);
		data->keys.push_back();
		data->keys.back().time.set(m_tStart ? m_tStart->get() : 0.0f);
		data->keys.back().value.set(iplr.value.get());

		iplr.data.assign(data);
		iplr.value.set(0.0f);
	}

	animationCurve = std::make_unique<AnimationCurve>(iplr.data.assigned(), m_cycleType, m_tStart, m_tStop);
}
