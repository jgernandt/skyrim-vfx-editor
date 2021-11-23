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
#include "Emitters.h"
#include "style.h"
#include "widget_types.h"

using namespace nif;

constexpr float VAR_FRACTION = -0.5f;
constexpr ControllerFlags DEFAULT_FLAGS = 72;
constexpr float DEFAULT_FREQUENCY = 1.0f;
constexpr float DEFAULT_PHASE = 0.0f;
constexpr float DEFAULT_STARTTIME = 0.0f;
constexpr float DEFAULT_STOPTIME = 1.0f;

constexpr ColRGBA DEFAULT_VCOLOUR = COL_WHITE;
constexpr math::degf DEFAULT_ELEVATION = math::degf{ 0.0f };


class node::Emitter::BirthRateField final : 
	public Field, public AssignableListener<NiInterpolator>
{
public:
	BirthRateField(
		const std::string& name, 
		NodeBase& node, 
		ni_ptr<NiPSysEmitterCtlr>&& ctlr,
		ni_ptr<NiFloatInterpolator>&& iplr) 
		:
		Field(name), 
		m_ctlr{ std::move(ctlr) },
		m_iplr{ std::move(iplr) }, 
		m_rcvr(m_ctlr),
		m_sndr(m_ctlr->interpolator)//old structure, should use shared_ptr
	{
		assert(m_ctlr);//too late!

		if (!m_ctlr->interpolator.assigned())
			m_ctlr->interpolator.assign(m_iplr);

		connector = node.addConnector(std::string(), ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sndr, m_rcvr));

		//Switch widget off when connected
		auto sw = node.newChild<gui::Switch>(std::bind(&gui::Connector::isConnected, connector));

		auto br = sw->newChild<DragFloat>(make_ni_ptr(m_iplr, &NiFloatInterpolator::value), name);
		br->setSensitivity(1.0f);
		br->setLogarithmic();
		br->setLowerLimit(0.0f);
		br->setUpperLimit(1000.0f);

		sw->newChild<gui::Label>(name);

		widget = sw;
	}

	virtual void onAssign(NiInterpolator* obj) override
	{
		//Assign our default iplr when null is assigned
		if (!obj)
			m_ctlr->interpolator.assign(m_iplr);
	}

private:
	//This will be universally useful, so we'll move this definition later
	class ControllerReceiver final : public Receiver<IController<float>>
	{
	public:
		ControllerReceiver(const ni_ptr<NiTimeController>& ctlr) :
			m_lFlags(make_ni_ptr(ctlr, &NiTimeController::flags)),
			m_lFrequency( make_ni_ptr(ctlr, &NiTimeController::frequency) ),
			m_lPhase(make_ni_ptr(ctlr, &NiTimeController::phase)),
			m_lStartTime(make_ni_ptr(ctlr, &NiTimeController::startTime)),
			m_lStopTime(make_ni_ptr(ctlr, &NiTimeController::stopTime))
		{}

		virtual void onConnect(IController<float>& ifc) override
		{
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
		virtual void onDisconnect(IController<float>& ifc) override
		{
			ifc.flags().removeListener(m_lFlags);
			ifc.frequency().removeListener(m_lFrequency);
			ifc.phase().removeListener(m_lPhase);
			ifc.startTime().removeListener(m_lStartTime);
			ifc.stopTime().removeListener(m_lStopTime);

			//we need to restore defaults
			m_lFlags.onRaise(DEFAULT_FLAGS);
			m_lFrequency.onSet(DEFAULT_FREQUENCY);
			m_lPhase.onSet(DEFAULT_PHASE);
			m_lStartTime.onSet(DEFAULT_STARTTIME);
			m_lStopTime.onSet(DEFAULT_STOPTIME);
		}

	private:
		FieldSyncer<FlagSet<ControllerFlags>> m_lFlags;
		FieldSyncer<Property<float>> m_lFrequency;
		FieldSyncer<Property<float>> m_lPhase;
		FieldSyncer<Property<float>> m_lStartTime;
		FieldSyncer<Property<float>> m_lStopTime;
	};
	
	ni_ptr<NiPSysEmitterCtlr> m_ctlr;
	ni_ptr<NiFloatInterpolator> m_iplr;

	ControllerReceiver m_rcvr;
	Sender<Assignable<NiInterpolator>> m_sndr;
};

class node::Emitter::LifeSpanField final : public Field
{
public:
	LifeSpanField(
		const std::string& name, 
		NodeBase& node, 
		ni_ptr<Property<float>>&& lifeSpan, 
		ni_ptr<Property<float>>&& lifeSpanVar) 
		: 
		Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(lifeSpan, "Life span");
		auto var = item->newChild<DragFloat>(lifeSpanVar, u8"±");
		main->setSensitivity(0.05f);
		main->setLowerLimit(0.0f);
		main->setAlwaysClamp();
		var->setSensitivity(0.05f);
		var->setLowerLimit(0.0f);
		var->setAlwaysClamp();
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

class node::Emitter::SizeField final : public Field
{
public:
	SizeField(
		const std::string& name,
		NodeBase& node,
		ni_ptr<Property<float>>&& size,
		ni_ptr<Property<float>>&& sizeVar)
		:
		Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(size, name);
		auto var = item->newChild<DragFloat>(sizeVar, u8"±");
		main->setSensitivity(0.1f);
		main->setLowerLimit(0.0f);
		main->setAlwaysClamp();
		var->setSensitivity(0.1f);
		var->setLowerLimit(0.0f);
		var->setAlwaysClamp();
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

class node::Emitter::ColourField final : public Field
{
public:
	ColourField(const std::string& name, NodeBase& node, ni_ptr<Property<ColRGBA>>&& col) : 
		Field(name)
	{
		widget = node.newChild<ColourInput>(col, "Vertex colour");
	}
};

class node::Emitter::SpeedField final : public Field
{
public:
	SpeedField(
		const std::string& name,
		NodeBase& node,
		ni_ptr<Property<float>>&& speed,
		ni_ptr<Property<float>>&& speedVar)
		:
		Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(speed, "Magnitude");
		auto var = item->newChild<DragFloat>(speedVar, u8"±");
		main->setSensitivity(0.1f);
		main->setLowerLimit(0.0f);
		main->setAlwaysClamp();
		var->setSensitivity(0.1f);
		var->setLowerLimit(0.0f);
		var->setAlwaysClamp();
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

class node::Emitter::AzimuthField final : public Field
{
public:
	AzimuthField(
		const std::string& name,
		NodeBase& node,
		ni_ptr<Property<math::degf>>&& azimuth,
		ni_ptr<Property<math::degf>>&& azimuthVar)
		:
		Field(name)
	{
		//Angle around z, counter-clockwise from x
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, NodeConverter>>(azimuth, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, NodeConverter>>(azimuthVar, u8"±");
		main->setSensitivity(0.5f);
		main->setLowerLimit(0.0f);
		main->setUpperLimit(360.0f);
		main->setNumberFormat(u8"%.0f°");
		main->setAlwaysClamp();

		var->setSensitivity(0.5f);
		var->setLowerLimit(0.0f);
		var->setUpperLimit(180.0f);
		var->setAlwaysClamp();
		var->setNumberFormat(u8"%.0f°");
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};

class node::Emitter::ElevationField final : public Field
{
public:
	ElevationField(
		const std::string& name,
		NodeBase& node,
		ni_ptr<Property<math::degf>>&& elevation,
		ni_ptr<Property<math::degf>>&& elevationVar)
		:
		Field(name)
	{
		//Angle around z, counter-clockwise from x
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, NodeConverter>>(elevation, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, NodeConverter>>(elevationVar, u8"±");
		main->setSensitivity(0.5f);
		main->setLowerLimit(-90.0f);
		main->setUpperLimit(90.0f);
		main->setNumberFormat(u8"%.0f°");
		main->setAlwaysClamp();

		var->setSensitivity(0.5f);
		var->setLowerLimit(0.0f);
		var->setUpperLimit(90.0f);
		var->setAlwaysClamp();
		var->setNumberFormat(u8"%.0f°");
		var->setSizeHint({ VAR_FRACTION, -1.0f });

		widget = item;
	}
};


node::Emitter::Emitter(nif::File& file, 
	ni_ptr<nif::NiPSysEmitter>&& obj,
	ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	ni_ptr<nif::NiFloatInterpolator>&& iplr,
	ni_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	Modifier(std::move(obj)),
	//ctlr goes to BirthRateField
	//iplr goes to BirthRateField
	m_visIplr{ std::move(vis_iplr) }
{
	if (ctlr) {
		ctlr = file.create<nif::NiPSysEmitterCtlr>();
		if (ctlr)
			throw std::runtime_error("Failed to create NiPSysEmitterCtlr");

		ctlr->flags.raise(DEFAULT_FLAGS);
		ctlr->frequency.set(DEFAULT_FREQUENCY);
		ctlr->phase.set(DEFAULT_PHASE);
		ctlr->startTime.set(DEFAULT_STARTTIME);
		ctlr->stopTime.set(DEFAULT_STOPTIME);
	}

	m_device.addController(ctlr);
	object().colour.addListener(*this);//add colour requirement dynamically
	onSet(object().colour.get());
	m_device.addRequirement(Requirement::LIFETIME);
	m_device.addRequirement(Requirement::MOVEMENT);

	if (!iplr) {
		iplr = file.create<nif::NiFloatInterpolator>();
		if (!iplr)
			throw std::runtime_error("Failed to create NiFloatInterpolator");
	}

	if (!m_visIplr) {
		m_visIplr = file.create<nif::NiBoolInterpolator>();
		if (!m_visIplr)
			throw std::runtime_error("Failed to create NiBoolInterpolator");
		m_visIplr->value.set(true);
	}
	if (!ctlr->visIplr.assigned())
		ctlr->visIplr.assign(m_visIplr);

	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);
	setClosable(true);

	newChild<gui::Separator>();

	m_birthRateField = newField<BirthRateField>(BIRTH_RATE, *this, std::move(ctlr), std::move(iplr));
	m_lifeSpanField = newField<LifeSpanField>(LIFE_SPAN, *this, 
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::lifeSpan),
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::lifeSpanVar));

	newChild<gui::Separator>();

	m_sizeField = newField<SizeField>(SIZE, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::lifeSpan),
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::lifeSpanVar));

	m_colField = newField<ColourField>(COLOUR, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::colour));

	newChild<gui::Separator>();

	newChild<gui::Text>("Velocity");
	m_speedField = newField<SpeedField>(SPEED, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::speed),
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::speedVar));

	m_azimField = newField<AzimuthField>(AZIMUTH, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::azimuth),
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::azimuthVar));

	m_elevField = newField<ElevationField>(ELEVATION, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::elevation),
		make_ni_ptr(std::static_pointer_cast<NiPSysEmitter>(m_obj), &NiPSysEmitter::elevationVar));
}

node::Emitter::~Emitter()
{
}

nif::NiPSysEmitter& node::Emitter::object()
{
	assert(m_obj);
	return *static_cast<nif::NiPSysEmitter*>(m_obj.get());
}


void node::Emitter::onSet(const nif::ColRGBA& col)
{
	if (m_colActive != (col != nif::COL_WHITE)) {
		if (m_colActive)
			m_device.removeRequirement(Requirement::COLOUR);
		else
			m_device.addRequirement(Requirement::COLOUR);
		m_colActive = !m_colActive;
	}
}


node::VolumeEmitter::VolumeEmitter(nif::File& file,
	ni_ptr<nif::NiPSysVolumeEmitter>&& obj,
	ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	ni_ptr<nif::NiFloatInterpolator>&& iplr,
	ni_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	Emitter(file, std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	newChild<gui::Separator>();
	m_emitterObjField = newField<EmitterObjectField>(EMITTER_OBJECT, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysVolumeEmitter>(m_obj), &NiPSysVolumeEmitter::emitterObject));
}

nif::NiPSysVolumeEmitter& node::VolumeEmitter::object()
{
	assert(m_obj);
	return *static_cast<nif::NiPSysVolumeEmitter*>(m_obj.get());
}

node::VolumeEmitter::EmitterObjectField::EmitterObjectField(
	const std::string& name, NodeBase& node, ni_ptr<Assignable<NiNode>>&& emitterObject) :
	Field(name), 
	m_sdr(*emitterObject)//old format, should use the pointer
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sdr, m_rvr));
}

node::VolumeEmitter::EmitterMetricField::EmitterMetricField(
	const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& prop) :
	Field(name)
{
	auto w = node.newChild<DragFloat>(prop, name);
	w->setSensitivity(0.1f);
	w->setLowerLimit(0.0f);
	w->setAlwaysClamp();
}


node::BoxEmitter::BoxEmitter(nif::File& file) :
	BoxEmitter(file, file.create<nif::NiPSysBoxEmitter>())
{
	object().colour.set(DEFAULT_VCOLOUR);
	object().elevation.set(DEFAULT_ELEVATION);
}

node::BoxEmitter::BoxEmitter(nif::File& file,
	ni_ptr<nif::NiPSysBoxEmitter>&& obj,
	ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	ni_ptr<nif::NiFloatInterpolator>&& iplr,
	ni_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(file, std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Box emitter");
	setSize({ WIDTH, HEIGHT });

	m_widthField = newField<EmitterMetricField>(BOX_WIDTH, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysBoxEmitter>(m_obj), &NiPSysBoxEmitter::width));
	m_heightField = newField<EmitterMetricField>(BOX_HEIGHT, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysBoxEmitter>(m_obj), &NiPSysBoxEmitter::height));
	m_depthField = newField<EmitterMetricField>(BOX_DEPTH, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysBoxEmitter>(m_obj), &NiPSysBoxEmitter::depth));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(BIRTH_RATE)->connector->setTranslation({ 0.0f, 90.0f });
	getField(EMITTER_OBJECT)->connector->setTranslation({ 0.0f, 288.0f });
}

node::BoxEmitter::~BoxEmitter()
{
	disconnect();
}

nif::NiPSysBoxEmitter& node::BoxEmitter::object()
{
	assert(m_obj);
	return *static_cast<nif::NiPSysBoxEmitter*>(m_obj.get());
}


node::CylinderEmitter::CylinderEmitter(nif::File& file) :
	CylinderEmitter(file, file.create<nif::NiPSysCylinderEmitter>())
{
	object().colour.set(DEFAULT_VCOLOUR);
	object().elevation.set(DEFAULT_ELEVATION);
}

node::CylinderEmitter::CylinderEmitter(nif::File& file,
	ni_ptr<nif::NiPSysCylinderEmitter>&& obj,
	ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	ni_ptr<nif::NiFloatInterpolator>&& iplr,
	ni_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(file, std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Cylinder emitter");
	setSize({ WIDTH, HEIGHT });

	m_radiusField = newField<EmitterMetricField>(CYL_RADIUS, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysCylinderEmitter>(m_obj), &NiPSysCylinderEmitter::radius));
	m_lengthField = newField<EmitterMetricField>(CYL_LENGTH, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysCylinderEmitter>(m_obj), &NiPSysCylinderEmitter::length));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(EMITTER_OBJECT)->connector->setTranslation({ 0.0f, 288.0f });
}

node::CylinderEmitter::~CylinderEmitter()
{
	disconnect();
}

nif::NiPSysCylinderEmitter& node::CylinderEmitter::object()
{
	assert(m_obj);
	return *static_cast<nif::NiPSysCylinderEmitter*>(m_obj.get());
}


node::SphereEmitter::SphereEmitter(nif::File& file) :
	SphereEmitter(file, file.create<nif::NiPSysSphereEmitter>())
{
	object().colour.set(DEFAULT_VCOLOUR);
	object().elevation.set(DEFAULT_ELEVATION);
}

node::SphereEmitter::SphereEmitter(nif::File& file,
	ni_ptr<nif::NiPSysSphereEmitter>&& obj,
	ni_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	ni_ptr<nif::NiFloatInterpolator>&& iplr,
	ni_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(file, std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Sphere emitter");
	setSize({ WIDTH, HEIGHT });

	m_radiusField = newField<EmitterMetricField>(SPH_RADIUS, *this,
		make_ni_ptr(std::static_pointer_cast<NiPSysBoxEmitter>(m_obj), &NiPSysBoxEmitter::width));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(EMITTER_OBJECT)->connector->setTranslation({ 0.0f, 288.0f });
}

node::SphereEmitter::~SphereEmitter()
{
	disconnect();
}

nif::NiPSysSphereEmitter& node::SphereEmitter::object()
{
	assert(m_obj);
	return *static_cast<nif::NiPSysSphereEmitter*>(m_obj.get());
}

