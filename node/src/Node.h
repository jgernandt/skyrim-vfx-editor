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
#include "AVObject.h"

namespace node
{
	using namespace nif;

	class NodeShared : public AVObject
	{
	protected:
		NodeShared(const ni_ptr<NiNode>& obj);

	public:
		virtual ~NodeShared() = default;

	public:
		constexpr static const char* CHILDREN = "Children";

	protected:
		class ChildField;

		std::unique_ptr<Field> m_children;
	};

	class Node final : public NodeShared
	{
	public:
		Node(ni_ptr<NiNode>&& obj);
		~Node();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 180.0f;
	};

	class Root final : public NodeShared
	{
	public:
		Root(ni_ptr<NiNode>&& obj);
		~Root();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 116.0f;
	};
}
