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
#include "NiController.h"
#include "node_concepts.h"

//This might be universally useful
template<typename T, typename FieldType, int Offset, int Width>
class BitsetProperty final : public IProperty<T>
{
	static_assert(std::is_integral<T>::value && std::is_integral<FieldType>::value);
	static_assert(Offset >= 0 && Width > 0);
	static_assert(Offset < 8 * sizeof(FieldType) && Width <= 8 * sizeof(FieldType) - Offset);
public:
	BitsetProperty(IProperty<FieldType>& bitfield) : m_bitfield{ bitfield } {}

	virtual T get() const override
	{
		return static_cast<T>((m_bitfield.get() & MASK) >> Offset);
	}
	virtual void set(const T& t) override
	{
		m_bitfield.set(((static_cast<FieldType>(t) << Offset) & MASK) | (m_bitfield.get() & ~MASK));
	}

private:
	constexpr static FieldType MASK = ~(~FieldType(0) << Width) << Offset;

	IProperty<FieldType>& m_bitfield;
};

namespace node
{
	class FloatController final : public NodeBase
	{
	public:
		FloatController(nif::File& file);
		FloatController(nif::File& file, std::shared_ptr<nif::NiFloatInterpolator>&& iplr, std::shared_ptr<nif::NiFloatData>&& data);
		~FloatController();

		virtual nif::NiFloatInterpolator& object() override;

		IObservable<IProperty<unsigned short>>& flags() { return m_flags; }
		IObservable<IProperty<float>>& frequency() { return m_frequency; }
		IObservable<IProperty<float>>& phase() { return m_phase; }
		IObservable<IProperty<float>>& startTime() { return *m_startTime; }
		IObservable<IProperty<float>>& stopTime() { return *m_stopTime; }

	private:
		void openKeyEditor();

	public:
		constexpr static const char* TARGET = "Target";

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 160.0f;

	private:
		//Or should this be baseline nif?
		class FlagsProperty final : public LocalProperty<unsigned short>
		{
		public:
			FlagsProperty() : 
				m_animType(*this), m_cycleType(*this), m_active(*this), m_playBackwards(*this) 
			{}

			IProperty<bool>& animType() { return m_animType; }
			IProperty<unsigned short>& cycleType() { return m_cycleType; }
			IProperty<bool>& active() { return m_active; }
			IProperty<bool>& playBackwards() { return m_playBackwards; }

		private:
			BitsetProperty<bool, unsigned short, 0, 1> m_animType;
			BitsetProperty<unsigned short, unsigned short, 1, 2> m_cycleType;
			BitsetProperty<bool, unsigned short, 3, 1> m_active;
			BitsetProperty<bool, unsigned short, 4, 1> m_playBackwards;
			//BitsetProperty<bool, unsigned short, 5, 1> m_managerControlled;
			//BitsetProperty<bool, unsigned short, 6, 1> m_unknown;
		};
		FlagsProperty m_flags;
		LocalProperty<float> m_frequency;
		LocalProperty<float> m_phase;
		std::shared_ptr<LocalProperty<float>> m_startTime;
		std::shared_ptr<LocalProperty<float>> m_stopTime;

		//We are moving away from the previous design of letting NodeBase own all objects,
		//which seems to be causing more problems than it solves.
		//We'll need a major refactor of NodeBase (and all existing nodes) to complete it,
		//but this is the first step.
		std::shared_ptr<nif::NiFloatData> m_data;

		class TargetField;
	};
}