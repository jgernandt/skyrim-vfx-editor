//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "ScaleModifier.h"
#include "widget_types.h"

#include "SplineInterpolant.h"

//We will fill out the scale values with time values and keep the resulting vector of points locally.
//We then add one widget to control the points and one to add/remove points, both of which pass their actions back to us.
//Not the most efficient design, since we're copying a whole new vector (back and forth) when changing one point.
//We'd need some sort of iterator or random access property to make it better. No big deal, though.
class node::ScaleModifier::ScaleField final : public Field, public IPropertyListener<std::vector<float>>
{
public:
	ScaleField(const std::string& name, ScaleModifier& node) : Field(name), m_prop{ node.object().scales() }
	{
		node.newChild<gui::Separator>();
		node.newChild<gui::Text>("Scale");
		auto area = node.newChild<gui::PlotArea>();
		area->addCurve(std::make_unique<gui::SimpleCurve>(m_data));
		area->addChild(std::make_unique<Controls>(*area, m_data, node.object().scales()));

		//segments +-
		auto item = node.newChild<gui::Item>(std::make_unique<gui::RightAlign>());
		item->newChild<gui::Text>("Segments");
		item->newChild<RemoveButton>(node.object().scales());
		item->newChild<AddButton>(node.object().scales());
		
		node.object().scales().addListener(*this);
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

private:
	class EditPoint final : public gui::ICommand
	{
	public:
		EditPoint(IProperty<std::vector<float>>& trgt, size_t i, float to, float from) :
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

	private:
		IProperty<std::vector<float>>& m_trgt;
		size_t m_index;
		float m_from;
		float m_to;
	};
	class AddSegment final : public gui::ICommand
	{
	public:
		AddSegment(IProperty<std::vector<float>>& trgt) : m_trgt{ trgt } {}

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

	private:
		IProperty<std::vector<float>>& m_trgt;
		std::vector<float> m_old;
		std::vector<float> m_new;
	};
	class RemoveSegment final : public gui::ICommand
	{
	public:
		RemoveSegment(IProperty<std::vector<float>>& trgt) : m_trgt{ trgt } {}

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

	private:
		IProperty<std::vector<float>>& m_trgt;
		std::vector<float> m_old;
		std::vector<float> m_new;
	};

	class Controls : public gui::SimpleHandles
	{
	public:
		Controls(const gui::PlotArea& area, const std::vector<gui::Floats<2>>& data, IProperty<std::vector<float>>& trgt) :
			SimpleHandles(area, data), m_trgt{ trgt } {}

		virtual void onClick(size_t i, gui::MouseButton button) override
		{
			if (button == gui::MouseButton::LEFT)
				m_tmp = m_points[i][1];
		}
		virtual void onMove(size_t i, const gui::Floats<2>& pos) override
		{
			assert(i < m_points.size());

			//we don't allow x movement
			if (pos[1] != m_points[i][1])
				asyncInvoke<EditPoint>(m_trgt, i, pos[1], pos[1]);
		}
		virtual void onRelease(size_t i, gui::MouseButton button) override
		{
			if (button == gui::MouseButton::LEFT) {
				asyncInvoke<EditPoint>(m_trgt, i, m_points[i][1], m_tmp);
				//m_tmp = 0.0f; //doesn't really matter
			}
		}

	private:
		IProperty<std::vector<float>>& m_trgt;
		float m_tmp{ 0.0f };
	};

	class AddButton final : public gui::Button
	{
	public:
		AddButton(IProperty<std::vector<float>>& target) : Button("+"), m_trgt{ target }
		{
			setSize({ gui::getDefaultHeight(), gui::getDefaultHeight() });
		}
		virtual void onActivate() override { asyncInvoke<AddSegment>(m_trgt); }

	private:
		IProperty<std::vector<float>>& m_trgt;
	};

	class RemoveButton final : public gui::Button
	{
	public:
		RemoveButton(IProperty<std::vector<float>>& target) : Button("-"), m_trgt{ target }
		{
			setSize({ gui::getDefaultHeight(), gui::getDefaultHeight() });
		}
		virtual void onActivate() override { asyncInvoke<RemoveSegment>(m_trgt); }

	private:
		IProperty<std::vector<float>>& m_trgt;
	};

	IProperty<std::vector<float>>& m_prop;
	std::vector<gui::Floats<2>> m_data;
};

node::ScaleModifier::ScaleModifier() : 
	ScaleModifier(std::make_unique<nif::BSPSysScaleModifier>())
{
	object().scales().set({ 0.0f, 1.0f });
}

node::ScaleModifier::ScaleModifier(std::unique_ptr<nif::BSPSysScaleModifier>&& obj) :
	Modifier(std::move(obj))
{
	setSize({ 200.0f, 0.0f });
	setTitle("Scale modifier");

	addTargetField(std::make_shared<Device>(*this));
	newField<ScaleField>("Scale", *this);
}

nif::BSPSysScaleModifier& node::ScaleModifier::object()
{
	assert(!getObjects().empty() && getObjects()[0]);
	return *static_cast<nif::BSPSysScaleModifier*>(getObjects()[0].get());
}
