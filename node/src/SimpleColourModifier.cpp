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
#include "SimpleColourModifier.h"
#include "ICommand.h"
#include "widget_types.h"

//Colour stops should be increasing. Force some small difference:
constexpr float DELTA = 1.1e-4f;
constexpr float RGB1END_MIN = 0.0f;
constexpr float RGB2BEG_MIN = RGB1END_MIN + DELTA;
constexpr float RGB2END_MIN = RGB2BEG_MIN + DELTA;
constexpr float RGB3BEG_MIN = RGB2END_MIN + DELTA;
constexpr float RGB3BEG_MAX = 1.0f;
constexpr float RGB2END_MAX = RGB3BEG_MAX - DELTA;
constexpr float RGB2BEG_MAX = RGB2END_MAX - DELTA;
constexpr float RGB1END_MAX = RGB2BEG_MAX - DELTA;
constexpr float ALPHA2BEG_MIN = 0.0f;
constexpr float ALPHA2END_MIN = ALPHA2BEG_MIN + DELTA;
constexpr float ALPHA2END_MAX = 1.0f;
constexpr float ALPHA2BEG_MAX = ALPHA2END_MAX - DELTA;

//This must hold, but may fail to due unlucky roundoff. Better procedure?
static_assert(RGB3BEG_MIN - DELTA - DELTA - DELTA >= 0.0f);
static_assert(RGB2END_MIN - DELTA - DELTA >= 0.0f);
static_assert(RGB2BEG_MIN - DELTA >= 0.0f);
static_assert(RGB1END_MAX + DELTA + DELTA + DELTA <= 1.0f);
static_assert(RGB2BEG_MAX + DELTA + DELTA <= 1.0f);
static_assert(ALPHA2END_MIN - DELTA >= 0.0f);
static_assert(ALPHA2BEG_MAX + DELTA <= 1.0f);

using namespace nif;
using namespace node;

class ColourField final : public Field
{
public:
	ColourField(const std::string& name, NodeBase& node, const ni_ptr<BSPSysSimpleColorModifier>& obj) : 
		Field(name),
		m_obj{ obj },
		m_end1Less(m_obj->col1.RGBend),
		m_begin2Less(m_obj->col2.RGBbegin),
		m_end2Less(m_obj->col2.RGBend),
		m_a2BegLess(m_obj->col2.Abegin),
		m_begin2Gr(m_obj->col2.RGBbegin),
		m_end2Gr(m_obj->col2.RGBend),
		m_begin3Gr(m_obj->col3.RGBbegin),
		m_a2EndGr(m_obj->col2.Aend)
	{
		assert(m_obj);

		//Limit our props to [0, 1], increasing by at least DELTA
		std::array<Property<float>*, 4> rgbs{ &m_obj->col1.RGBend, &m_obj->col2.RGBbegin, &m_obj->col2.RGBend, &m_obj->col3.RGBbegin };
		for (int i = 0; i < 4; i++)
			rgbs[i]->set(std::min(std::max(rgbs[i]->get(), i > 0 ? rgbs[i - 1]->get() + DELTA : 0.0f), 1.0f - (3 - i) * DELTA));

		std::array<Property<float>*, 2> alphas{ &m_obj->col2.Abegin, &m_obj->col2.Aend };
		alphas[0]->set(std::min(std::max(alphas[0]->get(), 0.0f), 1.0f - DELTA));
		alphas[1]->set(std::min(std::max(alphas[1]->get(), alphas[0]->get() + DELTA), 1.0f));

		auto grad = node.newChild<gui::ColourBar>(std::make_unique<Gradient>(m_obj));
		auto labels = node.newChild<gui::Item>(std::make_unique<gui::MarginAlign>());
		labels->newChild<gui::Text>("Birth");
		labels->newChild<gui::Text>("Death");
		node.newChild<RGBHandle>(*grad, rgbs, 0, gui::Floats<2>{ RGB1END_MIN, RGB1END_MAX });
		node.newChild<RGBHandle>(*grad, rgbs, 1, gui::Floats<2>{ RGB2BEG_MIN, RGB2BEG_MAX });
		node.newChild<RGBHandle>(*grad, rgbs, 2, gui::Floats<2>{ RGB2END_MIN, RGB2END_MAX });
		node.newChild<RGBHandle>(*grad, rgbs, 3, gui::Floats<2>{ RGB3BEG_MIN, RGB3BEG_MAX });

		node.newChild<AlphaHandle>(*grad, alphas, 0, gui::Floats<2>{ ALPHA2BEG_MIN, ALPHA2BEG_MAX });
		node.newChild<AlphaHandle>(*grad, alphas, 1, gui::Floats<2>{ ALPHA2END_MIN, ALPHA2END_MAX });

		auto cols = node.newChild<gui::Item>(std::make_unique<gui::MarginAlign>());
		cols->newChild<ColourInput>(make_ni_ptr(m_obj, &m_obj->col1.value), "");
		cols->newChild<ColourInput>(make_ni_ptr(m_obj, &m_obj->col2.value), "");
		cols->newChild<ColourInput>(make_ni_ptr(m_obj, &m_obj->col3.value), "");

		m_obj->col1.RGBend.addListener(m_begin2Gr);

		m_obj->col2.RGBbegin.addListener(m_end1Less);
		m_obj->col2.RGBbegin.addListener(m_end2Gr);

		m_obj->col2.RGBend.addListener(m_begin2Less);
		m_obj->col2.RGBend.addListener(m_begin3Gr);

		m_obj->col3.RGBbegin.addListener(m_end2Less);

		m_obj->col2.Aend.addListener(m_a2BegLess);
		m_obj->col2.Abegin.addListener(m_a2EndGr);
	}

	~ColourField()
	{
		m_obj->col1.RGBend.removeListener(m_begin2Gr);

		m_obj->col2.RGBbegin.removeListener(m_end1Less);
		m_obj->col2.RGBbegin.removeListener(m_end2Gr);

		m_obj->col2.RGBend.removeListener(m_begin2Less);
		m_obj->col2.RGBend.removeListener(m_begin3Gr);

		m_obj->col3.RGBbegin.removeListener(m_end2Less);

		m_obj->col2.Aend.removeListener(m_a2BegLess);
		m_obj->col2.Abegin.removeListener(m_a2EndGr);
	}

private:
	//If listening to a source property, sets a target property to the same if Compare(source, target) returns true
	template<typename Compare>
	class Limiter final : public PropertyListener<float>
	{
		Property<float>& m_dest;

	public:
		Limiter(Property<float>& dest) : m_dest{ dest } {}

		virtual void onSet(const float& sourceVal) override
		{
			if (Compare()(sourceVal, m_dest.get())) {
				if constexpr (Compare()(1.0f, 0.0f)) {
					//we should set to a value greater than sourceVal
					m_dest.set(sourceVal + DELTA);
				}
				else {
					//we should set to less
					m_dest.set(sourceVal - DELTA);
				}
			}
		}
	};

	class Gradient final : public gui::ColourGradient
	{
		const ni_ptr<BSPSysSimpleColorModifier> m_obj;

	public:
		Gradient(const ni_ptr<BSPSysSimpleColorModifier>& obj) : m_obj{ obj } { assert(m_obj); }

		virtual void draw(gui::Drawer& d) const override 
		{
			float alpha2Begin = m_obj->col2.Abegin.get();
			float alpha2End = m_obj->col2.Aend.get();
			float rgb1End = m_obj->col1.RGBend.get();
			float rgb2Begin = m_obj->col2.RGBbegin.get();
			float rgb2End = m_obj->col2.RGBend.get();
			float rgb3Begin = m_obj->col3.RGBbegin.get();
			ColRGBA col1 = m_obj->col1.value.get();
			ColRGBA col2 = m_obj->col2.value.get();
			ColRGBA col3 = m_obj->col3.value.get();

			assert(alpha2Begin >= 0.0f);
			assert(alpha2End > alpha2Begin && alpha2End <= 1.0f);
			assert(rgb1End >= 0.0f);
			assert(rgb2Begin > rgb1End);
			assert(rgb2End > rgb2Begin);
			assert(rgb3Begin > rgb2End && rgb3Begin <= 1.0f);

			std::vector<std::pair<float, ColRGBA>> stops;
			stops.reserve(8);
			//Insert colour stops
			stops.push_back({ 0.0f, col1 });
			stops.push_back({ rgb1End, col1 });
			stops.push_back({ rgb2Begin, col2 });
			stops.push_back({ rgb2End, col2 });
			stops.push_back({ rgb3Begin, col3 });
			stops.push_back({ 1.0f, col3 });

			//Set alpha
			for (auto&& stop : stops) {
				float a;
				if (stop.first > alpha2End && alpha2End != 1.0f)
					a = util::colour_traits<nif::ColRGBA>::A(col2) + 
						(util::colour_traits<nif::ColRGBA>::A(col3) - util::colour_traits<nif::ColRGBA>::A(col2)) * (stop.first - alpha2End) / (1.0f - alpha2End);
				else if (stop.first < alpha2Begin && alpha2Begin != 0.0f)
					a = util::colour_traits<nif::ColRGBA>::A(col1) +
						(util::colour_traits<nif::ColRGBA>::A(col2) - util::colour_traits<nif::ColRGBA>::A(col1)) * stop.first / alpha2Begin;
				else
					a = util::colour_traits<nif::ColRGBA>::A(col2);
					
				util::colour_traits<nif::ColRGBA>::A(stop.second) = a;
			}
			//Insert alpha stops
			auto it = std::find_if(stops.begin(), stops.end(),
				[alpha2Begin](const std::pair<float, nif::ColRGBA>& s) { return s.first >= alpha2Begin; });

			assert(it != stops.end());//or alpha2Begin >= 1

			if (it->first != alpha2Begin) {
				assert(it != stops.begin());//sanity check

				//Insert a stop before it. Alpha should be col2::A, rgb should be interpolated from previous stop
				it = stops.insert(it, { alpha2Begin, it->second });
				util::colour_traits<nif::ColRGBA>::A(it->second) = util::colour_traits<nif::ColRGBA>::A(col2);
				auto prev = it - 1;
				if (util::colour_traits<nif::ColRGBA>::R(prev->second) != util::colour_traits<nif::ColRGBA>::R(it->second))
					util::colour_traits<nif::ColRGBA>::R(it->second) = util::colour_traits<nif::ColRGBA>::R(prev->second) +
						(util::colour_traits<nif::ColRGBA>::R(it->second) - util::colour_traits<nif::ColRGBA>::R(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
				if (util::colour_traits<nif::ColRGBA>::G(prev->second) != util::colour_traits<nif::ColRGBA>::G(it->second))
					util::colour_traits<nif::ColRGBA>::G(it->second) = util::colour_traits<nif::ColRGBA>::G(prev->second) +
						(util::colour_traits<nif::ColRGBA>::G(it->second) - util::colour_traits<nif::ColRGBA>::G(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
				if (util::colour_traits<nif::ColRGBA>::B(prev->second) != util::colour_traits<nif::ColRGBA>::B(it->second))
					util::colour_traits<nif::ColRGBA>::B(it->second) = util::colour_traits<nif::ColRGBA>::B(prev->second) +
						(util::colour_traits<nif::ColRGBA>::B(it->second) - util::colour_traits<nif::ColRGBA>::B(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
			}

			it = std::find_if(it, stops.end(),
				[alpha2End](const std::pair<float, nif::ColRGBA>& s) { return s.first >= alpha2End; });

			assert(it != stops.end());//or alpha2End > 1

			if (it->first != alpha2End) {
				//Same procedure
				it = stops.insert(it, { alpha2End, it->second });
				util::colour_traits<nif::ColRGBA>::A(it->second) = util::colour_traits<nif::ColRGBA>::A(col2);
				auto prev = it - 1;
				if (util::colour_traits<nif::ColRGBA>::R(prev->second) != util::colour_traits<nif::ColRGBA>::R(it->second))
					util::colour_traits<nif::ColRGBA>::R(it->second) = util::colour_traits<nif::ColRGBA>::R(prev->second) +
						(util::colour_traits<nif::ColRGBA>::R(it->second) - util::colour_traits<nif::ColRGBA>::R(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
				if (util::colour_traits<nif::ColRGBA>::G(prev->second) != util::colour_traits<nif::ColRGBA>::G(it->second))
					util::colour_traits<nif::ColRGBA>::G(it->second) = util::colour_traits<nif::ColRGBA>::G(prev->second) +
						(util::colour_traits<nif::ColRGBA>::G(it->second) - util::colour_traits<nif::ColRGBA>::G(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
				if (util::colour_traits<nif::ColRGBA>::B(prev->second) != util::colour_traits<nif::ColRGBA>::B(it->second))
					util::colour_traits<nif::ColRGBA>::B(it->second) = util::colour_traits<nif::ColRGBA>::B(prev->second) +
						(util::colour_traits<nif::ColRGBA>::B(it->second) - util::colour_traits<nif::ColRGBA>::B(prev->second)) *
						(it->first - prev->first) / ((it + 1)->first - prev->first);
			}

			//Draw the rectangles
			d.begin();
			for (auto it = stops.begin() + 1; it != stops.end(); ++it) {
				auto prev = it - 1;
				d.rectangleGradient({ prev->first, 0.0f }, { it->first, 1.0f }, prev->second, it->second, prev->second, it->second);
			}
			d.end();
		}
	};

	template<size_t N>
	class EditStop final : public gui::ICommand
	{
		std::array<Property<float>*, N> m_props;
		int m_index;
		float m_value;
		std::array<float, N> m_olds{ 0.0f };
		bool m_reversible;
		bool m_async;

	public:
		//If reversible, reverts to the values of props at the time of execution
		EditStop(const std::array<Property<float>*, N>& props, int index, float val, bool reversible) :
			m_props{ props }, m_index{ index }, m_value{ val }, m_reversible{ reversible }, m_async{ false } {}

		//Reverts to the values olds, regardless of the current values of the properties
		EditStop(const std::array<Property<float>*, N>& props, int index, float val, const std::array<float, N>& olds) :
			m_props{ props }, m_index{ index }, m_value{ val }, m_olds{ olds }, m_reversible{ true }, m_async{ true } {}

		virtual void execute() override
		{
			if (m_reversible && !m_async) {
				for (size_t i = 0; i < m_props.size(); i++)
					m_olds[i] = m_props[i]->get();
			}
			m_props[m_index]->set(m_value);
		}
		virtual void reverse() override
		{
			for (size_t i = 0; i < m_props.size(); i++)
				m_props[i]->set(m_olds[i]);
		}
		virtual bool reversible() const override
		{
			return m_reversible && m_value != m_olds[m_index];
		}
	};

	class RGBHandle final : public gui::Handle
	{
		const gui::IComponent& m_gradient;
		std::array<Property<float>*, 4> m_props;
		int m_index;
		gui::Floats<2> m_lims;
		std::array<float, 4> m_tmps{ 0.0f };

	public:
		RGBHandle(const gui::IComponent& gradient, std::array<Property<float>*, 4> props, int index, const gui::Floats<2>& limits) :
			m_gradient{ gradient }, m_props{ props }, m_index{ index }, m_lims{ limits }
		{ 
			m_size = { 10.0f, 10.0f }; 
		}

		virtual void frame(gui::FrameDrawer& fd) override
		{
			gui::Floats<2> grad_pos = m_gradient.getTranslation();
			gui::Floats<2> grad_size = m_gradient.getSize();
			gui::Floats<2> currentSize = m_size * fd.getCurrentScale();
			//assumes we are a sibling of the gradient
			setTranslation({ grad_pos[0] + m_props[m_index]->get() * grad_size[0] - 0.5f * m_size[0], grad_pos[1] + grad_size[1] });

			Handle::frame(fd);

			//draw
			auto globalPos = fd.toGlobal(m_translation);
			gui::Drawer d;
			d.setTargetLayer(gui::Layer::WINDOW);
			d.begin();
			d.triangle({ globalPos[0] + 0.5f * currentSize[0], globalPos[1] },
				{ globalPos[0] + 0.15f * currentSize[0], globalPos[1] + 0.8f * currentSize[1] },
				{ globalPos[0] + 0.85f * currentSize[0], globalPos[1] + 0.8f * currentSize[1] },
				isHovered() ? nif::COL_WHITE : nif::COL_BLACK);
			d.end();
		}

		virtual void onClick(gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT)
				for (size_t i = 0; i < m_props.size(); i++)
					m_tmps[i] = m_props[i]->get();
		}
		virtual void onMove(const gui::Floats<2>& delta) override
		{
			if (delta[0] != 0.0f) {
				float newVal = std::min(std::max(m_props[m_index]->get() + delta[0] / m_gradient.getSize()[0], m_lims[0]), m_lims[1]);
				asyncInvoke<EditStop<4>>(m_props, m_index, newVal, false);
			}
		}
		virtual void onRelease(gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT)
				asyncInvoke<EditStop<4>>(m_props, m_index, m_props[m_index]->get(), m_tmps);
		}
	};

	class AlphaHandle final : public gui::Handle
	{
		const gui::IComponent& m_gradient;
		std::array<Property<float>*, 2> m_props;
		int m_index;
		gui::Floats<2> m_lims;
		std::array<float, 2> m_tmps{ 0.0f };

	public:
		AlphaHandle(const gui::IComponent& gradient, std::array<Property<float>*, 2> props, int index, const gui::Floats<2>& limits) :
			m_gradient{ gradient }, m_props{ props }, m_index{ index }, m_lims{ limits }
		{
			m_size = { 10.0f, 10.0f };
		}

		virtual void frame(gui::FrameDrawer& fd) override
		{
			gui::Floats<2> grad_pos = m_gradient.getTranslation();
			gui::Floats<2> grad_size = m_gradient.getSize();
			gui::Floats<2> currentSize = m_size * fd.getCurrentScale();
			//assumes we are a sibling of the gradient
			setTranslation({ grad_pos[0] + m_props[m_index]->get() * grad_size[0] - 0.5f * m_size[0], grad_pos[1] - m_size[1] });

			Handle::frame(fd);

			//draw
			auto globalPos = fd.toGlobal(m_translation);
			gui::Drawer d;
			d.setTargetLayer(gui::Layer::WINDOW);
			d.begin();
			d.triangle({ globalPos[0] + 0.5f * currentSize[0], globalPos[1] + currentSize[1] },
				{ globalPos[0] + 0.15f * currentSize[0], globalPos[1] + 0.2f * currentSize[1] },
				{ globalPos[0] + 0.85f * currentSize[0], globalPos[1] + 0.2f * currentSize[1] },
				isHovered() ? nif::COL_WHITE : nif::COL_BLACK);
			d.end();
		}

		virtual void onClick(gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT)
				for (size_t i = 0; i < m_props.size(); i++)
					m_tmps[i] = m_props[i]->get();
		}
		virtual void onMove(const gui::Floats<2>& delta) override
		{
			if (delta[0] != 0.0f) {
				float newVal = std::min(std::max(m_props[m_index]->get() + delta[0] / m_gradient.getSize()[0], m_lims[0]), m_lims[1]);
				asyncInvoke<EditStop<2>>(m_props, m_index, newVal, false);
			}
		}
		virtual void onRelease(gui::Mouse::Button button) override
		{
			if (button == gui::Mouse::Button::LEFT)
				asyncInvoke<EditStop<2>>(m_props, m_index, m_props[m_index]->get(), m_tmps);
		}
	};

	ni_ptr<BSPSysSimpleColorModifier> m_obj;

	Limiter<std::less_equal<float>> m_end1Less;
	Limiter<std::less_equal<float>> m_begin2Less;
	Limiter<std::less_equal<float>> m_end2Less;
	Limiter<std::less_equal<float>> m_a2BegLess;

	Limiter<std::greater_equal<float>> m_begin2Gr;
	Limiter<std::greater_equal<float>> m_end2Gr;
	Limiter<std::greater_equal<float>> m_begin3Gr;
	Limiter<std::greater_equal<float>> m_a2EndGr;
};

node::SimpleColourModifier::SimpleColourModifier(const ni_ptr<BSPSysSimpleColorModifier>& obj) :
	Modifier(obj)
{
	setSize({ WIDTH, HEIGHT });
	setTitle("Colour modifier");

	m_modifiableDevice.addRequirement(ModRequirement::COLOUR);

	newChild<gui::Separator>();
	newChild<gui::VerticalSpacing>(2);
	m_colField = newField<ColourField>("Colour", *this, obj);

	//until we have some other way to determine connector position for loading placement
	getField(NEXT_MODIFIER)->connector->setTranslation({ WIDTH, 38.0f });
	getField(TARGET)->connector->setTranslation({ 0.0f, 62.0f });
}

node::SimpleColourModifier::~SimpleColourModifier()
{
	disconnect();
}
