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
#include <cassert>
#include "Observable.h"
#include "DataField.h"

template<typename T>
class ISequence
{
public:
	virtual ~ISequence() = default;

	//index -1 (cast to unsigned) indicates end here
	virtual size_t insert(size_t, const T&) = 0;
	virtual size_t erase(size_t) = 0;
	virtual size_t find(const T&) const = 0;
	virtual size_t size() const = 0;
};

template<typename T>
using OSequence = IObservable<ISequence<T>>;

template<typename T>
class IListener<ISequence<T>>
{
public:
	virtual ~IListener() = default;

	virtual void onInsert(size_t) {}
	virtual void onErase(size_t) {}
};

namespace nif
{
	template<typename T>
	using Sequence = NiObject::DataField<ISequence<T>>;

	template<typename T>
	using SequenceListener = IListener<ISequence<T>>;

	template<typename T, typename BlockType>
	class SequenceBase : public Sequence<T>
	{
	public:
		SequenceBase(BlockType& block) : Sequence<T>(block) {}
		virtual ~SequenceBase() = default;

	protected:
		typename BlockType::native_type* nativePtr() const
		{
			assert(this->m_block);
			return &static_cast<BlockType*>(this->m_block)->getNative();
		}
		typename BlockType& block() const
		{
			assert(this->m_block);
			return *static_cast<BlockType*>(this->m_block);
		}

		void notifyInsert(size_t pos) const
		{
			for (SequenceListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onInsert(pos);
			}
		}
		void notifyErase(size_t pos) const
		{
			for (SequenceListener<T>* l : this->m_lsnrs) {
				assert(l);
				l->onErase(pos);
			}
		}
	};
}
