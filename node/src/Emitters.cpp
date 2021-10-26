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

constexpr float VAR_FRACTION = -0.5f;

class node::Emitter::BirthRateField final : public Field
{
public:
	BirthRateField(const std::string& name, Emitter& node, std::unique_ptr<nif::NiFloatInterpolator>&& iplr) :
		Field(name), m_ctlr{ node.m_ctlr }, m_iplr{ std::move(iplr) }
	{
		if (!m_iplr) {
			m_iplr = std::make_unique<nif::NiFloatInterpolator>();
		}
		assert(m_ctlr);
		if (m_ctlr->interpolator().isAssigned(nullptr))
			m_ctlr->interpolator().assign(m_iplr.get());

		auto br = node.newChild<DragFloat>(m_iplr->value(), u8"Birth rate");
		br->setSensitivity(1.0f);
		br->setLogarithmic();
		br->setLowerLimit(0.0f);
		br->setUpperLimit(1000.0f);
		widget = br;
	}

	nif::NiPSysEmitterCtlr* m_ctlr;
	std::unique_ptr<nif::NiFloatInterpolator> m_iplr;
};

class node::Emitter::LifeSpanField final : public Field
{
public:
	LifeSpanField(const std::string& name, Emitter& node) : Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().lifeSpan(), u8"Life span");
		auto var = item->newChild<DragFloat>(node.object().lifeSpanVar(), u8"±");
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
	SizeField(const std::string& name, Emitter& node) : Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().size(), name);
		auto var = item->newChild<DragFloat>(node.object().sizeVar(), u8"±");
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
	ColourField(const std::string& name, Emitter& node) : Field(name)
	{
		widget = node.newChild<ColourInput>(node.object().colour(), "Vertex colour");
	}
};

class node::Emitter::SpeedField final : public Field
{
public:
	SpeedField(const std::string& name, Emitter& node) : Field(name)
	{
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().speed(), "Magnitude");
		auto var = item->newChild<DragFloat>(node.object().speedVar(), u8"±");
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
	AzimuthField(const std::string& name, Emitter& node) : Field(name)
	{
		//Angle around z, counter-clockwise from x
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().azimuth(), name);
		auto var = item->newChild<DragFloat>(node.object().azimuthVar(), u8"±");
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
	ElevationField(const std::string& name, Emitter& node) : Field(name)
	{
		//Angle around z, counter-clockwise from x
		gui::Item* item = node.newChild<gui::Item>();
		auto main = item->newChild<DragFloat>(node.object().elevation(), name);
		auto var = item->newChild<DragFloat>(node.object().elevationVar(), u8"±");
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


node::Emitter::Emitter(std::unique_ptr<nif::NiPSysEmitter>&& obj,
	std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
	std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	Modifier(std::move(obj))
{
	if (!ctlr) {
		ctlr = std::make_unique<nif::NiPSysEmitterCtlr>();
		ctlr->flags().set(72);
		ctlr->frequency().set(1.0f);
		ctlr->phase().set(0.0f);
		ctlr->startTime().set(0.0f);
		ctlr->stopTime().set(1.0f);
	}
	m_ctlr = ctlr.get();
	addController(std::move(ctlr));

	m_modNameLsnr = std::make_unique<ModifierNameListener>(m_ctlr->modifierName());
	object().name().addListener(*m_modNameLsnr);

	if (!vis_iplr) {
		vis_iplr = std::make_unique<nif::NiBoolInterpolator>();
		vis_iplr->value().set(true);
	}
	m_visIplr = std::move(vis_iplr);
	if (m_ctlr->visIplr().isAssigned(nullptr))
		m_ctlr->visIplr().assign(m_visIplr.get());

	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);
	setClosable(true);

	addTargetField(std::make_shared<Emitter::Device>(*this));

	newChild<gui::Separator>();

	newField<BirthRateField>(BIRTH_RATE, *this, std::move(iplr));
	newField<LifeSpanField>(LIFE_SPAN, *this);

	newChild<gui::Separator>();

	newField<SizeField>(SIZE, *this);
	newField<ColourField>(COLOUR, *this);

	newChild<gui::Separator>();

	newChild<gui::Text>("Velocity");
	newField<SpeedField>(SPEED, *this);
	newField<AzimuthField>(AZIMUTH, *this);
	newField<ElevationField>(ELEVATION, *this);
}

node::Emitter::~Emitter()
{
	object().name().removeListener(*m_modNameLsnr);
	if (controller().visIplr().isAssigned(m_visIplr.get()))
		controller().visIplr().assign(nullptr);
}

nif::NiPSysEmitter& node::Emitter::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysEmitter*>(getObjects()[0].get());
}

nif::NiPSysEmitterCtlr& node::Emitter::controller()
{
	assert(m_ctlr);
	return *m_ctlr;
}

/*nif::NiFloatInterpolator& node::Emitter::brIplr()
{
	assert(getObjects().size() > 1 && getObjects()[1]);
	return *static_cast<nif::NiFloatInterpolator*>(getObjects()[1].get());
}

nif::NiBoolInterpolator& node::Emitter::visIplr()
{
	assert(getObjects().size() > 2 && getObjects()[2]);
	return *static_cast<nif::NiBoolInterpolator*>(getObjects()[2].get());
}*/


node::Emitter::Device::Device(Emitter& node) :
	Modifier::Device(node),
	m_col(node.object().colour()),
	m_life(node.object().lifeSpan(), node.object().lifeSpanVar(), Requirement::LIFETIME),
	m_move(node.object().speed(), node.object().speedVar(), Requirement::MOVEMENT)
{
}

void node::Emitter::Device::onConnect(IModifiable& ifc)
{
	m_col.activate(&ifc.requirements());
	m_life.activate(&ifc.requirements());
	m_move.activate(&ifc.requirements());

	//pass on last
	Modifier::Device::onConnect(ifc);
}

void node::Emitter::Device::onDisconnect(IModifiable& ifc)
{
	//pass on first
	Modifier::Device::onDisconnect(ifc);

	m_col.deactivate();
	m_life.deactivate();
	m_move.deactivate();
}

void node::Emitter::Device::ColourActivator::onSet(const nif::ColRGBA& f)
{
	if ((f != nif::COL_WHITE) != m_active) {
		assert(m_target);
		if (m_active)
			m_target->remove(Requirement::COLOUR);
		else
			m_target->add(Requirement::COLOUR);
		m_active = !m_active;
	}
}

void node::Emitter::Device::ColourActivator::activate(ISet<Requirement>* target)
{
	m_target = target;
	m_prop.addListener(*this);
	//we'll get a call here...
}

void node::Emitter::Device::ColourActivator::deactivate()
{
	//...but not here
	if (m_active) {
		assert(m_target);
		m_target->remove(Requirement::COLOUR);
		m_active = false;
	}
	m_prop.removeListener(*this);
	m_target = nullptr;
}

void node::Emitter::Device::LifeMoveActivator::onSet(const float& f)
{
	if ((m_prop.get() != 0.0f || m_var.get() != 0.0f) != m_active) {
		assert(m_target);
		if (m_active)
			m_target->remove(m_req);
		else
			m_target->add(m_req);
		m_active = !m_active;
	}
}

void node::Emitter::Device::LifeMoveActivator::activate(ISet<Requirement>* target)
{
	m_target = target;
	m_prop.addListener(*this);
	m_var.addListener(*this);
	//we'll get a call here...
}

void node::Emitter::Device::LifeMoveActivator::deactivate()
{
	//...but not here
	if (m_active) {
		assert(m_target);
		m_target->remove(m_req);
		m_active = false;
	}
	m_prop.removeListener(*this);
	m_var.removeListener(*this);
	m_target = nullptr;
}


node::VolumeEmitter::VolumeEmitter(std::unique_ptr<nif::NiPSysVolumeEmitter>&& obj,
	std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
	std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	Emitter(std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	newChild<gui::Separator>();
	newField<EmitterObjectField>(EMITTER_OBJECT, *this);
}

nif::NiPSysVolumeEmitter& node::VolumeEmitter::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysVolumeEmitter*>(getObjects()[0].get());
}

node::VolumeEmitter::EmitterObjectField::EmitterObjectField(const std::string& name, VolumeEmitter& node) :
	Field(name), m_sdr(node.object().emitterObject())
{
	connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::SingleConnector>(m_sdr, m_rvr));
}

node::VolumeEmitter::EmitterMetricField::EmitterMetricField(const std::string& name, VolumeEmitter& node, IProperty<float>& prop) :
	Field(name)
{
	auto w = node.newChild<DragFloat>(prop, name);
	w->setSensitivity(0.1f);
	w->setLowerLimit(0.0f);
	w->setAlwaysClamp();
}


node::BoxEmitter::BoxEmitter() :
	BoxEmitter(std::make_unique<nif::NiPSysBoxEmitter>())
{
	object().colour().set(nif::COL_WHITE);
	object().elevation().set(0.0f);
}

node::BoxEmitter::BoxEmitter(std::unique_ptr<nif::NiPSysBoxEmitter>&& obj,
	std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
	std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Box emitter");
	setSize({ WIDTH, HEIGHT });

	newField<EmitterMetricField>(BOX_WIDTH, *this, object().width());
	newField<EmitterMetricField>(BOX_HEIGHT, *this, object().height());
	newField<EmitterMetricField>(BOX_DEPTH, *this, object().depth());
}

nif::NiPSysBoxEmitter& node::BoxEmitter::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysBoxEmitter*>(getObjects()[0].get());
}


node::CylinderEmitter::CylinderEmitter() :
	CylinderEmitter(std::make_unique<nif::NiPSysCylinderEmitter>())
{
	object().colour().set(nif::COL_WHITE);
	object().elevation().set(0.0f);
}

node::CylinderEmitter::CylinderEmitter(std::unique_ptr<nif::NiPSysCylinderEmitter>&& obj,
	std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
	std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Cylinder emitter");
	setSize({ WIDTH, HEIGHT });

	newField<EmitterMetricField>(CYL_RADIUS, *this, object().radius());
	newField<EmitterMetricField>(CYL_LENGTH, *this, object().height());
}

nif::NiPSysCylinderEmitter& node::CylinderEmitter::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysCylinderEmitter*>(getObjects()[0].get());
}


node::SphereEmitter::SphereEmitter() :
	SphereEmitter(std::make_unique<nif::NiPSysSphereEmitter>())
{
	object().colour().set(nif::COL_WHITE);
	object().elevation().set(0.0f);
}

node::SphereEmitter::SphereEmitter(std::unique_ptr<nif::NiPSysSphereEmitter>&& obj,
	std::unique_ptr<nif::NiPSysEmitterCtlr>&& ctlr,
	std::unique_ptr<nif::NiFloatInterpolator>&& iplr,
	std::unique_ptr<nif::NiBoolInterpolator>&& vis_iplr) :
	VolumeEmitter(std::move(obj), std::move(ctlr), std::move(iplr), std::move(vis_iplr))
{
	setTitle("Sphere emitter");
	setSize({ WIDTH, HEIGHT });

	newField<EmitterMetricField>(SPH_RADIUS, *this, object().radius());
}

nif::NiPSysSphereEmitter& node::SphereEmitter::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::NiPSysSphereEmitter*>(getObjects()[0].get());
}

