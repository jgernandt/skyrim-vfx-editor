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
#include "Composition.h"

struct ImRect;
struct ImVec2;

namespace gui
{
	class LayoutOperator
	{
	public:
		virtual ~LayoutOperator() {}
		//virtual std::vector<ImRect> layout(const std::vector<ComponentPtr>& comps, const ImVec2& area) const = 0;
		virtual std::vector<ImRect> layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const = 0;
	};

	class Item final : public Composite
	{
	public:
		Item(std::unique_ptr<LayoutOperator> layout = std::unique_ptr<LayoutOperator>());
		virtual void frame() override;
		virtual Floats<2> getSizeHint() const override;

	private:
		std::unique_ptr<LayoutOperator> m_layout;
	};

	class LayoutBase : public LayoutOperator
	{
	public:
		virtual ~LayoutBase() {}

	protected:
		std::vector<float> calculateWidths(const std::vector<ImVec2>& sizes, float space) const;
	};

	class LeftAlign final : public LayoutBase
	{
	public:
		//virtual std::vector<ImRect> layout(const std::vector<ComponentPtr>& comps, const ImVec2& area) const override;
		virtual std::vector<ImRect> layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const override;
	};

	class RightAlign final : public LayoutBase
	{
	public:
		virtual std::vector<ImRect> layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const override;
	};

	class MarginAlign final : public LayoutBase
	{
	public:
		//virtual std::vector<ImRect> layout(const std::vector<ComponentPtr>& comps, const ImVec2& area) const override;
		virtual std::vector<ImRect> layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const override;
	};
}