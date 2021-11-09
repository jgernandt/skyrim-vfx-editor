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
#include "Drawer.h"

ImDrawList* gui::getDrawList(gui::Layer l)
{
	switch (l) {
	case gui::Layer::BACKGROUND:
		return ImGui::GetBackgroundDrawList();
	case gui::Layer::FOREGROUND:
		return ImGui::GetForegroundDrawList();
	case gui::Layer::WINDOW:
		return ImGui::GetWindowDrawList();
	default:
		return nullptr;
	}
}

ImU32 ColRGBAToInt(const gui::ColRGBA& col)
{
	return IM_COL32(static_cast<unsigned int>(col[0] * 255.0f),
		static_cast<unsigned int>(col[1] * 255.0f),
		static_cast<unsigned int>(col[2] * 255.0f),
		static_cast<unsigned int>(col[3] * 255.0f));
}

static ImVec2 transform(const gui::Floats<2>& p, const std::stack<gui::Floats<4>>& T)
{
	if (T.empty())
		return { p[0], p[1] };
	else
		return { T.top()[0] + T.top()[2] * p[0], T.top()[1] + T.top()[3] * p[1] };
}

gui::Drawer::~Drawer()
{
	if (m_drawing)
		end();
}

void gui::Drawer::begin()
{
	if (!m_drawing) {
		m_drawing = true;
		if (!m_clipArea.empty())
			getDrawList(m_layer)->PushClipRect(
				{ std::min(m_clipArea.top()[0], m_clipArea.top()[2]), std::min(m_clipArea.top()[1], m_clipArea.top()[3]) },
				{ std::max(m_clipArea.top()[0], m_clipArea.top()[2]), std::max(m_clipArea.top()[1], m_clipArea.top()[3]) },
				true);
	}
}

void gui::Drawer::end()
{
	if (m_drawing) {
		m_drawing = false;
		if (!m_clipArea.empty())
			getDrawList(m_layer)->PopClipRect();
	}
}

/*void gui::Drawer::pushClipArea(float x1, float y1, float x2, float y2)
{
	assert(!m_drawing);

	//We apply our currently pushed transform to the passed points, so that the clip area is always expressed in global coords
	Floats<4> global;
	if (m_transform.empty())
		global = { x1, y1, x2, y2 };
	else
		global = {
			m_transform.top()[0] + m_transform.top()[2] * x1,
			m_transform.top()[1] + m_transform.top()[3] * y1,
			m_transform.top()[0] + m_transform.top()[2] * x2,
			m_transform.top()[1] + m_transform.top()[3] * y2 };

	//Intersection test
	if (!m_clipArea.empty()) {
		if (global[0] > m_clipArea.top()[0] && global[0] > m_clipArea.top()[2])
			global[0] = std::max(m_clipArea.top()[0], m_clipArea.top()[2]);
		else if (global[0] < m_clipArea.top()[0] && global[0] < m_clipArea.top()[2])
			global[0] = std::min(m_clipArea.top()[0], m_clipArea.top()[2]);

		if (global[1] > m_clipArea.top()[1] && global[1] > m_clipArea.top()[3])
			global[1] = std::max(m_clipArea.top()[1], m_clipArea.top()[3]);
		else if (global[1] < m_clipArea.top()[1] && global[1] < m_clipArea.top()[3])
			global[1] = std::min(m_clipArea.top()[1], m_clipArea.top()[3]);

		if (global[2] > m_clipArea.top()[0] && global[2] > m_clipArea.top()[2])
			global[2] = std::max(m_clipArea.top()[0], m_clipArea.top()[2]);
		else if (global[2] < m_clipArea.top()[0] && global[2] < m_clipArea.top()[2])
			global[2] = std::min(m_clipArea.top()[0], m_clipArea.top()[2]);

		if (global[3] > m_clipArea.top()[1] && global[3] > m_clipArea.top()[3])
			global[3] = std::max(m_clipArea.top()[1], m_clipArea.top()[3]);
		else if (global[3] < m_clipArea.top()[1] && global[3] < m_clipArea.top()[3])
			global[3] = std::min(m_clipArea.top()[1], m_clipArea.top()[3]);
	}

	m_clipArea.push(global);
}*/

void gui::Drawer::pushClipArea(const Floats<2>& p1, const Floats<2>& p2)
{
	assert(!m_drawing);

	//We apply our currently pushed transform to the passed points, so that the clip area is always expressed in global coords
	Floats<4> global;
	if (m_transform.empty())
		global = { p1[0], p1[1], p2[0], p2[1] };
	else
		global = {
			m_transform.top()[0] + m_transform.top()[2] * p1[0],
			m_transform.top()[1] + m_transform.top()[3] * p1[1],
			m_transform.top()[0] + m_transform.top()[2] * p2[0],
			m_transform.top()[1] + m_transform.top()[3] * p2[1] };

	//Intersection test
	if (!m_clipArea.empty()) {
		if (global[0] > m_clipArea.top()[0] && global[0] > m_clipArea.top()[2])
			global[0] = std::max(m_clipArea.top()[0], m_clipArea.top()[2]);
		else if (global[0] < m_clipArea.top()[0] && global[0] < m_clipArea.top()[2])
			global[0] = std::min(m_clipArea.top()[0], m_clipArea.top()[2]);

		if (global[1] > m_clipArea.top()[1] && global[1] > m_clipArea.top()[3])
			global[1] = std::max(m_clipArea.top()[1], m_clipArea.top()[3]);
		else if (global[1] < m_clipArea.top()[1] && global[1] < m_clipArea.top()[3])
			global[1] = std::min(m_clipArea.top()[1], m_clipArea.top()[3]);

		if (global[2] > m_clipArea.top()[0] && global[2] > m_clipArea.top()[2])
			global[2] = std::max(m_clipArea.top()[0], m_clipArea.top()[2]);
		else if (global[2] < m_clipArea.top()[0] && global[2] < m_clipArea.top()[2])
			global[2] = std::min(m_clipArea.top()[0], m_clipArea.top()[2]);

		if (global[3] > m_clipArea.top()[1] && global[3] > m_clipArea.top()[3])
			global[3] = std::max(m_clipArea.top()[1], m_clipArea.top()[3]);
		else if (global[3] < m_clipArea.top()[1] && global[3] < m_clipArea.top()[3])
			global[3] = std::min(m_clipArea.top()[1], m_clipArea.top()[3]);
	}

	m_clipArea.push(global);
}

void gui::Drawer::popClipArea()
{
	assert(!m_drawing);
	if (!m_clipArea.empty())
		m_clipArea.pop();
}

void gui::Drawer::pushTransform(float tx, float ty, float sx, float sy)
{
	assert(!m_drawing);

	if (m_transform.empty())
		m_transform.push({ tx, ty, sx, sy });
	else
		m_transform.push({ 
		m_transform.top()[0] + m_transform.top()[2] * tx,
		m_transform.top()[1] + m_transform.top()[3] * ty,
		m_transform.top()[2] * sx,
		m_transform.top()[3] * sy });
}

void gui::Drawer::pushTransform(const Floats<2>& translation, const Floats<2>& scale)
{
	pushTransform(translation[0], translation[1], scale[0], scale[1]);
}

void gui::Drawer::popTransform()
{
	assert(!m_drawing);
	if (!m_transform.empty())
		m_transform.pop();
}

void gui::Drawer::line(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col)
{
	assert(m_drawing);

	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		drawList->AddLine(transform(p1, m_transform), transform(p2, m_transform), ColRGBAToInt(col));
}

void gui::Drawer::rectangle(const Floats<2>& p1, const Floats<2>& p2, const ColRGBA& col)
{
	assert(m_drawing);

	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		drawList->AddRectFilled(transform(p1, m_transform), transform(p2, m_transform), ColRGBAToInt(col));
}

void gui::Drawer::rectangleGradient(const Floats<2>& p1, const Floats<2>& p2, 
	const ColRGBA& tl, const ColRGBA& tr, const ColRGBA& bl, const ColRGBA& br)
{
	assert(m_drawing);

	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		drawList->AddRectFilledMultiColor(
			transform(p1, m_transform), transform(p2, m_transform), 
			ColRGBAToInt(tl), ColRGBAToInt(tr), ColRGBAToInt(br), ColRGBAToInt(bl));
}

void gui::Drawer::triangle(const Floats<2>& p1, const Floats<2>& p2, const Floats<2>& p3, const ColRGBA& col)
{
	assert(m_drawing);

	ImDrawList* drawList = getDrawList(m_layer);
	if (drawList)
		drawList->AddTriangleFilled(transform(p1, m_transform), transform(p2, m_transform), transform(p3, m_transform), ColRGBAToInt(col));
}
