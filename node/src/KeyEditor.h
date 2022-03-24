//Copyright 2021, 2022 Jonas Gernandt
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
		public gui::KeyListener
	{
	public:
		using Selection = std::set<KeyHandle*>;

		//AnimationCurve uses a Property<CycleType> (since that's what NiControllerSequence has).
		//This adapts a NiTimeController's FlagSet to that format.
		class CycleTypeAdapter final :
			public FlagSetListener<ControllerFlags>,
			public PropertyListener<CycleType>
		{
		public:
			CycleTypeAdapter(const ni_ptr<FlagSet<ControllerFlags>>& flags);
			~CycleTypeAdapter();

			virtual void onSet(const CycleType& c) override;
			virtual void onRaise(ControllerFlags flags) override;
			virtual void onClear(ControllerFlags flags) override;

			const ni_ptr<Property<CycleType>>& getProperty() const { return m_cycleType; }

		private:
			const ni_ptr<Property<CycleType>> m_cycleType;
			const ni_ptr<FlagSet<ControllerFlags>> m_flags;
		};

	public:
		FloatKeyEditor(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);

		~FloatKeyEditor();

		virtual void onClose() override;

		virtual void onKeyDown(gui::key_t key) override;

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
		const ni_ptr<NiTimeController> m_ctlr;

		CycleTypeAdapter m_cycleTypeAdapter;

		gui::Plot* m_plot{ nullptr };
		gui::Composite* m_activePanel{ nullptr };
		AnimationCurve* m_curve{ nullptr };

		enum class Op
		{
			NONE,
			DRAG,
			PAN,
			ZOOM,
		};

		//temp data for handling mouse input
		gui::Floats<2> m_clickPoint;//point clicked in global coords
		gui::Floats<2> m_startS;//axis scale at the time of clicking
		gui::Floats<2> m_startT;//axis translation at the time of clicking
		Op m_currentOp{ Op::NONE };

		KeyHandle* m_clicked{ nullptr };
		std::unique_ptr<AnimationCurve::MoveOperation> m_op;
	};
}