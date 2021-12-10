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

constexpr gui::ColRGBA TitleCol_Anim = { 1.0f, 1.0f, 0.7f, 1.0f };
constexpr gui::ColRGBA TitleCol_AnimActive = { 1.0f, 1.0f, 0.7f, 1.0f };

using namespace nif;

class node::FloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, FloatController& node) :
		Field(name), m_rcvr(node.m_iplr), m_ifc(node), m_sndr(m_ifc)
	{
		//This is technically an upwards connector, but feels somehow better to have it downwards.
		//Go for consistency or intuitivity?
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}
private:
	class ControllableReceiver : public Receiver<IControllable>
	{
	public:
		ControllableReceiver(const ni_ptr<NiInterpolator>& obj) : m_obj{ obj } {}

		virtual void onConnect(IControllable& ifc) override { ifc.iplr().assign(m_obj); }
		virtual void onDisconnect(IControllable& ifc) override { ifc.iplr().assign(nullptr); }

	private:
		ni_ptr<NiInterpolator> m_obj;
	};
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

	ControllableReceiver m_rcvr;
	Controller m_ifc;
	Sender<IController<float>> m_sndr;
};

node::FloatController::FloatController(const ni_ptr<NiFloatInterpolator>& iplr) :
	m_iplr{ iplr },
	//Our controller is a dummy object that will never be exported or assigned to another field,
	//hence we can bypass file. Still, this is a little fishy.
	m_ctlr{ std::make_shared<NiTimeController>() }
{
	assert(iplr->data.assigned());

	setClosable(true);
	setTitle("Animation");
	setSize({ WIDTH, HEIGHT });
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);

	m_target = newField<TargetField>(TARGET, *this);

	newChild<gui::VerticalSpacing>();

	auto button = newChild<gui::Button>("Edit keys", std::bind(&FloatController::openKeyEditor, this));
	button->setSize({ -1.0f, 0.0f });

	//until we have some other way to determine connector position for loading placement
	getField(TARGET)->connector->setTranslation({ WIDTH, 38.0f });
}

node::FloatController::~FloatController()
{
	disconnect();
}

void node::FloatController::openKeyEditor()
{
	auto c = std::make_unique<FloatKeyEditor>(m_ctlr, m_iplr->data.assigned());
	c->open();
	asyncInvoke<gui::AddChild>(std::move(c), this, false);
}
