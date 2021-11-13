#pragma once
#include <set>
#include "Graph.h"
#include "Popup.h"
#include "NiController.h"

namespace node
{
	class FloatKeyEditor final : 
		public gui::Popup, public nif::PropertyListener<nif::KeyType>, public gui::MouseHandler
	{
	public:
		FloatKeyEditor(
			std::shared_ptr<OProperty<nif::KeyType>>&& keyType,
			std::shared_ptr<nif::InterpolationData<float>>&& keys,
			std::shared_ptr<OProperty<float>>&& tStart,
			std::shared_ptr<OProperty<float>>&& tStop);

		~FloatKeyEditor();

		virtual void onClose() override;

		virtual void onSet(const nif::KeyType& type) override;

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
			public Interpolant, public nif::VectorPropertyListener<nif::Key<float>>
		{
		public:
			class LinearHandle;
		public:
			LinearInterpolant(std::shared_ptr<OVector<nif::Key<float>>>&& keys);
			~LinearInterpolant();

			virtual void frame(gui::FrameDrawer& fd) override;

			virtual gui::Floats<2> getBounds() const override;

			virtual void onSet(int i, const nif::Key<float>& key) override;
			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}

		private:
			std::shared_ptr<OVector<nif::Key<float>>> m_keys;
			//unless we add a way to iterate through the property (good idea?), 
			//we should store a copy:
			std::vector<nif::Key<float>> m_data;
		};
		class QuadraticInterpolant final :
			public Interpolant, public nif::VectorPropertyListener<nif::Key<float>>
		{
		public:
			QuadraticInterpolant(std::shared_ptr<OVector<nif::Key<float>>>&& keys,
				std::shared_ptr<OVector<nif::Tangent<float>>>&& tans) {}

			virtual gui::Floats<2> getBounds() const override { return gui::Floats<2>(); }

			//update our interpolation of [i-1, i] and [i, i+1]
			virtual void onSet(int i, const nif::Key<float>& key) override {}
			//add handle
			virtual void onInsert(int i) override {}
			//remove handle
			virtual void onErase(int i) override {}
		};

		std::shared_ptr<OProperty<nif::KeyType>> m_keyType;
		std::shared_ptr<nif::InterpolationData<float>> m_keys;

		gui::Plot* m_plot{ nullptr };
		Interpolant* m_curve{ nullptr };

		enum class Op
		{
			NONE,
			DRAG,
			PAN,
			ZOOM,
		};
		std::set<IComponent*> m_selection;
		gui::Floats<2> m_clickPoint;
		gui::Floats<2> m_startS;
		gui::Floats<2> m_startT;
		std::set<IComponent*>::iterator m_clickedComp;
		Op m_currentOp{ Op::NONE };

		std::vector<std::pair<IComponent*, gui::Floats<2>>> m_initialState;
		bool m_dragThresholdPassed{ false };
	};
}