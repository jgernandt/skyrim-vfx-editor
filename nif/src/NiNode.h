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
#include "NiAVObject.h"
#include "NiExtraData.h"

namespace nif
{
	class NiNode : public NiAVObject
	{
	public:
		NiNode();
		NiNode(native::NiNode* obj);
		NiNode(const NiNode& other) = delete;

		virtual ~NiNode() = default;

		NiNode& operator=(const NiNode&) = delete;

		native::NiNode& getNative() const;

		ISet<NiAVObject>& children() { return m_children; }

	private:
		struct NodeChildren : SetBase<NiAVObject>
		{
			NodeChildren(NiNode& super) : m_super{ super } {}

			virtual void add(const NiAVObject& obj) override;
			virtual void remove(const NiAVObject& obj) override;
			virtual bool has(const NiAVObject& obj) const override;
			virtual size_t size() const override;

			NiNode& m_super;

		} m_children;
	};

	class BSFadeNode : public NiNode
	{
	public:
		BSFadeNode();
		BSFadeNode(native::BSFadeNode* obj);
		BSFadeNode(const BSFadeNode& other) = delete;

		virtual ~BSFadeNode() = default;

		BSFadeNode& operator=(const BSFadeNode&) = delete;

		native::BSFadeNode& getNative() const;
	};
}