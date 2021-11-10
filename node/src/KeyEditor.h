#pragma once
#include <set>
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
			virtual gui::Floats<2> getBounds() const override { return gui::Floats<2>(); }
		};
		class LinearInterpolant final : 
			public Interpolant, public VectorPropertyListener<nif::Key<float>>
		{
		public:
			struct LinearKey
			{
				float time{ 0.0f };
				float value{ 0.0f };
			};
			class LinearHandle;
		public:
			LinearInterpolant(IVectorProperty<nif::Key<float>>& keys);
			~LinearInterpolant();

			virtual void frame(gui::FrameDrawer& fd) override;

			virtual gui::Floats<2> getBounds() const override;

			virtual void onSet(const std::vector<nif::Key<float>>& keys) override;
			virtual void onSet(int i, const nif::Key<float>& key) override;
			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}

		private:
			IVectorProperty<nif::Key<float>>& m_keys;
			//unless we add a way to iterate through the property (good idea?), 
			//we should store a copy:
			std::vector<LinearKey> m_data;
		};
		class QuadraticInterpolant final :
			public Interpolant, public VectorPropertyListener<nif::Key<float>>
		{
		public:
			QuadraticInterpolant(IVectorProperty<nif::Key<float>>& keys) {}

			virtual gui::Floats<2> getBounds() const override { return gui::Floats<2>(); }

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
			PlotAreaInput(gui::PlotArea& area);

			virtual bool onMouseDown(gui::Mouse::Button button) override;
			virtual bool onMouseUp(gui::Mouse::Button button) override;
			virtual bool onMouseWheel(float delta) override;

			virtual void onMouseMove(const gui::Floats<2>& pos) override;

		private:
			void updateAxisUnits();

		private:
			gui::PlotArea& m_area;
			gui::Floats<2> m_clickPoint;
			gui::Floats<2> m_startS;
			gui::Floats<2> m_startT;
			bool m_panning{ false };
			bool m_zooming{ false };

			//temp location
			std::set<IComponent*> m_selection;
		};

		IVectorProperty<nif::Key<float>>& m_keys;
		IProperty<nif::KeyType>& m_keyType;

		gui::Plot* m_plot{ nullptr };
		std::unique_ptr<PlotAreaInput> m_inputHandler;
		Interpolant* m_curve{ nullptr };
	};
}