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

#pragma once
#include "Controls.h"
#include "AVObject.h"

namespace node
{
	class NodeShared : public AVObject
	{
	protected:
		NodeShared(std::unique_ptr<nif::NiNode>&& obj);

	public:
		virtual ~NodeShared() = default;
		virtual nif::NiNode& object() override;

	public:
		constexpr static const char* CHILDREN = "Children";

	protected:
		class ChildField;
	};

	class Node final : public NodeShared
	{
	public:
		Node();
		Node(std::unique_ptr<nif::NiNode>&& obj);

	};

	class Root final : public NodeShared
	{
	public:
		Root();
		Root(std::unique_ptr<nif::NiNode>&& obj);
	};
}