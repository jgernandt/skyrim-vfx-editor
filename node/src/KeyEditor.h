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
		using Selection = std::set<IComponent*>;

		class Interpolant;
		class KeyFrame;
		class ForwardHandle
		{
			//Maybe instead of holding an interface ptr directly we should know
			//what KeyFrame we are a part of and the name (or other id)
			//of the property we represent.

			KeyFrame* m_keyFrame;
			const char* id = "ForwardTangent";
		};
		class BackwardHandle
		{
		};

		class KeyFrame
		{
			Interpolant* m_curve;
			int m_index;
			ForwardHandle* m_fwd;
			BackwardHandle* m_bwd;
		};

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
			class LinearHandle;
		public:
			LinearInterpolant(const ni_ptr<List<Key<float>>>& keys);
			~LinearInterpolant();

			virtual void frame(gui::FrameDrawer& fd) override;

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
		gui::Floats<2> m_clickPoint;
		gui::Floats<2> m_startS;
		gui::Floats<2> m_startT;
		Selection::iterator m_activeItem;
		Op m_currentOp{ Op::NONE };

		std::vector<std::pair<IComponent*, gui::Floats<2>>> m_initialState;
		bool m_dragThresholdPassed{ false };
	};
}