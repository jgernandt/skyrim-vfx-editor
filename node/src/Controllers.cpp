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

#include "pch.h"
#include "Controllers.h"
#include "widget_types.h"
#include "KeyEditor.h"

#include "CompositionActions.h"

constexpr gui::ColRGBA TitleCol_Anim = { 0.8f, 0.8f, 0.8f, 1.0f };
constexpr gui::ColRGBA TitleCol_AnimActive = { 0.8f, 0.8f, 0.8f, 1.0f };

using namespace nif;

template<typename T, T Width, T Offset>
struct BitSetWrapper
{
	constexpr static T MASK = ~(~T(0) << Width) << Offset;
	ni_ptr<FlagSet<T>> ptr;
};

template<typename T, T Width, T Offset>
struct util::property_traits<BitSetWrapper<T, Width, Offset>>
{
	using property_type = BitSetWrapper<T, Width, Offset>;
	using value_type = T;
	using get_type = T;

	static T get(const BitSetWrapper<T, Width, Offset>& p)
	{
		return (p.ptr->raised() & p.MASK) >> Offset;
	}
	static void set(BitSetWrapper<T, Width, Offset>& p, T val)
	{
		p.ptr->clear(~(val << Offset) & p.MASK);
		p.ptr->raise((val << Offset) & p.MASK);
	}
};

class node::FloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, FloatController& node) :
		Field(name), m_ifc(node), m_rcvr(node.m_iplr), m_sndr(m_ifc)
	{
		//This is technically an upwards connector, but feels somehow better to have it downwards.
		//Go for consistency or intuitivity?
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}
private:
	class Controller final : public IController<float>
	{
	public:
		Controller(FloatController& node) : m_node{ node } {}

		virtual FlagSet<ControllerFlags>& flags() override { return m_node.flags(); }
		virtual Property<float>& frequency() override { return m_node.frequency(); }
		virtual Property<float>& phase() override { return m_node.phase(); }
		virtual Property<float>& startTime() override { return m_node.startTime(); }
		virtual Property<float>& stopTime() override { return m_node.stopTime(); }

	private:
		FloatController& m_node;
	};
	Controller m_ifc;
	AssignableReceiver<NiInterpolator> m_rcvr;
	Sender<IController<float>> m_sndr;
};

node::FloatController::FloatController(
	ni_ptr<NiFloatInterpolator>&& iplr,
	ni_ptr<NiFloatData>&& data) :
	m_iplr{ std::move(iplr) },
	m_data{ std::move(data) },
	//Our controller is a dummy object that will never be exported or assigned to another field,
	//hence we can bypass file. Still, this is a little fishy.
	m_ctlr{ std::make_shared<NiTimeController>() }
{
	assert(iplr && data);
	iplr->data.assign(data);

	setClosable(true);
	setTitle("Float controller");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	m_target = newField<TargetField>(TARGET, *this);

	using selector_type = gui::Selector<ControllerFlags, BitSetWrapper<ControllerFlags, 2, 1>>;
	newChild<selector_type>(
		BitSetWrapper<ControllerFlags, 2, 1>{ make_ni_ptr(m_ctlr, &NiTimeController::flags) }, 
		std::string(),
		selector_type::ItemList{ { 0, "Repeat" }, { 1, "Reverse" }, { 2, "Clamp" } });

	auto fr = newChild<DragFloat>(make_ni_ptr(m_ctlr, &NiTimeController::frequency), "Frequency");
	fr->setSensitivity(0.01f);
	fr->setLowerLimit(0.0f);
	fr->setAlwaysClamp(true);
	fr->setNumberFormat("%.2f");

	auto ph = newChild<DragFloat>(make_ni_ptr(m_ctlr, &NiTimeController::phase), "Phase");
	ph->setSensitivity(0.01f);
	ph->setNumberFormat("%.2f");

	newChild<gui::Button>("Keys", std::bind(&FloatController::openKeyEditor, this));

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ WIDTH, 38.0f });
}

node::FloatController::~FloatController()
{
	disconnect();
}

void node::FloatController::openKeyEditor()
{
	auto c = std::make_unique<FloatKeyEditor>(
		make_ni_ptr(m_data, &NiFloatData::keyType),
		make_ni_ptr(m_data, &NiFloatData::keys),
		make_ni_ptr(m_ctlr, &NiTimeController::startTime),
		make_ni_ptr(m_ctlr, &NiTimeController::stopTime));
	c->open();
	asyncInvoke<gui::AddChild>(std::move(c), this, false);
}
