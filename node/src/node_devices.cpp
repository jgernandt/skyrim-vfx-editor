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
#include "node_devices.h"
#include "Controllers_internal.h"//defaults

using namespace nif;
using namespace node;

node::FloatCtlrReceiver::FloatCtlrReceiver() :
	m_lFlags(nullptr), m_lFrequency(nullptr), m_lPhase(nullptr),
	m_lStartTime(nullptr), m_lStopTime(nullptr)
{
}

node::FloatCtlrReceiver::FloatCtlrReceiver(const ni_ptr<NiTimeController>& ctlr) :
	m_lFlags(make_ni_ptr(ctlr, &NiTimeController::flags)),
	m_lFrequency(make_ni_ptr(ctlr, &NiTimeController::frequency)),
	m_lPhase(make_ni_ptr(ctlr, &NiTimeController::phase)),
	m_lStartTime(make_ni_ptr(ctlr, &NiTimeController::startTime)),
	m_lStopTime(make_ni_ptr(ctlr, &NiTimeController::stopTime))
{
}

void node::FloatCtlrReceiver::onConnect(IController<float>& ifc)
{
	m_ifc = &ifc;

	ifc.flags().addListener(m_lFlags);
	ifc.frequency().addListener(m_lFrequency);
	ifc.phase().addListener(m_lPhase);
	ifc.startTime().addListener(m_lStartTime);
	ifc.stopTime().addListener(m_lStopTime);

	//we need to set the current values
	m_lFlags.onRaise(ifc.flags().raised());
	m_lFrequency.onSet(ifc.frequency().get());
	m_lPhase.onSet(ifc.phase().get());
	m_lStartTime.onSet(ifc.startTime().get());
	m_lStopTime.onSet(ifc.stopTime().get());
}

void node::FloatCtlrReceiver::onDisconnect(IController<float>& ifc)
{
	ifc.flags().removeListener(m_lFlags);
	ifc.frequency().removeListener(m_lFrequency);
	ifc.phase().removeListener(m_lPhase);
	ifc.startTime().removeListener(m_lStartTime);
	ifc.stopTime().removeListener(m_lStopTime);

	//we need to restore defaults
	m_lFlags.onClear(~DEFAULT_CTLR_FLAGS);
	m_lFlags.onRaise(DEFAULT_CTLR_FLAGS);
	m_lFrequency.onSet(DEFAULT_FREQUENCY);
	m_lPhase.onSet(DEFAULT_PHASE);
	m_lStartTime.onSet(DEFAULT_STARTTIME);
	m_lStopTime.onSet(DEFAULT_STOPTIME);

	m_ifc = nullptr;
}

void node::FloatCtlrReceiver::setController(const ni_ptr<NiTimeController>& ctlr)
{
	m_lFlags.setTarget(make_ni_ptr(ctlr, &NiTimeController::flags));
	m_lFrequency.setTarget(make_ni_ptr(ctlr, &NiTimeController::frequency));
	m_lPhase.setTarget(make_ni_ptr(ctlr, &NiTimeController::phase));
	m_lStartTime.setTarget(make_ni_ptr(ctlr, &NiTimeController::startTime));
	m_lStopTime.setTarget(make_ni_ptr(ctlr, &NiTimeController::stopTime));

	if (m_ifc) {
		m_lFlags.onRaise(m_ifc->flags().raised());
		m_lFrequency.onSet(m_ifc->frequency().get());
		m_lPhase.onSet(m_ifc->phase().get());
		m_lStartTime.onSet(m_ifc->startTime().get());
		m_lStopTime.onSet(m_ifc->stopTime().get());
	}
}
