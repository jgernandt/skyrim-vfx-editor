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

class node::EffectShader::GeometryField final : public Field
{
public:
	GeometryField(const std::string& name, EffectShader& node) :
		Field(name), m_receiver(node.object())
	{
		connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::MultiConnector>(m_sender, m_receiver));
	}

private:
	AssignableReceiver<nif::BSEffectShaderProperty> m_receiver;
	Sender<void> m_sender;

};
/*class node::EffectShader::GeometryField final : public Field
{
public:
	GeometryField(const std::string& name, EffectShader& node) : 
		Field(name), m_receiver(node.object()), m_sender(m_subtexCount)
	{
		connector = node.addConnector(name, ConnectorType::UP, std::make_unique<gui::MultiConnector>(m_sender, m_receiver));
	}

	IProperty<nif::SubtextureCount>& subtexCount() { return m_subtexCount; }

private:
	LocalProperty<nif::SubtextureCount> m_subtexCount{ { 1, 1 } };
	AssignableReceiver<nif::BSEffectShaderProperty> m_receiver;
	Sender<IProperty<nif::SubtextureCount>> m_sender;

};*/

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

class node::EffectShader::SourceTexField final : public Field
{
public:
	SourceTexField(const std::string& name, EffectShader& node) : Field(name)
	{
		node.newChild<gui::Text>("Texture");
		widget = node.newChild<gui::TextInput<IProperty<std::string>>>(node.object().sourceTex(), "");
	}
};

/*class node::EffectShader::SubtexturesField final : public Field
{
public:
	SubtexturesField(const std::string& name, EffectShader& node) : Field(name)
	{
		Composite* item = node.newChild<gui::Item>();

		item->newChild<gui::FramePadded>(std::make_unique<gui::Text>("Subtextures"));

		std::array<std::string, 2> labels{ "X", "Y" };
		assert(node.m_geomField);
		auto w = item->newChild<DragInputH<nif::SubtextureCount, 2>>(node.m_geomField->subtexCount(), labels);
		w->setSensitivity(0.02f);
		w->setLowerLimit(1);
		w->setUpperLimit(100);//crazy numbers could be problematic, since we don't multithread
		w->setAlwaysClamp();

		widget = w;
	}
};*/

class node::EffectShader::EmissiveColourField final : public Field
{
public:
	EmissiveColourField(const std::string& name, EffectShader& node) : Field(name)
	{
		widget = node.newChild<ColourInput>(node.object().emissiveCol(), "Emissive colour");
	}
};

class node::EffectShader::EmissiveMultipleField final : public Field
{
public:
	EmissiveMultipleField(const std::string& name, EffectShader& node) : Field(name)
	{
		auto w = node.newChild<DragFloat>(node.object().emissiveMult(), "Emissive multiple");
		w->setSensitivity(0.01f);
		w->setLowerLimit(0.0f);
		w->setAlwaysClamp();
		w->setNumberFormat("%.2f");

		widget = w;
	}
};

class node::EffectShader::PaletteTexField final : public Field
{
public:
	PaletteTexField(const std::string& name, EffectShader& node) : Field(name), m_prop(node.object())
	{
		node.newChild<gui::Text>("Palette");
		widget = node.newChild<gui::TextInput<IProperty<std::string>>>(node.object().greyscaleTex(), "");
		std::array<std::string, 2> labels{ "Use RGB", "Use alpha" };
		node.newChild<gui::Checkbox<std::array<bool, 2>, 2, IProperty<std::array<bool, 2>>>>(m_prop, labels);
		
	}

private:
	struct FlagsProperty : IProperty<std::array<bool, 2>>
	{
		FlagsProperty(nif::BSEffectShaderProperty& obj) : m_obj{ obj } {}
		virtual std::array<bool, 2> get() const override
		{
			return { m_obj.shaderFlags1().isSet(nif::ShaderFlag1::PALETTE_COLOUR), 
				m_obj.shaderFlags1().isSet(nif::ShaderFlag1::PALETTE_ALPHA) };
		}
		virtual void set(const std::array<bool, 2>& val) override
		{
			m_obj.shaderFlags1().set(nif::ShaderFlag1::PALETTE_COLOUR, val[0]);
			m_obj.shaderFlags1().set(nif::ShaderFlag1::PALETTE_ALPHA, val[1]);
		}
		virtual void addListener(IPropertyListener<std::array<bool, 2>>& l) override {}
		virtual void removeListener(IPropertyListener<std::array<bool, 2>>& l) override {}

		nif::BSEffectShaderProperty& m_obj;

	} m_prop;
};

node::EffectShader::EffectShader() : EffectShader(std::make_unique<nif::BSEffectShaderProperty>())
{
	object().shaderFlags1().set(nif::ShaderFlag1::ZBUFFER_TEST, true);
	object().shaderFlags2().set(nif::ShaderFlag2::VERTEX_COLOUR, true);
	object().emissiveCol().set(nif::COL_WHITE);
	object().emissiveMult().set(1.0f);
}

node::EffectShader::EffectShader(std::unique_ptr<nif::BSEffectShaderProperty>&& obj) :
	NodeBase(std::move(obj))
{
	setClosable(true);
	setColour(COL_TITLE, TitleCol_Shader);
	setColour(COL_TITLE_ACTIVE, TitleCol_ShaderActive);
	setSize({ WIDTH, HEIGHT });
	setTitle("Effect shader");

	newField<GeometryField>(GEOMETRY, *this);
	//flags can wait
	//newField<ShaderFlagsField1>(SHADER_FLAGS_1, *this);
	//newField<ShaderFlagsField2>(SHADER_FLAGS_2, *this);

	newChild<gui::Separator>();

	newField<EmissiveColourField>(EMISSIVE_COLOUR, *this);
	newField<EmissiveMultipleField>(EMISSIVE_MULTIPLE, *this);

	newChild<gui::Separator>();

	newField<SourceTexField>(SOURCE_TEXTURE, *this);
	//newField<SubtexturesField>(SUBTEXTURES, *this);

	newChild<gui::Separator>();

	newField<PaletteTexField>(PALETTE_TEXTURE, *this);

	//until we have some other way to determine connector position for loading placement
	getField(GEOMETRY)->connector->setTranslation({ 0.0f, 38.0f });
}

nif::BSEffectShaderProperty& node::EffectShader::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::BSEffectShaderProperty*>(getObjects()[0].get());
}

/*IProperty<nif::SubtextureCount>& node::EffectShader::subtexCount()
{
	assert(m_geomField);
	return m_geomField->subtexCount();
}*/
