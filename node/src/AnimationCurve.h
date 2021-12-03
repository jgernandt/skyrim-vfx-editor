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
	struct AnimationClip
	{
		int size() { return points.size(); }

		float time(int i) const
		{
			assert(i >= 0);
			return points[i][0];
		}

		float value(int i) const
		{
			assert(i >= 0);
			return points[i][1];
		}

		gui::Floats<2>& operator[](int i) { return points[i]; }
		const gui::Floats<2>& operator[](int i) const { return points[i]; }
		gui::Floats<2>& front() { return points.front(); }
		gui::Floats<2>& back() { return points.back(); }

		std::vector<gui::Floats<2>> points;
		float length{ 0.0f };
	};

	class AnimationCurve final :
		public gui::Composite,
		public nif::PropertyListener<KeyType>,
		public nif::VectorListener<Key<float>>
	{
	public:
		class Operation : public gui::ICommand
		{
		public:
			virtual ~Operation() = default;
			//Tailored to MoveOp specifically, we'll see how useful it turns out in general.
			//Update the total move of this op.
			virtual void update(const gui::Floats<2>& local_move) = 0;
		};

	public:
		AnimationCurve(const ni_ptr<NiTimeController>& ctlr, const ni_ptr<NiFloatData>& data);
		~AnimationCurve();

		virtual void frame(gui::FrameDrawer& fd) override;

		virtual void onInsert(int pos) override;
		virtual void onErase(int pos) override;

		gui::Floats<2> getBounds() const;

		//std::unique_ptr<gui::ICommand> getEraseOp() const;
		std::unique_ptr<gui::ICommand> getInsertOp(const gui::Floats<2>& pos) const;

		void setAxisLimits(const gui::Floats<2>& lims) { m_axisLims = lims; }

	private:
		void buildClip(const gui::Floats<2>& lims, float resolution);

	private:
		const ni_ptr<NiTimeController> m_ctlr;
		const ni_ptr<NiFloatData> m_data;
		AnimationClip m_clip;
		gui::Floats<2> m_axisLims;
	};

	class Interpolant
	{
	public:
		Interpolant(float m = 0.0f, float k = 1.0f) : m_m{ m }, m_k{ k } {}
		//should this be in normalised time?
		float eval(float t) const { return t <= 0.0f ? m_m : t >= 1.0f ? m_m + m_k : m_m + t * m_k; }

		float m_m;
		float m_k;
	};

	class KeyHandle : public gui::Component, public PropertyListener<float>
	{
		class Listener final : public PropertyListener<float>
		{
		public:
			Listener(KeyHandle* owner, float* target) : m_owner{ owner }, m_target{ target } {}
			virtual void onSet(const float& val) { *m_target = val; m_owner->setDirty(); }

		private:
			KeyHandle* m_owner;
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

		std::unique_ptr<AnimationCurve::Operation> getMoveOp(const std::set<KeyHandle*>& keys) const;

		int getIndex() const { return m_index; }
		void setIndex(int i) { m_index = i; }

		//evaluate the interpolation at time t (normalised to the time interval)
		float eval(float t);

		void invalidate() { m_invalid = true; }

		void setDirty() { m_dirty = true; }

	private:
		Listener m_timeLsnr;
		Listener m_valueLsnr;
		const ni_ptr<Vector<Key<float>>> m_keys;
		Interpolant m_interpolant;
		int m_index;
		bool m_selected{ false };
		bool m_active{ false };
		bool m_dirty{ true };
		bool m_invalid{ false };
	};
}
