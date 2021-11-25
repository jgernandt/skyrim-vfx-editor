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
#include "ScaleModifier.h"
#include "widget_types.h"

#include "SplineInterpolant.h"

using namespace nif;
using namespace node;

//We will fill out the scale values with time values and keep the resulting vector of points locally.
//We then add one widget to control the points and one to add/remove points, both of which pass their actions back to us.
//Not the most efficient design, since we're copying a whole new vector (back and forth) when changing one point.
//We'd need some sort of iterator or random access property to make it better. No big deal, though.
class ScaleField final : 
	public Field, public PropertyListener<std::vector<float>>, public gui::MouseHandler
{
	ni_ptr<Property<std::vector<float>>> m_scales;
	std::vector<gui::Floats<2>> m_data;
	gui::PlotArea* m_area{ nullptr };

public:
	ScaleField(const std::string& name, NodeBase& node, ni_ptr<Property<std::vector<float>>>&& scales) : 
		Field(name), m_scales{ std::move(scales) }
	{
		assert(m_scales);

		node.newChild<gui::Separator>();
		node.newChild<gui::Text>("Scale");

		auto plot = node.newChild<gui::Plot>();
		m_area = &plot->getPlotArea();

		plot->getPlotArea().setMouseHandler(this);

		plot->getPlotArea().addCurve(std::make_unique<gui::SimpleCurve>(m_data));
		plot->getPlotArea().getAxes().addChild(std::make_unique<Controls>(m_data, *m_scales));
		std::vector<gui::CustomXLabels::AxisLabel> labels{ { "Birth", 0.0f, 0.0f }, { "Death", 1.0f, 1.0f } };
		plot->setXLabels(std::make_unique<gui::CustomXLabels>(plot->getPlotArea().getAxes(), std::move(labels)));
		//plot->setLimits({ 0.0f, 1.0f, 0.0f, 1.0f });

		//segments +-
		auto item = node.newChild<gui::Item>(std::make_unique<gui::RightAlign>());
		item->newChild<gui::Text>("Segments");
		item->newChild<RemoveButton>(*m_scales);
		item->newChild<AddButton>(*m_scales);
		
		m_scales->addListener(*this);
		onSet(m_scales->get());
	}

	virtual void onSet(const std::vector<float>& v) override
	{
		if (v.size() > 1) {
			float spacing = 1.0f / (v.size() - 1);
			m_data.resize(v.size());
			for (size_t i = 0; i < v.size(); i++)
				m_data[i] = { i * spacing, v[i] };
		}
		else if (v.size() == 1)
			m_data = { { 0.0f, v.front() } };
		else
			m_data.clear();
	}

	virtual bool onMouseWheel(float delta) override
	{
		assert(m_area);
		gui::Floats<2> ylims = m_area->getYLimits();
		ylims[1] = std::max(ylims[1] - delta, 1.0f);
		m_area->setYLimits(ylims);

		return true;
	}

private:
	class EditPoint final : public gui::ICommand
	{
		Property<std::vector<float>>& m_trgt;
		size_t m_index;
		float m_from;
		float m_to;

	public:
		EditPoint(Property<std::vector<float>>& trgt, size_t i, float to, float from) :
			m_trgt{ trgt }, m_index{ i }, m_from{ from }, m_to{ to } {}

		virtual void execute() override 
		{
			if (auto v = m_trgt.get(); m_index < v.size() && v[m_index] != m_to) {
				v[m_index] = m_to;
				m_trgt.set(v);
			}
		}
		virtual void reverse() override 
		{
			if (auto v = m_trgt.get(); m_index < v.size()) {
				v[m_index] = m_from;
				m_trgt.set(v);
			}
		}
		virtual bool reversible() const override { return m_from != m_to; }
	};

	class AddSegment final : public gui::ICommand
	{
		Property<std::vector<float>>& m_trgt;
		std::vector<float> m_old;
		std::vector<float> m_new;

	public:
		AddSegment(Property<std::vector<float>>& trgt) : m_trgt{ trgt } {}

		virtual void execute() override 
		{
			if (m_new.empty()) {
				//this is the first time we're executed
				if (m_old = m_trgt.get(); m_old.size() != std::numeric_limits<decltype(m_old)::size_type>::max()) {//very, very safe
					if (m_old.size() < 2) {
						//could happen on importing
						if (m_old.empty())
							m_new = { 0.0f, 1.0f };
						else
							m_new = { m_old.front(), m_old.front() };
					}
					else if (m_old.size() == 2) {
						m_new = { m_old.front(), 0.5f * (m_old.back() + m_old.front()), m_old.back() };
					}
					else {
						m_new.resize(m_old.size() + 1);

						Eigen::VectorXf vals(m_old.size());
						for (int i = 0; i < vals.size(); i++)
							vals[i] = m_old[i];

						math::SplineInterpolant spline(vals);
						vals = spline.eval(Eigen::VectorXf::LinSpaced(m_new.size(), 0.0f, 1.0f));

						for (int i = 0; i < vals.size(); i++)
							m_new[i] = std::max(vals[i], 0.0f);
					}
				}
				else
					m_new = m_old;
			}
			else {
				//we are being redone
			}
			m_trgt.set(m_new);
		}
		virtual void reverse() override 
		{
			m_trgt.set(m_old);
		}
		virtual bool reversible() const override { return true; }
	};

	class RemoveSegment final : public gui::ICommand
	{
		Property<std::vector<float>>& m_trgt;
		std::vector<float> m_old;
		std::vector<float> m_new;

	public:
		RemoveSegment(Property<std::vector<float>>& trgt) : m_trgt{ trgt } {}

		virtual void execute() override 
		{
			if (m_new.empty()) {
				m_old = m_trgt.get();
				if (m_old.size() < 3)
					m_new = m_old;
				else if (m_old.size() == 3)
					m_new = { m_old.front(), m_old.back() };
				else {
					m_new.resize(m_old.size() - 1);

					Eigen::VectorXf vals(m_old.size());
					for (int i = 0; i < vals.size(); i++)
						vals[i] = m_old[i];

					math::SplineInterpolant spline(vals);
					vals = spline.eval(Eigen::VectorXf::LinSpaced(m_new.size(), 0.0f, 1.0f));

					for (int i = 0; i < vals.size(); i++)
						m_new[i] = std::max(vals[i], 0.0f);
				}
			}
			m_trgt.set(m_new);
		}
		virtual void reverse() override 
		{
			m_trgt.set(m_old);
		}
		virtual bool reversible() const override { return m_old.size() > 2; }
	};

	class Controls : public gui::SimpleHandles
	{
		Property<std::vector<float>>& m_trgt;
		float m_tmp{ 0.0f };

	public:
		Controls(const std::vector<gui::Floats<2>>& data, Property<std::vector<float>>& trgt) :
			SimpleHandles(data), m_trgt{ trgt } {}

		virtual void onClick(size_t i, gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT)
				m_tmp = m_points[i][1];
		}
		virtual void onMove(size_t i, const gui::Floats<2>& pos) override
		{
			assert(i < m_points.size());

			//we don't allow x movement, disallow negative y
			if (float newY = std::max(pos[1], 0.0f); newY != m_points[i][1])
				asyncInvoke<EditPoint>(m_trgt, i, newY, newY);
		}
		virtual void onRelease(size_t i, gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT) {
				asyncInvoke<EditPoint>(m_trgt, i, m_points[i][1], m_tmp);
				//m_tmp = 0.0f; //doesn't really matter
			}
		}
	};

	class AddButton final : public gui::Button
	{
		Property<std::vector<float>>& m_trgt;

	public:
		AddButton(Property<std::vector<float>>& target) : Button("+"), m_trgt{ target }
		{
			setSize({ gui::getDefaultHeight(), gui::getDefaultHeight() });
		}
		virtual void onActivate() override { asyncInvoke<AddSegment>(m_trgt); }
	};

	class RemoveButton final : public gui::Button
	{
		Property<std::vector<float>>& m_trgt;

	public:
		RemoveButton(Property<std::vector<float>>& target) : Button("-"), m_trgt{ target }
		{
			setSize({ gui::getDefaultHeight(), gui::getDefaultHeight() });
		}
		virtual void onActivate() override { asyncInvoke<RemoveSegment>(m_trgt); }
	};
};

node::ScaleModifier::ScaleModifier(ni_ptr<BSPSysScaleModifier>&& obj) :
	Modifier(obj)
{
	setSize({ WIDTH, HEIGHT });
	setTitle("Scale modifier");

	m_scaleField = newField<ScaleField>("Scale", *this,
		make_ni_ptr(obj, &BSPSysScaleModifier::scales));

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
}

node::ScaleModifier::~ScaleModifier()
{
	disconnect();
}
