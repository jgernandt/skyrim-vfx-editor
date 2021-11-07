#pragma once
#include "Graph.h"
#include "Popup.h"
#include "NiController.h"

namespace node
{
	class FloatKeyEditor final : public gui::Popup
	{
	public:
		FloatKeyEditor(nif::NiFloatData& data, IProperty<float>& tStart, IProperty<float>& tStop);

		virtual void onClose() override;

	private:
		constexpr static float SCALE_BASE = 1.1f;
		constexpr static float SCALE_SENSITIVITY = 0.1f;

		class PlotAreaInput final : public gui::MouseHandler
		{
		public:
			PlotAreaInput(gui::PlotArea& area) : m_area{ area } {}

			virtual void onMouseDown(gui::Mouse::Button button) override;
			virtual void onMouseUp(gui::Mouse::Button button) override;
			virtual void onMouseMove(const gui::Floats<2>& delta) override;
			virtual void onMouseWheel(float delta) override;

		private:
			void applyScale(const gui::Floats<2>& factor, const gui::Floats<2>& pivot);
			void updateAxisUnits();

		private:
			gui::PlotArea& m_area;
			gui::Floats<2> m_zoomPivot;
			bool m_panning{ false };
			bool m_zooming{ false };
		};

		nif::NiFloatData& m_data;
		IProperty<float>& m_tStart;
		IProperty<float>& m_tStop;
	};
}