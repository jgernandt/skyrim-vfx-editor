//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "NiNode.h"

nif::NiNode::NiNode() : NiNode(new Niflib::NiNode)
{
	assert(getNative().GetType().IsSameType(Niflib::NiNode::TYPE));

	static int count = 0;
	getNative().SetName("Node" + std::to_string(++count));
	getNative().SetFlags(14U);
}

nif::NiNode::NiNode(native::NiNode* obj) : NiAVObject(obj), m_children(*this) {}

nif::native::NiNode& nif::NiNode::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::NiNode::TYPE));
	return static_cast<native::NiNode&>(*m_ptr);
}

nif::BSFadeNode::BSFadeNode() : BSFadeNode(new Niflib::BSFadeNode)
{
	assert(getNative().GetType().IsSameType(Niflib::BSFadeNode::TYPE));

	static int count = 0;
	getNative().SetName("FadeNode" + std::to_string(++count));
	getNative().SetFlags(14U);
}

nif::BSFadeNode::BSFadeNode(native::BSFadeNode* obj) : NiNode(obj) {}

nif::native::BSFadeNode& nif::BSFadeNode::getNative() const
{
	assert(m_ptr && m_ptr->GetType().IsDerivedType(Niflib::BSFadeNode::TYPE));
	return static_cast<native::BSFadeNode&>(*m_ptr);
}

void nif::NiNode::NodeChildren::add(const NiAVObject& obj)
{
	if (!has(obj)) {
		assert(!obj.getNative().GetParent());//Should be cleared before calling us
		m_super.getNative().AddChild(&obj.getNative());
		notifyAdd(obj);
	}
}

void nif::NiNode::NodeChildren::remove(const NiAVObject& obj)
{
	if (has(obj)) {
		m_super.getNative().RemoveChild(&obj.getNative());
		notifyRemove(obj);
	}
}

bool nif::NiNode::NodeChildren::has(const NiAVObject& obj) const
{
	for (auto&& child : m_super.getNative().GetChildren())
		if (child == &obj.getNative())
			return true;

	return false;
}

size_t nif::NiNode::NodeChildren::size() const
{
	return m_super.getNative().GetChildren().size();
}
