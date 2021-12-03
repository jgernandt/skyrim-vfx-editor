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

#pragma once
#include <set>
#include "Graph.h"
#include "Popup.h"
#include "NiController.h"
#include "node_concepts.h"
#include "AnimationCurve.h"

namespace node
{
	using namespace nif;

	class FloatKeyEditor final : 
		public gui::Popup, 
		public gui::MouseHandler,
		public gui::ComponentListener
	{
	public:
		using Selection = std::set<KeyHandle*>;

		class FrequencyListener final : public PropertyListener<float>
		{
		public:
			FrequencyListener(Property<float>* phase) : m_phase{ phase } {}
			virtual void onSet(const float& f) override;
			void setTarget(gui::IComponent* target) { m_target = target; }

		private:
			Property<float>* m_phase;
			IComponent* m_target{ nullptr };
		};

		class PhaseListener final : public PropertyListener<float>
		{
		public:
			PhaseListener(Property<float>* frequency) : m_frequency{ frequency } {}
			virtual void onSet(const float& f) override;
			void setTarget(gui::IComponent* target) { m_target = target; }

		private:
			Property<float>* m_frequency;
			IComponent* m_target{ nullptr };
		};

	public:
		FloatKeyEditor(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);

		~FloatKeyEditor();

		virtual void onClose() override;

		virtual bool onMouseDown(gui::Mouse::Button button) override;
		virtual bool onMouseUp(gui::Mouse::Button button) override;
		virtual bool onMouseWheel(float delta) override;

		virtual void onMouseMove(const gui::Floats<2>& pos) override;

		virtual void onAddChild(gui::IComponent* c, gui::Component* source) override;
		virtual void onRemoveChild(gui::IComponent* c, gui::Component* source) override;

		void setActiveKey(Selection::iterator key);

	private:
		void drag(const gui::Floats<2>& pos);
		void pan(const gui::Floats<2>& pos);
		void zoom(const gui::Floats<2>& pos);

		void updateAxisUnits();

	private:
		const ni_ptr<NiTimeController> m_ctlr;

		FrequencyListener m_freqLsnr;
		PhaseListener m_phaseLsnr;

		gui::Plot* m_plot{ nullptr };
		AnimationCurve* m_data{ nullptr };

		enum class Op
		{
			NONE,
			DRAG,
			PAN,
			ZOOM,
		};
		Selection m_selection;
		Selection::iterator m_activeItem{ m_selection.end() };
		gui::Composite* m_activePanel{ nullptr };

		//temp data for handling mouse input
		gui::Floats<2> m_clickPoint;//point clicked in global coords
		gui::Floats<2> m_startS;//axis scale at the time of clicking
		gui::Floats<2> m_startT;//axis translation at the time of clicking
		Op m_currentOp{ Op::NONE };

		std::unique_ptr<AnimationCurve::Operation> m_op;
		bool m_dragThresholdPassed{ false };
	};
}