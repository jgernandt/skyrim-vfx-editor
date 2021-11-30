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
		FloatKeyEditor(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);

		~FloatKeyEditor();

		virtual void onClose() override;

		virtual bool onMouseDown(gui::Mouse::Button button) override;
		virtual bool onMouseUp(gui::Mouse::Button button) override;
		virtual bool onMouseWheel(float delta) override;

		virtual void onMouseMove(const gui::Floats<2>& pos) override;

		virtual void onRemoveChild(gui::IComponent* c, gui::Component* source) override;

	private:
		void drag(const gui::Floats<2>& pos);
		void pan(const gui::Floats<2>& pos);
		void zoom(const gui::Floats<2>& pos);

		void updateAxisUnits();

	private:
		class KeyHandle : public gui::Component
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
			KeyHandle(ni_ptr<Vector<Key<float>>>&& keys, int index);
			virtual ~KeyHandle();

			virtual void frame(gui::FrameDrawer& fd) override;
			virtual void setTranslation(const gui::Floats<2>& t) override;

			virtual void setFocussed(bool on) override {}
			virtual void setSelected(bool on) override { m_selected = on; }

			std::unique_ptr<gui::ICommand> getMoveOp(
				const std::vector<std::pair<KeyHandle*, gui::Floats<2>>>& initial) const;

			int getIndex() const { return m_index; }
			void setIndex(int i) { m_index = i; }

			void invalidate();

			void recalcIpln() { m_dirty = true; }

		private:
			Listener m_timeLsnr;
			Listener m_valueLsnr;
			ni_ptr<Vector<Key<float>>> m_keys;
			int m_index;
			bool m_selected{ false };
			bool m_dirty{ true };
		};

		using Selection = std::set<KeyHandle*>;

		class DataSeries final : 
			public gui::Composite, 
			public nif::PropertyListener<KeyType>,
			public nif::VectorListener<Key<float>>
		{
		public:
			DataSeries(const ni_ptr<NiFloatData>& data);
			~DataSeries();

			//virtual void onSet(const KeyType& type) override;
			virtual void onInsert(int pos) override;
			virtual void onErase(int pos) override;

			gui::Floats<2> getBounds() const;
			NiFloatData* get() { return m_data.get(); }

			//std::unique_ptr<gui::ICommand> getEraseOp() const;
			std::unique_ptr<gui::ICommand> getInsertOp(const gui::Floats<2>& pos) const;

		private:
			const ni_ptr<NiFloatData> m_data;
		};

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

		//temp data for handling mouse input
		gui::Floats<2> m_clickPoint;//point clicked in global coords
		gui::Floats<2> m_startS;//axis scale at the time of clicking
		gui::Floats<2> m_startT;//axis translation at the time of clicking
		Selection::iterator m_activeItem;
		Op m_currentOp{ Op::NONE };

		std::vector<std::pair<KeyHandle*, gui::Floats<2>>> m_initialState;
		bool m_dragThresholdPassed{ false };
	};
}