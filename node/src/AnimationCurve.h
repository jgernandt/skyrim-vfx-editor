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
#include "NodeBase.h"

namespace node
{
	enum class SelectionState
	{
		NOT_SELECTED,
		SELECTED,
		ACTIVE
	};

	class AnimationKey;

	class AnimationCurve final :
		public gui::Composite,
		public nif::VectorListener<Key<float>>
	{
	public:
		class MoveOperation : public gui::ICommand
		{
		public:
			virtual ~MoveOperation() = default;
			virtual void update(const gui::Floats<2>& local_pos) {}
		};

	public:
		AnimationCurve(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);
		~AnimationCurve();

		virtual void frame(gui::FrameDrawer& fd) override;

		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;
		virtual void onMove(int from, int to) override;

		SelectionState getSelectionState() const { return m_selectionState; }
		void setSelectionState(SelectionState state) { m_selectionState = state; }

		AnimationKey* getActive() const;
		std::vector<AnimationKey*> getSelected() const;

		ni_ptr<Vector<Key<float>>> getKeysPtr() const;
		ni_ptr<Property<KeyType>> getTypePtr() const;
		Vector<Key<float>>& keys() { return m_data->keys; }
		Property<KeyType>& keyType() { return m_data->keyType; }

		gui::Floats<2> getBounds() const;

		std::unique_ptr<gui::ICommand> getEraseOp(const std::vector<AnimationKey*>& keys) const;
		std::unique_ptr<gui::ICommand> getInsertOp(const gui::Floats<2>& pos) const;

		void setAxisLimits(const gui::Floats<2>& lims) { m_axisLims = lims; }

	private:
		void buildClip(const gui::Floats<2>& lims, float resolution);

	private:
		const ni_ptr<NiTimeController> m_ctlr;
		const ni_ptr<NiFloatData> m_data;
		gui::Floats<2> m_axisLims;
		SelectionState m_selectionState{ SelectionState::NOT_SELECTED };

		std::vector<gui::Floats<2>> m_clipPoints;
		float m_clipLength{ 0.0f };
	};

	//Root of the key handle. Responsible for positioning at the correct time/value,
	//and for interpolating to the next key.
	//Parents the interactive widgets.
	class AnimationKey final : 
		public gui::Composite, 
		public PropertyListener<float>,
		public nif::PropertyListener<KeyType>
	{
	public:
		enum class HandleType
		{
			ALIGNED,
			FREE,
		};

	public:
		AnimationKey(AnimationCurve& curve, int index);
		~AnimationKey();

		virtual void setTranslation(const gui::Floats<2>& t) override;

		virtual void onSet(const float& val) override;
		virtual void onSet(const KeyType& val) override;

		AnimationCurve& curve() { return *m_curve; }
		Key<float>& key() { return m_curve->keys().at(m_index); }

		int getIndex() const { return m_index; }
		void setIndex(int i) { m_index = i; }//track this ourselves instead?

		SelectionState getSelectionState() const { return m_selectionState; }
		void setSelectionState(SelectionState state) { m_selectionState = state; }

		AnimationCurve* getCurve() const { return m_curve; }

		//evaluate the interpolation at time t (normalised to the time interval)
		float eval(float t);

		void invalidate() { m_invalid = true; }

		bool getDirty() const { return m_dirty; }
		void setDirty() { m_dirty = true; }

	protected:
		AnimationCurve* const m_curve;
		int m_index;

		SelectionState m_selectionState{ SelectionState::NOT_SELECTED };

		HandleType m_handleType{ HandleType::ALIGNED };

		bool m_dirty{ true };
		bool m_invalid{ false };
	};

	//A widget for inputting key properties. Used in the side panel of the key editor.
	class KeyWidget final :
		public gui::Composite,
		public VectorListener<Key<float>>,
		public PropertyListener<KeyType>
	{
	public:
		KeyWidget(AnimationKey& key);
		~KeyWidget();

		//Keep our index correct
		virtual void onInsert(int i) override;
		virtual void onErase(int i) override;
		virtual void onMove(int from, int to) override;

		//Show widgets for all relevant fields only
		virtual void onSet(const KeyType& type) override;

		int getIndex() const { return m_index; }
		const ni_ptr<Vector<Key<float>>>& getKeysPtr() const { return m_keys; }
		Key<float>& key() { return m_keys->at(m_index); }

	private:
		//We need this if we want to set handle types (aligned/free)
		AnimationKey* m_key;
		//We need these to listen to type and index changes.
		//We need to store them in order to unregister.
		const ni_ptr<Property<KeyType>> m_type;
		const ni_ptr<Vector<Key<float>>> m_keys;
		int m_index;
	};

	//Base type of an interactive key widget.
	class KeyHandle : public gui::Component
	{
	public:
		KeyHandle(AnimationKey& root) : m_root{ &root } {}
		virtual ~KeyHandle() = default;

		virtual std::unique_ptr<AnimationCurve::MoveOperation> getMoveOp(
			std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos) = 0;

		SelectionState getSelectionState() const { return m_root->getSelectionState(); }
		void setSelectionState(SelectionState state) { m_root->setSelectionState(state); }

		AnimationCurve* getCurve() const { return m_root->getCurve(); }
		std::unique_ptr<gui::IComponent> getWidget();

	protected:
		AnimationKey* const m_root;//we could just static_cast our parent
	};

	//Interactive widget for key time/value.
	class CentreHandle final : public KeyHandle
	{
	public:
		CentreHandle(AnimationKey& root) : KeyHandle{ root } {}
		virtual void frame(gui::FrameDrawer& fd) override;
		virtual std::unique_ptr<AnimationCurve::MoveOperation> getMoveOp(
			std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos) override;
	};

	class BwdTangentHandle final : public KeyHandle
	{
	public:
		BwdTangentHandle(AnimationKey& root);
		~BwdTangentHandle();
		virtual void frame(gui::FrameDrawer& fd) override;
		virtual std::unique_ptr<AnimationCurve::MoveOperation> getMoveOp(
			std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos) override;
	};

	class FwdTangentHandle final : public KeyHandle
	{
	public:
		FwdTangentHandle(AnimationKey& root);
		~FwdTangentHandle();
		virtual void frame(gui::FrameDrawer& fd) override;
		virtual std::unique_ptr<AnimationCurve::MoveOperation> getMoveOp(
			std::vector<AnimationKey*>&& keys, const gui::Floats<2>& pos) override;
	};
}
