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
#include "NiObject.h"
#include "File.h"
#include "Observable.h"

namespace nif
{
	//base class for Assignable, Property, etc.
	//should always be a static member of a NiObject
	//The template arg is type of interface
	template<typename T>
	class NiObject::DataField : public IObservable<T>
	{
	private:
		/*class OurUnsubscriber final : public Unsubscriber
		{
		public:
			OurUnsubscriber(std::shared_ptr<DataField<T>>&& field, IListener<T>* lsnr) :
				m_field{ std::move(field) }, m_lsnr{ lsnr } {}

			~OurUnsubscriber() { unsub(); }

			virtual void unsub() override
			{
				if (auto field = m_field.lock()) {
					assert(m_lsnr);
					field->removeListener(*m_lsnr);
				}
			}

		private:
			std::weak_ptr<DataField<T>> m_field;
			IListener<T>* m_lsnr{ nullptr };
		};*/

	public:
		DataField(NiObject& block) : m_block{ &block } {}
		virtual ~DataField() = default;

		DataField(const DataField<T>&) = delete;
		DataField<T>& operator=(const DataField<T>&) = delete;

		DataField(DataField<T>&&) = delete;
		DataField<T>& operator=(DataField<T>&&) = delete;

		virtual void addListener(IListener<T>& l) override
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it == m_lsnrs.end())
				m_lsnrs.push_back(&l);
		}

		virtual void removeListener(IListener<T>& l) override
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it != m_lsnrs.end())
				m_lsnrs.erase(it);
		}

		/* [[nodiscard]] virtual std::unique_ptr<Unsubscriber> addListener(IListener<T>& l) override
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it == m_lsnrs.end()) {
				m_lsnrs.push_back(&l);
				assert(m_block && m_block->m_file);
				return std::make_unique<OurUnsubscriber>(std::shared_ptr<DataField<T>>(m_block->m_file->get<NiObject>(m_block->m_ptr), this), &l);
			}
			else
				return std::unique_ptr<Unsubscriber>();
		}

	private:
		void removeListener(IListener<T>& l)
		{
			if (auto it = std::find(m_lsnrs.begin(), m_lsnrs.end(), &l); it != m_lsnrs.end())
				m_lsnrs.erase(it);
		}*/

	protected:
		NiObject* m_block;
		std::vector<IListener<T>*> m_lsnrs;
	};
}