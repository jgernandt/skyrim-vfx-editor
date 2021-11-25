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
#include "Shaders.h"
#include "style.h"
#include "widget_types.h"

using namespace node;

class GeometryField final : public Field
{
	AssignableReceiver<BSShaderProperty> m_receiver;//Should be BSEffecShaderProperty, but we need to fix device inheritance
	Sender<void> m_sender;

public:
	GeometryField(const std::string& name, NodeBase& node, const ni_ptr<BSShaderProperty>& shader) :
		Field(name), m_receiver(shader)
	{
		connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::MultiConnector>(m_sender, m_receiver));
	}
};
/*
class node::EffectShader::ShaderFlagsField1 final : public Field
{
public:
	ShaderFlagsField1(const std::string& name, EffectShader& node) : Field(name)
	{
		using widget_type = gui::FlagSelector<nif::FlagSet<nif::ShaderFlag1>>;
		widget = node.newChild<widget_type>(
			node.object().shaderFlags1(), "Shader flags 1", widget_type::ItemList{
				{ nif::ShaderFlag1::PALETTE_COLOUR, "Palette colour" },
				{ nif::ShaderFlag1::PALETTE_ALPHA, "Palette alpha" },
				{ nif::ShaderFlag1::ZBUFFER_TEST, "Depth test" } });
	}
};

class node::EffectShader::ShaderFlagsField2 final : public Field
{
public:
	ShaderFlagsField2(const std::string& name, EffectShader& node) : Field(name)
	{
		using widget_type = gui::FlagSelector<nif::FlagSet<nif::ShaderFlag2>>;
		widget = node.newChild<widget_type>(
			node.object().shaderFlags2(), "Shader flags 2", widget_type::ItemList{
				{ nif::ShaderFlag2::DOUBLE_SIDED, "Double sided" },
				{ nif::ShaderFlag2::VERTEX_COLOUR, "Vertex colour" } });
	}
};
*/
class SourceTexField final : public Field
{
public:
	SourceTexField(const std::string& name, NodeBase& node, ni_ptr<Property<std::string>>&& tex) : 
		Field(name)
	{
		node.newChild<gui::Text>("Texture");
		widget = node.newChild<StringInput>(tex, "");
	}
};

class EmissiveColourField final : public Field
{
public:
	EmissiveColourField(const std::string& name, NodeBase& node, ni_ptr<Property<ColRGBA>>&& col) : 
		Field(name)
	{
		widget = node.newChild<ColourInput>(col, "Emissive colour");
	}
};

class EmissiveMultipleField final : public Field
{
public:
	EmissiveMultipleField(const std::string& name, NodeBase& node, ni_ptr<Property<float>>&& mult) : 
		Field(name)
	{
		auto w = node.newChild<DragFloat>(mult, "Emissive multiple");
		w->setSensitivity(0.01f);
		w->setLowerLimit(0.0f);
		w->setAlwaysClamp();
		w->setNumberFormat("%.2f");

		widget = w;
	}
};

class PaletteTexField final : public Field
{
	const ni_ptr<FlagSet<ShaderFlags>> m_flags;

public:
	PaletteTexField(const std::string& name, NodeBase& node, 
		ni_ptr<Property<std::string>>&& tex, ni_ptr<FlagSet<ShaderFlags>>&& flags) 
		: Field(name), m_flags{ std::move(flags) }
	{
		node.newChild<gui::Text>("Palette");
		widget = node.newChild<StringInput>(tex, "");

		//Using raw pointer as property here, even though the widget will survive us.
		//They should not access us during destruction, though.
		std::array<std::string, 2> labels{ "Use RGB", "Use alpha" };
		node.newChild<gui::Checkbox<std::array<bool, 2>, 2, PaletteTexField*>>(this, labels);
	}

	std::array<bool, 2> getFlags() const 
	{
		ShaderFlags flags = m_flags->raised();
		return { (flags & SF1_PALETTE_COLOUR) != 0, (flags & SF1_PALETTE_ALPHA) != 0 };
	}
	void setFlags(std::array<bool, 2> flags) 
	{
		m_flags->clear((!flags[0] ? SF1_PALETTE_COLOUR : 0) | (!flags[1] ? SF1_PALETTE_ALPHA : 0));
		m_flags->raise((flags[0] ? SF1_PALETTE_COLOUR : 0) | (flags[1] ? SF1_PALETTE_ALPHA : 0));
	}
};

template<>
struct util::property_traits<PaletteTexField*>
{
	using property_type = PaletteTexField*;
	using value_type = std::array<bool, 2>;
	using get_type = std::array<bool, 2>;

	static std::array<bool, 2> get(PaletteTexField* p) { return p->getFlags(); }
	static void set(PaletteTexField* p, std::array<bool, 2> data) { p->setFlags(data); }
};


node::EffectShader::EffectShader(const ni_ptr<nif::BSEffectShaderProperty>& obj)
{
	assert(obj);

	setClosable(true);
	setColour(COL_TITLE, TitleCol_Shader);
	setColour(COL_TITLE_ACTIVE, TitleCol_ShaderActive);
	setSize({ WIDTH, HEIGHT });
	setTitle("Effect shader");

	m_targetField = newField<GeometryField>(GEOMETRY, *this, obj);
	//flags can wait
	//newField<ShaderFlagsField1>(SHADER_FLAGS_1, *this);
	//newField<ShaderFlagsField2>(SHADER_FLAGS_2, *this);

	newChild<gui::Separator>();

	m_colField = newField<EmissiveColourField>(EMISSIVE_COLOUR, *this, 
		make_ni_ptr(obj, &BSEffectShaderProperty::emissiveCol));

	m_multField = newField<EmissiveMultipleField>(EMISSIVE_MULTIPLE, *this, 
		make_ni_ptr(obj, &BSEffectShaderProperty::emissiveMult));

	newChild<gui::Separator>();

	m_texField = newField<SourceTexField>(SOURCE_TEXTURE, *this, 
		make_ni_ptr(obj, &BSEffectShaderProperty::sourceTex));

	newChild<gui::Separator>();

	m_paletteField = newField<PaletteTexField>(PALETTE_TEXTURE, *this, 
		make_ni_ptr(obj, &BSEffectShaderProperty::greyscaleTex), make_ni_ptr(obj, &BSEffectShaderProperty::shaderFlags1));

	//until we have some other way to determine connector position for loading placement
	getField(GEOMETRY)->connector->setTranslation({ 0.0f, 38.0f });
}

node::EffectShader::~EffectShader()
{
	disconnect();
}
