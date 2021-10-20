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

#include "pch.h"
#include "Graph.h"

//for ImVec operators
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

constexpr gui::MouseButton imguiToGuiButton(int button)
{
	switch (button) {
	case ImGuiButtonFlags_MouseButtonLeft:
		return gui::MouseButton::LEFT;
	case ImGuiButtonFlags_MouseButtonMiddle:
		return gui::MouseButton::MIDDLE;
	case ImGuiButtonFlags_MouseButtonRight:
		return gui::MouseButton::RIGHT;
	default:
		return gui::MouseButton::NONE;
	}
}

//This is made specifically for the scale mod. We'll make it more general as we go.
void gui::PlotArea::frame(FrameDrawer& fd)
{
	using namespace ImGui;

	//Determine the size of the plot area.
	//If size is set to auto, make it a square that fits the allowed width (including labels).
	//If height is set to auto, make it a square given whatever width is set (auto or not).
	//(Weird things may happen if size is set, but too small to fit the labels. Let's ignore this fact, and it will go away.)
	float yLabelWidth = CalcTextSize("00").x;
	float xLabelHeight = GetFontSize();
	ImVec2 itemSize;
	ImVec2 plotSize;
	itemSize[0] = m_size[0] > 0.0f ? m_size[0] : CalcItemWidth();
	plotSize[0] = std::max(itemSize[0] - yLabelWidth - GetStyle().ItemInnerSpacing.x, 0.0f);
	if (m_size[1] <= 0.0f) {
		plotSize[1] = plotSize[0];
		itemSize[1] = plotSize[1] + xLabelHeight + GetStyle().ItemInnerSpacing.y;
	}
	else {
		itemSize[1] = m_size[1];
		plotSize[1] = std::max(itemSize[1] - xLabelHeight - GetStyle().ItemInnerSpacing.y, 0.0f);
	}

	//Screen space position
	ImVec2 TLSS = GetCursorScreenPos() + ImVec2{ yLabelWidth + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() };
	ImVec2 BRSS = TLSS + plotSize;

	//Window space position
	ImVec2 TLWS = GetCursorPos() + ImVec2{ yLabelWidth + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() };
	ImVec2 BRWS = TLWS + plotSize;

	m_scale = { (BRSS.x - TLSS.x) / (m_xlim[1] - m_xlim[0]),//r - l
		(TLSS.y - BRSS.y) / (m_ylim[1] - m_ylim[0]) };		//t - b
	m_translation = { TLSS.x - m_xlim[0] * m_scale[0],		//l
		BRSS.y - m_ylim[0] * m_scale[1] };					//b

	//Graph area
	ImDrawList* drawList = GetWindowDrawList();
	drawList->AddRectFilled(TLSS, BRSS, ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_FrameBg]));

	//Major grid lines
	for (float f = TLSS.x + m_scale[0] * std::fmod(m_xlim[0], m_majorGrid[0]);
		f <= BRSS.x;
		f += m_scale[0] * m_majorGrid[0])
		drawList->AddLine(
			ImVec2(f, TLSS.y),
			ImVec2(f, BRSS.y),
			ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Border]),
			m_majorGridWidth);
	for (float f = BRSS.y + m_scale[1] * std::fmod(m_ylim[0], m_majorGrid[1]);
		f >= TLSS.y;
		f += m_scale[1] * m_majorGrid[1])
		drawList->AddLine(
			ImVec2(TLSS.x, f),
			ImVec2(BRSS.x, f),
			ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Border]),
			m_majorGridWidth);

	//We draw the controls first, or the plot area steals focus (or so it seems).
	//This might require another order if we want the plot area to capture other mouse input as well.
	Drawer d;
	d.setTargetLayer(Layer::WINDOW);
	d.pushTransform(m_translation, m_scale);
	d.pushClipArea(m_xlim, m_ylim);
	for (auto&& curve : m_curves) {
		assert(curve);
		curve->draw(d);
	}

	Composite::frame(fd);

	//Scrolling
	SetCursorScreenPos(TLSS);
	InvisibleButton("graph_area", plotSize, ImGuiButtonFlags_MouseButtonMiddle);
	SetItemUsingMouseWheel();
	if (IsItemHovered() && GetIO().MouseWheel != 0.0f)
		m_ylim[1] = std::max(m_ylim[1] - GetIO().MouseWheel, 1.0f);

	//y labels
	char buf[8];
	snprintf(buf, sizeof(buf), "%.0f", m_ylim[1]);
	SetCursorPos(TLWS - ImVec2{ CalcTextSize(buf).x + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() });
	Text(buf);
	snprintf(buf, sizeof(buf), "%.0f", m_ylim[0]);
	SetCursorPos(ImVec2{ TLWS.x - CalcTextSize(buf).x - GetStyle().ItemInnerSpacing.x, BRWS.y - 0.5f * GetFontSize() });
	Text(buf);

	//x labels
	SetCursorPos(ImVec2{ TLWS.x, BRWS.y + GetStyle().ItemInnerSpacing.y });
	Text("Birth");
	SetCursorPos({ BRWS.x - CalcTextSize("Death").x, BRWS.y + GetStyle().ItemSpacing.y });
	Text("Death");
}

void gui::PlotArea::addCurve(std::unique_ptr<Curve>&& curve)
{
	m_curves.push_back(std::move(curve));
}

void gui::PlotArea::removeCurve(Curve* curve)
{
	auto up = [curve](const std::unique_ptr<Curve>& p) { return p.get() == curve; };
	if (auto it = std::find_if(m_curves.begin(), m_curves.end(), up); it != m_curves.end())
		m_curves.erase(it);
}

gui::Floats<2> gui::PlotArea::localToScreen(const Floats<2>& p) const
{
	return { m_translation[0] + m_scale[0] * p[0], m_translation[1] + m_scale[1] * p[1] };
	//return { (p[0] - m_bl[0]) * m_scaleS2L[0] + m_screenBL[0], (p[1] - m_bl[1]) * m_scaleS2L[1] + m_screenBL[1] };
}


void gui::SimpleCurve::draw(Drawer& d) const
{
	d.begin();
	for (int i = 0; i < static_cast<int>(m_points.size()) - 1; i++)
		d.line(m_points[i], m_points[i + 1], { 0.0f, 0.0f, 0.0f, 1.0f });
	d.end();
}

void gui::SimpleHandles::frame(FrameDrawer& fd)
{
	//NOTE: with the FrameDrawer, we no longer need to know about the plot area. 
	//It can send its transform down to us instead.

	using namespace ImGui;

	float buttonSize = m_size + 10.0f;

	ImVec2 pos = GetCursorPos();//we should probably reset this when we're done

	for (size_t i = 0; i < m_points.size(); i++) {
		//convert to screen space
		ImVec2 p = gui_type_conversion<ImVec2>::from(m_area.localToScreen(m_points[i]));

		//limit test
		if (Floats<4> lims = m_area.getLimits();
			m_points[i][0] >= lims[0] &&
			m_points[i][0] <= lims[1] &&
			m_points[i][1] >= lims[2] &&
			m_points[i][1] <= lims[3])
		{
			//Apparently, there are Push/PopID functions to replace this
			SetCursorScreenPos({ p.x - buttonSize / 2.0f, p.y - buttonSize / 2.0f });
			char buf[16];
			snprintf(buf, sizeof(buf), "##point%d", static_cast<int>(i));
			InvisibleButton(buf, { buttonSize, buttonSize }, ImGuiButtonFlags_MouseButtonLeft);

			bool highlight = false;

			if (IsItemActivated())
				onClick(i, imguiToGuiButton(ImGuiButtonFlags_MouseButtonLeft));

			if (IsItemActive()) {
				highlight = true;
				float x;
				float y;
				if (GetIO().MouseDelta.x != 0.0f)
					x = std::max(std::min(m_points[i][0] + GetIO().MouseDelta.x / m_area.getScale()[0], lims[1]), lims[0]);
				else
					x = m_points[i][0];
				if (GetIO().MouseDelta.y != 0.0f)
					y = std::max(std::min(m_points[i][1] + GetIO().MouseDelta.y / m_area.getScale()[1], lims[3]), lims[2]);
				else
					y = m_points[i][1];

				if (x != m_points[i][0] || y != m_points[i][1])
					onMove(i, { x, y });
			}

			if (IsItemDeactivated())
				onRelease(i, imguiToGuiButton(ImGuiButtonFlags_MouseButtonLeft));

			if (!highlight && IsItemHovered())
				highlight = true;

			ImU32 col = highlight ? ColorConvertFloat4ToU32({ 1.0f, 1.0f, 1.0f, 1.0f }) : ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Text]);

			GetWindowDrawList()->AddRectFilled(
				{ p.x - m_size / 2.0f, p.y - m_size / 2.0f },
				{ p.x + m_size / 2.0f, p.y + m_size / 2.0f },
				col);
		}
	}

	SetCursorPos(pos);
}
