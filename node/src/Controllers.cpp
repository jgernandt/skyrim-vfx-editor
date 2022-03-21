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
#include "AnimationManager.h"
#include "Controllers.h"
#include "widget_types.h"
#include "KeyEditor.h"

#include "CompositionActions.h"

#include "style.h"

using namespace nif;

class FloatControllerImpl final : public node::IController<float>
{
public:
	FloatControllerImpl(node::ControllerBase& node) : m_node{ node } {}

	virtual FlagSet<ControllerFlags>& flags() override { return m_node.flags(); }
	virtual Property<float>& frequency() override { return m_node.frequency(); }
	virtual Property<float>& phase() override { return m_node.phase(); }
	virtual Property<float>& startTime() override { return m_node.startTime(); }
	virtual Property<float>& stopTime() override { return m_node.stopTime(); }

private:
	node::ControllerBase& m_node;
};

node::ControllerBase::ControllerBase(const ni_ptr<NiInterpolator>& iplr) :
	//Our controller is a dummy object that will never be exported or assigned to another field,
	//hence we can bypass file. Still, this is a little fishy.
	m_ctlr{ std::make_shared<NiTimeController>() }
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_Anim);
	setColour(COL_TITLE_ACTIVE, TitleCol_AnimActive);
}

class node::FloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, ControllerBase& node, const ni_ptr<NiInterpolator>& iplr) :
		Field(name), m_rcvr(iplr), m_ifc(node), m_sndr(m_ifc)
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
	
	ControllableReceiver m_rcvr;
	FloatControllerImpl m_ifc;
	Sender<IController<float>> m_sndr;
};

node::FloatController::FloatController(const ni_ptr<NiFloatInterpolator>& iplr) :
	ControllerBase{ iplr }, m_iplr{ iplr }
{
	assert(iplr && iplr->data.assigned());

	setTitle("Animation");
	setSize({ WIDTH, HEIGHT });

	m_target = newField<TargetField>(TARGET, *this, iplr);

	newChild<gui::VerticalSpacing>();

	auto button = newChild<gui::Button>("Edit keys", std::bind(&FloatController::openKeyEditor, this));
	button->setSize({ -1.0f, 0.0f });

	//until we have some other way to determine connector position for loading placement
	m_target->connector->setTranslation({ WIDTH, 38.0f });
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

/*
Copies a lot from FloatController::TargetField.
The only addition is that this one registers an animation data block on connecting, and listens to target changes.
*/
class node::NLFloatController::TargetField : public node::Field
{
public:
	TargetField(const std::string& name, NLFloatController& node, const ni_ptr<NiInterpolator>& iplr) :
		Field(name), m_rcvr(node), m_ifc(node), m_sndr(m_ifc)
	{
		connector = node.addConnector(name, ConnectorType::DOWN, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));
	}
private:
	class ControllableReceiver : public Receiver<IControllable>, public AssignableListener<NiAVObject>
	{
	public:
		ControllableReceiver(const NLFloatController& node) : m_node{ node } {}

		virtual void onConnect(IControllable& ifc) override;
		virtual void onDisconnect(IControllable& ifc) override;

		virtual void onAssign(NiAVObject* obj) override;

	private:
		void registerBlock();
		void unregisterBlock();

	private:
		const NLFloatController& m_node;
		IControllable* m_connected{ nullptr };
		AnimationManager::Block* m_registeredBlock{ nullptr };
	};

	ControllableReceiver m_rcvr;
	FloatControllerImpl m_ifc;
	Sender<IController<float>> m_sndr;
};

node::NLFloatController::NLFloatController(const ni_ptr<NiBlendFloatInterpolator>& obj) :
	ControllerBase{ obj }, m_iplr{ obj }
{
	setTitle("Nonlinear anim.");
	setSize({ WIDTH, HEIGHT });

	flags().raise(CTLR_MNGR_CTRLD);

	m_target = newField<TargetField>(TARGET, *this, obj);

	//until we have some other way to determine connector position for loading placement
	m_target->connector->setTranslation({ WIDTH, 38.0f });
}

node::NLFloatController::~NLFloatController()
{
	disconnect();
}

void node::NLFloatController::setAnimationManager(const std::shared_ptr<AnimationManager>& animMngr)
{
	m_animationManager = animMngr;
}

void node::NLFloatController::TargetField::ControllableReceiver::onConnect(IControllable& ifc)
{
	//assign our interpolator
	//register the animation
	//listen to changes in controller target

	assert(!m_connected);

	m_connected = &ifc;

	ifc.iplr().assign(m_node.m_iplr);
	registerBlock();
	ifc.node().addListener(*this);
}

void node::NLFloatController::TargetField::ControllableReceiver::onDisconnect(IControllable& ifc)
{
	assert(m_connected);

	ifc.node().removeListener(*this);
	unregisterBlock();
	ifc.iplr().assign(nullptr);

	m_connected = nullptr;
}

void node::NLFloatController::TargetField::ControllableReceiver::onAssign(NiAVObject* obj)
{
	//relay the change to the manager
	//if obj is null, unregister the block (but keep listening)

	assert(m_connected);

	if (m_registeredBlock) {
		if (obj) {
			//switch target
			m_registeredBlock->target.assign(m_connected->node().assigned());
		}
		else {
			unregisterBlock();
		}
	}
	else if (obj) {
		registerBlock();
	}
}

void node::NLFloatController::TargetField::ControllableReceiver::registerBlock()
{
	assert(m_connected && !m_registeredBlock);

	if (auto&& target = m_connected->node().assigned()) {
		if (auto animMngr = m_node.m_animationManager.lock()) {
			AnimationManager::BlockInfo info;
			info.ctlr = m_connected->ctlr();
			info.ctlrIDProperty = m_connected->ctlrIDProperty();
			info.target = m_connected->node().assigned();
			info.propertyType = m_connected->propertyType();
			info.ctlrType = m_connected->ctlrType();
			info.ctlrID = m_connected->ctlrID();
			info.iplrID = m_connected->iplrID();

			m_registeredBlock = animMngr->registerBlock(info);
		}
	}
}

void node::NLFloatController::TargetField::ControllableReceiver::unregisterBlock()
{
	if (m_registeredBlock) {
		if (auto animMngr = m_node.m_animationManager.lock()) {
			animMngr->unregisterBlock(m_registeredBlock);
			m_registeredBlock = nullptr;
		}
	}
}
