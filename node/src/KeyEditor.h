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


namespace node
{
	using namespace nif;

	class FloatKeyEditor final : 
		public gui::Popup, 
		public gui::MouseHandler,
		public gui::ComponentListener
	{
	public:
		class Interpolant
		{
		public:
			Interpolant(float m = 0.0f, float k = 1.0f) : m_m{ m }, m_k{ k } {}
			//should this be in normalised time?
			float eval(float t) { return t <= 0.0f ? m_m : t >= 1.0f ? m_k : m_m + t * m_k; }

			float m_m;
			float m_k;
		};

		class KeyHandle : public gui::Component, public PropertyListener<float>
		{
			class Listener final : public PropertyListener<float>
			{
			public:
				Listener(float* target) : m_target{ target } {}
				virtual void onSet(const float& val) { *m_target = val; }//should also dirty us!

			private:
				float* m_target;
			};

		public:
			//For use in our active widgets. We specialise property_traits
			//to get/set keys->at(index).*member
			struct KeyProperty
			{
				const ni_ptr<Vector<Key<float>>> keys;
				int index;
				Property<float> Key<float>::* member;
			};

		public:
			KeyHandle(ni_ptr<Vector<Key<float>>>&& keys, int index);
			virtual ~KeyHandle();

			virtual void frame(gui::FrameDrawer& fd) override;
			virtual void setTranslation(const gui::Floats<2>& t) override;

			virtual void onSet(const float& val) override;

			void setActive(bool on) { m_active = on; }
			void setSelected(bool on) { m_selected = on; }

			std::unique_ptr<gui::IComponent> getActiveWidget() const;

			std::unique_ptr<gui::ICommand> getMoveOp(
				const std::vector<std::pair<KeyHandle*, gui::Floats<2>>>& initial) const;

			int getIndex() const { return m_index; }
			void setIndex(int i) { m_index = i; }
			//bool isStartKey() const { return m_index == 0; }
			//bool isStopKey() const { return m_index == m_keys->size() - 1; }

			Interpolant getInterpolant();

			void invalidate() { m_invalid = true; }

			void recalcIpln() { m_dirty = true; }

		private:
			Listener m_timeLsnr;
			Listener m_valueLsnr;
			const ni_ptr<Vector<Key<float>>> m_keys;
			//const ni_ptr<NiTimeController> m_ctlr;
			int m_index;
			bool m_selected{ false };
			bool m_active{ false };
			bool m_dirty{ true };
			bool m_invalid{ false };
		};

		class DataSeries final :
			public gui::Composite,
			public nif::PropertyListener<KeyType>,
			public nif::VectorListener<Key<float>>
		{
		public:
			DataSeries(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);
			~DataSeries();

			virtual void frame(gui::FrameDrawer& fd) override;

			virtual void onInsert(int pos) override;
			virtual void onErase(int pos) override;

			gui::Floats<2> getBounds() const;

			//std::unique_ptr<gui::ICommand> getEraseOp() const;
			std::unique_ptr<gui::ICommand> getInsertOp(const gui::Floats<2>& pos) const;

			void setAxisLimits(const gui::Floats<2>& lims) { m_axisLims = lims; }

		private:
			const ni_ptr<NiTimeController> m_ctlr;
			const ni_ptr<NiFloatData> m_data;
			gui::Floats<2> m_axisLims;
		};

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
		DataSeries* m_data{ nullptr };

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

		std::vector<std::pair<KeyHandle*, gui::Floats<2>>> m_initialState;
		bool m_dragThresholdPassed{ false };
	};
}