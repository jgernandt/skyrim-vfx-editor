#pragma once
#include "Graph.h"
#include "Popup.h"
#include "NiController.h"

namespace node
{
	class FloatKeyEditor final : public gui::Popup, public PropertyListener<nif::KeyType>
	{
	public:
		FloatKeyEditor(nif::NiFloatData& data, IProperty<float>& tStart, IProperty<float>& tStop);
		~FloatKeyEditor();

		virtual void onClose() override;

		virtual void onSet(const nif::KeyType& type) override;

	private:
		constexpr static float SCALE_BASE = 1.1f;
		constexpr static float SCALE_SENSITIVITY = 0.1f;

		class ConstantInterpolation final : public gui::Composite
		{

		};
		class LinearInterpolation final : 
			public gui::Composite, public ListPropertyListener<nif::Key<float>>
		{
		public:
			LinearInterpolation(IListProperty<nif::Key<float>>& keys);
			~LinearInterpolation();

			virtual void frame(gui::FrameDrawer& fd) override;

			//redo everything?
			virtual void onSet(const std::vector<nif::Key<float>>& keys) override {}
			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}

		private:
			IListProperty<nif::Key<float>>& m_keys;
			//unless we add a way to iterate through the property (good idea?), 
			//we should store a copy:
			std::vector<nif::Key<float>> m_data;
		};
		class QuadraticInterpolation final :
			public gui::Composite, public ListPropertyListener<nif::Key<float>>
		{
		public:
			QuadraticInterpolation(IListProperty<nif::Key<float>>& keys) {}

			//redo everything?
			virtual void onSet(const std::vector<nif::Key<float>>& keys) override {}
			//update our interpolation of [i-1, i] and [i, i+1]
			virtual void onSet(int i, const nif::Key<float>& key) override {}
			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}
		};

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

		IListProperty<nif::Key<float>>& m_keys;
		IProperty<nif::KeyType>& m_keyType;

		gui::Plot* m_plot{ nullptr };
		gui::IComponent* m_curve{ nullptr };
	};
}