//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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

#include "pch.h"
#include "Item.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"//for ImVec operators

//Redo this once we swap to Eigen::Vectors

gui::Item::Item(std::unique_ptr<LayoutOperator> layout) : m_layout{ std::move(layout) }
{
	if (!m_layout)
		m_layout = std::make_unique<LeftAlign>();

	//We are a bit inconsistent with what 0 size means
	m_sizeHint = { -1.0f, -1.0f };
}

void gui::Item::frame()
{
	using namespace ImGui;

	std::vector<ImVec2> sizes(getChildren().size());
	int i = 0;
	for (auto&& c : getChildren()) {
		assert(c);
		sizes[i++] = gui_type_conversion<ImVec2>::from(c->getSizeHint());
	}

	assert(m_layout);
	std::vector<ImRect> layout = m_layout->layout(
		sizes, 
		{ m_size[0] <= 0.0f ? CalcItemWidth() : m_size[0], GetFrameHeight() });
	assert(layout.size() == getChildren().size());

	ImVec2 origin = GetCursorPos();

	for (int i = 0; i < getChildren().size(); i++) {
		SetCursorPos(origin + layout[i].GetTL());
		//SetNextItemWidth(layout[i].GetWidth());
		PushItemWidth(layout[i].GetWidth());
		assert(getChildren()[i]);
		getChildren()[i]->frame();
		PopItemWidth();
	}

	//We need this in case the last component does not place an imgui item
	SetCursorPos(origin + ImVec2(0.0f, GetFrameHeight() + GetStyle().ItemSpacing.y));
}

gui::Floats<2> gui::Item::getSizeHint() const
{
	return { m_sizeHint[0], ImGui::GetFrameHeight() };
}

std::vector<float> gui::LayoutBase::calculateWidths(const std::vector<ImVec2>& sizes, float space) const
{
	using namespace ImGui;

	//check the required width of each component
	std::vector<float> widths(sizes.size());
	std::vector<int> adaptive;//to hold indices to negative widths
	float total = 0.0f;
	float totalAdaptive = 0.0f;
	for (int i = 0; i < widths.size(); i++) {
		widths[i] = sizes[i].x;
		if (widths[i] < 0.0f) {
			adaptive.push_back(i);
			totalAdaptive += widths[i];
		}
		else
			total += widths[i];
	}

	//determine the size of any adaptive-width component
	if (!adaptive.empty()) {
		total += (widths.size() - 1) * GetStyle().ItemInnerSpacing.x;
		float adaptiveWidthUnit = (space - total) / totalAdaptive;
		for (int i : adaptive)
			widths[i] *= adaptiveWidthUnit;
		//total = space;
	}

	return std::move(widths);
}

std::vector<ImRect> gui::LeftAlign::layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const
{
	using namespace ImGui;

	std::vector<ImRect> ret(sizes.size());

	if (!sizes.empty()) {

		//---this section is always the same, right?

		std::vector<float> widths = calculateWidths(sizes, area.x);
		float centerHeight;
		if (area.y > 0.0f)
			centerHeight = area.y / 2.0f;
		else {
			float maxHeight = 0.0f;
			for (const ImVec2& s : sizes)
				if (s.y > maxHeight)
					maxHeight = s.y;
			centerHeight = maxHeight / 2.0f;
		}
		//assert(centerHeight > 0.0f);

		//---

		for (int i = 0; i < ret.size(); i++) {
			if (i > 0)
				ret[i].Min.x = ret[i - 1].Max.x + GetStyle().ItemInnerSpacing.x;
			else
				ret[i].Min.x = 0.0f;
			ret[i].Max.x = ret[i].Min.x + widths[i];
			ret[i].Min.y = centerHeight - sizes[i].y / 2.0f;
			ret[i].Max.y = ret[i].Min.y + sizes[i].y;

		}
	}

	return std::move(ret);
}

std::vector<ImRect> gui::MarginAlign::layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const
{
	using namespace ImGui;

	std::vector<ImRect> ret(sizes.size());

	if (!sizes.empty()) {
		std::vector<float> widths = calculateWidths(sizes, area.x);
		float centerHeight;
		if (area.y > 0.0f)
			centerHeight = area.y / 2.0f;
		else {
			float maxHeight = 0.0f;
			for (const ImVec2& s : sizes)
				if (s.y > maxHeight)
					maxHeight = s.y;
			centerHeight = maxHeight / 2.0f;
		}

		//sum the widths and spread the spacing evenly
		float totalWidth = 0.0f;
		for (float w : widths)
			totalWidth += w;
		float spacing = (area.x - totalWidth) / (sizes.size() - 1);

		for (int i = 0; i < ret.size(); i++) {
			if (i > 0)
				ret[i].Min.x = ret[i - 1].Max.x + spacing;
			else
				ret[i].Min.x = 0.0f;
			ret[i].Max.x = ret[i].Min.x + widths[i];
			ret[i].Min.y = centerHeight - sizes[i].y / 2.0f;
			ret[i].Max.y = ret[i].Min.y + sizes[i].y;
		}
	}

	return std::move(ret);
}

std::vector<ImRect> gui::RightAlign::layout(const std::vector<ImVec2>& sizes, const ImVec2& area) const
{
	using namespace ImGui;

	std::vector<ImRect> ret(sizes.size());

	if (!sizes.empty()) {
		std::vector<float> widths = calculateWidths(sizes, area.x);
		float centerHeight;
		if (area.y > 0.0f)
			centerHeight = area.y / 2.0f;
		else {
			float maxHeight = 0.0f;
			for (const ImVec2& s : sizes)
				if (s.y > maxHeight)
					maxHeight = s.y;
			centerHeight = maxHeight / 2.0f;
		}
		//assert(centerHeight > 0.0f);

		for (size_t i = ret.size(); i > 0;) {
			i--;

			if (i == ret.size() - 1)
				ret[i].Max.x = area.x;
			else
				ret[i].Max.x = ret[i + 1].Min.x - GetStyle().ItemInnerSpacing.x;

			ret[i].Min.x = ret[i].Max.x - widths[i];

			ret[i].Min.y = centerHeight - sizes[i].y / 2.0f;
			ret[i].Max.y = ret[i].Min.y + sizes[i].y;

		}
	}

	return std::move(ret);
}
