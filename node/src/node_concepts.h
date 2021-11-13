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
#include "data_model.h"

namespace node
{
	//A wrapper that allows reserving positions at the beginning and end of a sequence
	template<typename T>
	class ReservableSequence final : public OSequence<T>
	{
	public:
		ReservableSequence(IObservable<ISequence<T>>& seq) : m_seq{ seq }
		{
			m_lim[1] = m_seq.size();
		}

		virtual size_t insert(size_t pos, const T& obj) final override
		{
			size_t size = m_seq.size();

			//restrict pos to [lim[0], lim[1]]
			pos = std::min(std::max(pos, m_lim[0]), m_lim[1]);
			size_t result = m_seq.insert(pos, obj);

			//If an actual insertion took place, increase upper limit
			if (m_seq.size() > size)
				m_lim[1]++;

			return result;
		}
		virtual size_t erase(size_t pos) final override
		{
			size_t result = m_seq.erase(pos);

			//If erase took place at or above lim[1], no change. 
			//If below lim[1], decrease lim[1].
			//If below lim[0], decrease lim[0].
			if (result < m_lim[1])
				m_lim[1]--;
			if (result < m_lim[0])
				m_lim[0]--;

			return result;
		}
		virtual size_t find(const T& obj) const final override { return m_seq.find(obj); }
		virtual size_t size() const override { return m_seq.size(); }

		virtual void addListener(nif::SequenceListener<T>& l) final override { m_seq.addListener(l); }
		virtual void removeListener(nif::SequenceListener<T>& l) final override { m_seq.removeListener(l); }

		size_t reserve(size_t pos, const T& obj)
		{
			assert(pos == 0 || pos == -1);//only support reserving the beginning or the end

			size_t result = insert(pos, obj);

			if (pos == 0) {
				assert(result == m_lim[0]);
				m_lim[0]++;
			}
			else if (pos == -1) {
				m_lim[1]--;
				assert(result == m_lim[1]);
			}

			return result;
		}

	private:
		OSequence<T>& m_seq;
		size_t m_lim[2]{ 0, 0 };//the min and max pos allowed for inserts
	};

	template<typename T>//template only to distinguish types
	class IController
	{
	public:
		virtual ~IController() = default;

		virtual OProperty<unsigned short>& flags() = 0;
		virtual OProperty<float>& frequency() = 0;
		virtual OProperty<float>& phase() = 0;
		virtual OProperty<float>& startTime() = 0;
		virtual OProperty<float>& stopTime() = 0;
	}; 

	template<typename T>
	class LocalProperty : public ObservableBase<IProperty<T>>
	{
	public:
		LocalProperty(const T& def = T()) : m_val{ def } {}
		virtual ~LocalProperty() = default;

		virtual T get() const final override { return m_val; }
		virtual void set(const T& val) final override 
		{
			if (val != m_val) {
				m_val = val;
				for (auto&& l : this->getListeners()) {
					assert(l);
					l->onSet(val);
				}
			}
		}

	private:
		T m_val;
	};

	//May need a conversion before setting
	template<typename T, typename TargetType = T, template<typename> typename ConverterType = nif::NifConverter>
	class SetterListener : public nif::PropertyListener<T>
	{
	public:
		SetterListener(IProperty<TargetType>& prop) : m_ifc{ prop } {}
		virtual ~SetterListener() = default;

		virtual void onSet(const T& t) override 
		{ 
			m_ifc.set(util::type_conversion<TargetType, ConverterType<TargetType>>::from(t));
		}

	private:
		IProperty<TargetType>& m_ifc;
	};
}