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

#include "Emitters_internal.h"

using namespace nif;

constexpr float VAR_FRACTION = -0.5f;


class node::Emitter::BirthRateField final : 
	public Field, public AssignableListener<NiInterpolator>
{
public:
	BirthRateField(
		const std::string& name, 
		NodeBase& node, 
		const ni_ptr<NiPSysEmitterCtlr>& ctlr,
		const ni_ptr<NiFloatInterpolator>& iplr)
		:
		Field{ name },
		m_ctlr{ ctlr },
		m_iplr{ iplr }, 
		m_rcvr{ m_ctlr },
		m_ifc{ *this },
		m_sndr{ m_ifc }
	{
		assert(m_ctlr);

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
	class Controllable final : public IControllable
	{
		BirthRateField& m_parent;

	public:
		Controllable(BirthRateField& parent) : m_parent{ parent } {}

		virtual Ref<NiInterpolator>& iplr() override { return m_parent.m_ctlr->interpolator; }

		virtual ni_ptr<NiTimeController> ctlr() override { return ni_ptr<NiTimeController>(); }
		virtual ni_ptr<NiAVObject> object() override { return ni_ptr<NiAVObject>(); }
		virtual std::string propertyType() override { return std::string(); }
		virtual std::string ctlrType() override { return std::string(); }
		virtual Property<std::string>* ctlrID() override { return nullptr; }
		virtual std::string iplrID() override { return std::string(); }
	};
	
	ni_ptr<NiPSysEmitterCtlr> m_ctlr;
	ni_ptr<NiFloatInterpolator> m_iplr;

	FloatCtlrReceiver m_rcvr;
	Controllable m_ifc;
	Sender<IControllable> m_sndr;
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
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(azimuth, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(azimuthVar, u8"±");
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
		auto main = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(elevation, name);
		auto var = item->newChild<gui::DragInput<float, 1, ni_ptr<Property<math::degf>>, Converter>>(elevationVar, u8"±");
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


node::Emitter::Emitter(
	const ni_ptr<NiPSysEmitter>& obj,
	const ni_ptr<nif::NiPSysEmitterCtlr>& ctlr,
	const ni_ptr<nif::NiFloatInterpolator>& iplr,
	const ni_ptr<nif::NiBoolInterpolator>& vis_iplr) :
	Modifier(obj),
	//ctlr goes to BirthRateField
	//iplr goes to BirthRateField
	m_visIplr{ vis_iplr }
{
	assert(obj);

	m_device.addController(ctlr);
	obj->colour.addListener(*this);//add colour requirement dynamically
	onSet(obj->colour.get());

	if (!ctlr->visIplr.assigned())
		ctlr->visIplr.assign(m_visIplr);

	setColour(COL_TITLE, TitleCol_Modifier);
	setColour(COL_TITLE_ACTIVE, TitleCol_ModifierActive);
	setClosable(true);

	newChild<gui::Separator>();

	m_birthRateField = newField<BirthRateField>(BIRTH_RATE, *this, ctlr, iplr);
	m_lifeSpanField = newField<LifeSpanField>(LIFE_SPAN, *this, 
		make_ni_ptr(obj, &NiPSysEmitter::lifeSpan),
		make_ni_ptr(obj, &NiPSysEmitter::lifeSpanVar));

	newChild<gui::Separator>();

	m_sizeField = newField<SizeField>(SIZE, *this,
		make_ni_ptr(obj, &NiPSysEmitter::size),
		make_ni_ptr(obj, &NiPSysEmitter::sizeVar));

	m_colField = newField<ColourField>(COLOUR, *this,
		make_ni_ptr(obj, &NiPSysEmitter::colour));

	newChild<gui::Separator>();

	newChild<gui::Text>("Velocity");
	m_speedField = newField<SpeedField>(SPEED, *this,
		make_ni_ptr(obj, &NiPSysEmitter::speed),
		make_ni_ptr(obj, &NiPSysEmitter::speedVar));

	m_azimField = newField<AzimuthField>(AZIMUTH, *this,
		make_ni_ptr(obj, &NiPSysEmitter::azimuth),
		make_ni_ptr(obj, &NiPSysEmitter::azimuthVar));

	m_elevField = newField<ElevationField>(ELEVATION, *this,
		make_ni_ptr(obj, &NiPSysEmitter::elevation),
		make_ni_ptr(obj, &NiPSysEmitter::elevationVar));
}

node::Emitter::~Emitter()
{
}


void node::Emitter::onSet(const nif::ColRGBA& col)
{
	if (m_colActive != (col != nif::COL_WHITE)) {
		if (m_colActive)
			m_device.removeRequirement(ModRequirement::COLOUR);
		else
			m_device.addRequirement(ModRequirement::COLOUR);
		m_colActive = !m_colActive;
	}
}


node::VolumeEmitter::VolumeEmitter(
	const ni_ptr<NiPSysVolumeEmitter>& obj,
	const ni_ptr<nif::NiPSysEmitterCtlr>& ctlr,
	const ni_ptr<nif::NiFloatInterpolator>& iplr,
	const ni_ptr<nif::NiBoolInterpolator>& vis_iplr) :
	Emitter(obj, ctlr, iplr, vis_iplr)
{
	newChild<gui::Separator>();
	m_emitterObjField = newField<EmitterObjectField>(EMITTER_OBJECT, *this,
		make_ni_ptr(obj, &NiPSysVolumeEmitter::emitterObject));
}

node::VolumeEmitter::EmitterObjectField::EmitterObjectField(
	const std::string& name, NodeBase& node, ni_ptr<Ptr<NiNode>>&& emitterObject) :
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


node::BoxEmitter::BoxEmitter(
	const ni_ptr<NiPSysBoxEmitter>& obj,
	const ni_ptr<NiPSysEmitterCtlr>& ctlr,
	const ni_ptr<NiFloatInterpolator>& iplr,
	const ni_ptr<NiBoolInterpolator>& vis_iplr) :
	VolumeEmitter(obj, ctlr, iplr, vis_iplr)
{
	setTitle("Box emitter");
	setSize({ WIDTH, HEIGHT });

	m_widthField = newField<EmitterMetricField>(BOX_WIDTH, *this,
		make_ni_ptr(obj, &NiPSysBoxEmitter::width));
	m_heightField = newField<EmitterMetricField>(BOX_HEIGHT, *this,
		make_ni_ptr(obj, &NiPSysBoxEmitter::height));
	m_depthField = newField<EmitterMetricField>(BOX_DEPTH, *this,
		make_ni_ptr(obj, &NiPSysBoxEmitter::depth));

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


node::CylinderEmitter::CylinderEmitter(
	const ni_ptr<NiPSysCylinderEmitter>& obj,
	const ni_ptr<NiPSysEmitterCtlr>& ctlr,
	const ni_ptr<NiFloatInterpolator>& iplr,
	const ni_ptr<NiBoolInterpolator>& vis_iplr) :
	VolumeEmitter(obj, ctlr, iplr, vis_iplr)
{
	setTitle("Cylinder emitter");
	setSize({ WIDTH, HEIGHT });

	m_radiusField = newField<EmitterMetricField>(CYL_RADIUS, *this,
		make_ni_ptr(obj, &NiPSysCylinderEmitter::radius));
	m_lengthField = newField<EmitterMetricField>(CYL_LENGTH, *this,
		make_ni_ptr(obj, &NiPSysCylinderEmitter::length));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(EMITTER_OBJECT)->connector->setTranslation({ 0.0f, 288.0f });
}

node::CylinderEmitter::~CylinderEmitter()
{
	disconnect();
}


node::SphereEmitter::SphereEmitter(
	const ni_ptr<NiPSysSphereEmitter>& obj,
	const ni_ptr<NiPSysEmitterCtlr>& ctlr,
	const ni_ptr<NiFloatInterpolator>& iplr,
	const ni_ptr<NiBoolInterpolator>& vis_iplr) :
	VolumeEmitter(obj, ctlr, iplr, vis_iplr)
{
	setTitle("Sphere emitter");
	setSize({ WIDTH, HEIGHT });

	m_radiusField = newField<EmitterMetricField>(SPH_RADIUS, *this,
		make_ni_ptr(obj, &NiPSysSphereEmitter::radius));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
	getField(EMITTER_OBJECT)->connector->setTranslation({ 0.0f, 288.0f });
}

node::SphereEmitter::~SphereEmitter()
{
	disconnect();
}
