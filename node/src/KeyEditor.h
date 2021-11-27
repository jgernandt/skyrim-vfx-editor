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
		public gui::Popup, public nif::PropertyListener<KeyType>, public gui::MouseHandler
	{
	public:
		FloatKeyEditor(
			ni_ptr<Property<KeyType>>&& keyType,
			ni_ptr<List<Key<float>>>&& keys,
			ni_ptr<Property<float>>&& tStart,
			ni_ptr<Property<float>>&& tStop);

		~FloatKeyEditor();

		virtual void onClose() override;

		virtual void onSet(const KeyType& type) override;

		virtual bool onMouseDown(gui::Mouse::Button button) override;
		virtual bool onMouseUp(gui::Mouse::Button button) override;
		virtual bool onMouseWheel(float delta) override;

		virtual void onMouseMove(const gui::Floats<2>& pos) override;

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
			KeyHandle(ni_ptr<Key<float>>&& key, ni_ptr<Key<float>>&& next);
			virtual ~KeyHandle();

			virtual void frame(gui::FrameDrawer& fd) override;
			virtual void setTranslation(const gui::Floats<2>& t) override;

			virtual void setFocussed(bool on) override {}
			virtual void setSelected(bool on) override { m_selected = on; }

			ni_ptr<Property<float>> getXProperty() const;
			ni_ptr<Property<float>> getYProperty() const;

		private:
			Listener m_timeLsnr;
			Listener m_valueLsnr;
			ni_ptr<Key<float>> m_key;
			ni_ptr<Key<float>> m_next;
			bool m_selected{ false };
			bool m_dirty{ true };
		};

		using Selection = std::set<KeyHandle*>;


		class Interpolant : public gui::Composite
		{
		public:
			virtual ~Interpolant() = default;
			virtual gui::Floats<2> getBounds() const = 0;

			virtual void insertKey(const gui::Floats<2>& key) {}
			virtual void setKey(const gui::Floats<2>& key) {}
		};

		class ConstantInterpolant final : public Interpolant
		{
		public:
			virtual gui::Floats<2> getBounds() const override { return gui::Floats<2>(0.0f, 0.0f); }
		};
		class LinearInterpolant final : 
			public Interpolant, public ListListener<Key<float>>
		{
		public:
			LinearInterpolant(const ni_ptr<List<Key<float>>>& keys);
			~LinearInterpolant();

			virtual gui::Floats<2> getBounds() const override;

			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}

		private:
			const ni_ptr<List<Key<float>>> m_keys;
		};
		class QuadraticInterpolant final :
			public Interpolant, public ListListener<Key<float>>
		{
		public:
			QuadraticInterpolant(const ni_ptr<List<Key<float>>>& keys) {}

			virtual gui::Floats<2> getBounds() const override { return gui::Floats<2>(0.0f, 0.0f); }

			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}
		};

		const ni_ptr<Property<KeyType>> m_keyType;
		const ni_ptr<List<Key<float>>> m_keys;

		gui::Plot* m_plot{ nullptr };
		Interpolant* m_curve{ nullptr };

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