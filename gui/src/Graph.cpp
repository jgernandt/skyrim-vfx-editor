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
#include "CallWrapper.h"

//for ImVec operators
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

constexpr size_t AREA_AXES_INDEX = 0;
constexpr size_t AREA_MIN_SIZE = 1;

constexpr size_t PLOT_PLOTAREA_INDEX = 0;
constexpr size_t PLOT_XLABEL_INDEX = 1;
constexpr size_t PLOT_YLABEL_INDEX = 2;
constexpr size_t PLOT_MIN_SIZE = 3;

void gui::Axes::frame(FrameDrawer& fd)
{
	//We might want to do this in global space instead (rounded to nearest pixel),
	//to improve consistency on scaling

	assert(getParent());
	Floats<2> parentSize = getParent()->getSize();
	Floats<2> sizeTol = parentSize * 1.0e-3f;

	Floats<2> majorGrid = m_scale.abs();
	Floats<2> minorGrid = 0.25f * m_scale.abs();
	ImU32 axisCol = 0xff000000;
	ImU32 majorCol = 0xff7f7f7f;
	ImU32 minorCol = 0xffbfbfbf;

	//Minor grid
	float x = std::fmodf(m_translation[0], minorGrid[0]);
	if (x + sizeTol[0] < 0.0f)
		x += minorGrid[0];
	for (; x <= parentSize[0] + sizeTol[0]; x += minorGrid[0])
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ x, 0.0f })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ x, parentSize[1] })),
			minorCol);
	float y = std::fmodf(m_translation[1], minorGrid[1]);
	if (y + sizeTol[1] < 0.0f)
		y += minorGrid[1];
	for (; y <= parentSize[1] + sizeTol[1]; y += minorGrid[1])
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ 0.0f, y })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ parentSize[0], y })),
			minorCol);

	//Major grid
	x = std::fmodf(m_translation[0], majorGrid[0]);
	if (x + sizeTol[0] < 0.0f)
		x += majorGrid[0];
	for (; x <= parentSize[0] + sizeTol[0]; x += majorGrid[0])
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ x, 0.0f })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ x, parentSize[1] })),
			majorCol);
	y = std::fmodf(m_translation[1], majorGrid[1]);
	if (y + sizeTol[1] < 0.0f)
		y += majorGrid[1];
	for (; y <= parentSize[1] + sizeTol[1]; y += majorGrid[1])
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ 0.0f, y })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ parentSize[0], y })),
			majorCol);

	//Main axes
	if (m_translation[0] >= 0.0f && m_translation[0] <= parentSize[0] + sizeTol[0]) {
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ m_translation[0], 0.0f })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ m_translation[0], parentSize[1] })),
			axisCol);
	}
	if (m_translation[1] >= 0.0f && m_translation[1] <= parentSize[1] + sizeTol[1]) {
		ImGui::GetWindowDrawList()->AddLine(
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ 0.0f, m_translation[1] })),
			gui_type_conversion<ImVec2>::from(fd.toGlobal({ parentSize[0], m_translation[1] })),
			axisCol);
	}

	//Curves (I'm not sure we want to keep doing it this way)
	Drawer d;
	d.setTargetLayer(Layer::WINDOW);
	d.pushTransform(fd.getCurrentTranslation(), fd.getCurrentScale());
	d.pushClipArea({ 0.0f, 0.0f }, parentSize);
	d.pushTransform(m_translation, m_scale);
	for (auto&& curve : m_curves) {
		assert(curve);
		curve->draw(d);
	}

	fd.pushClipArea({ 0.0f, 0.0f }, parentSize);
	util::CallWrapper end(&FrameDrawer::popClipArea, &fd);
	Composite::frame(fd);
}

void gui::Axes::addCurve(std::unique_ptr<Curve>&& curve)
{
	m_curves.push_back(std::move(curve));
}

std::unique_ptr<gui::Curve> gui::Axes::removeCurve(Curve* curve)
{
	std::unique_ptr<gui::Curve> result;
	auto up = [curve](const std::unique_ptr<Curve>& p) { return p.get() == curve; };
	if (auto it = std::find_if(m_curves.begin(), m_curves.end(), up); it != m_curves.end()) {
		result = std::move(*it);
		m_curves.erase(it);
	}
	return result;
}

std::unique_ptr<gui::Curve> gui::Axes::removeCurve(int index)
{
	std::unique_ptr<gui::Curve> result;
	if (index >= 0 && static_cast<size_t>(index) < m_curves.size()) {
		result = std::move(m_curves[index]);
		m_curves.erase(m_curves.begin() + index);
	}
	return result;
}


gui::PlotArea::PlotArea()
{
	auto axes = newChild<Axes>();

	//This will define our axis limits
	m_size = { 1.0f, 1.0f };
	axes->setTranslation({ 0.0f, 1.0f });
	axes->setScale({ 1.0f, -1.0f });
}

//This is made specifically for the scale mod. We'll make it more general as we go.
void gui::PlotArea::frame(FrameDrawer& fd)
{
	using namespace ImGui;
	/*
	//We rely on the imgui cursor for our position. Would be nicer to have it set by some layout procedure instead.
	//This assumes we are child of a Window. Or Popup. Crap.
	// Maybe just require that a translation be set explicitly?
	//Should be enough to do this only once?
	Floats<2> scale = fd.getCurrentScale();
	//m_translation = gui_type_conversion<Floats<2>>::from(GetCursorPos()) / scale;

	//Ok: use our translation to position the plot area. As it should be done.

	//Determine the size of the plot area.
	//If size is set to auto, make it a square that fits the allowed width (including labels).
	//If height is set to auto, make it a square given whatever width is set (auto or not).
	//(Weird things may happen if size is set, but too small to fit the labels. Let's ignore this fact, and it will go away.)
	float yLabelWidth = CalcTextSize("00").x;
	float xLabelHeight = GetFontSize();
	ImVec2 itemSize;//in screen space
	ImVec2 plotSize;//in screen space
	itemSize[0] = m_size[0] > 0.0f ? m_size[0] * scale[0] * m_scale[0] : CalcItemWidth();
	plotSize[0] = std::max(itemSize[0] - yLabelWidth - GetStyle().ItemInnerSpacing.x, 0.0f);
	if (m_size[1] <= 0.0f) {
		plotSize[1] = plotSize[0];
		itemSize[1] = plotSize[1] + xLabelHeight + GetStyle().ItemInnerSpacing.y;
	}
	else {
		itemSize[1] = m_size[1] * scale[1] * m_scale[1];
		plotSize[1] = std::max(itemSize[1] - xLabelHeight - GetStyle().ItemInnerSpacing.y, 0.0f);
	}

	//Screen space position
	ImVec2 TLSS = GetCursorScreenPos() + ImVec2{ yLabelWidth + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() };
	ImVec2 BRSS = TLSS + plotSize;

	//Window space position
	ImVec2 TLWS = GetCursorPos() + ImVec2{ yLabelWidth + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() };
	ImVec2 BRWS = TLWS + plotSize;

	//We don't need to recalc!
	m_CSscale = { (BRSS.x - TLSS.x) / (m_xlim[1] - m_xlim[0]),	//r - l
		(TLSS.y - BRSS.y) / (m_ylim[1] - m_ylim[0]) };			//t - b
	m_CStranslation = { TLSS.x - m_xlim[0] * m_CSscale[0],		//l
		BRSS.y - m_ylim[0] * m_CSscale[1] };					//b

	//Graph area
	ImDrawList* drawList = GetWindowDrawList();
	drawList->AddRectFilled(TLSS, BRSS, ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_FrameBg]));

	//Major grid lines
	for (float f = TLSS.x + m_CSscale[0] * std::fmod(m_xlim[0], m_majorGrid[0]);
		f <= BRSS.x;
		f += m_CSscale[0] * m_majorGrid[0])
		drawList->AddLine(
			ImVec2(f, TLSS.y),
			ImVec2(f, BRSS.y),
			ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Border]),
			m_majorGridWidth);
	for (float f = BRSS.y + m_CSscale[1] * std::fmod(m_ylim[0], m_majorGrid[1]);
		f >= TLSS.y;
		f += m_CSscale[1] * m_majorGrid[1])
		drawList->AddLine(
			ImVec2(TLSS.x, f),
			ImVec2(BRSS.x, f),
			ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Border]),
			m_majorGridWidth);

	//We draw the controls first, or the plot area steals focus (or so it seems).
	//This might require another order if we want the plot area to capture other mouse input as well.
	Drawer d;
	d.setTargetLayer(Layer::WINDOW);
	d.pushTransform(m_CStranslation, m_CSscale);
	d.pushClipArea(m_xlim, m_ylim);
	for (auto&& curve : m_curves) {
		assert(curve);
		curve->draw(d);
	}

	Composite::frame(fd);

	//Scrolling
	//Use event handler funciton instead
	SetCursorScreenPos(TLSS);
	InvisibleButton("graph_area", plotSize, ImGuiButtonFlags_MouseButtonMiddle);
	SetItemUsingMouseWheel();
	if (IsItemHovered() && GetIO().MouseWheel != 0.0f)
		m_ylim[1] = std::max(m_ylim[1] - GetIO().MouseWheel, 1.0f);

	//y labels
	char buf[8];
	snprintf(buf, sizeof(buf), "%.0f", m_ylim[1]);
	drawList->AddText(TLSS - ImVec2{ CalcTextSize(buf).x + GetStyle().ItemInnerSpacing.x, 0.5f * GetFontSize() }, 0xff000000, buf);
	snprintf(buf, sizeof(buf), "%.0f", m_ylim[0]);
	drawList->AddText(ImVec2{ TLSS.x - CalcTextSize(buf).x - GetStyle().ItemInnerSpacing.x, BRSS.y - 0.5f * GetFontSize() }, 0xff000000, buf);

	//x labels
	if (m_xLabels.empty()) {
		//Use axis values
	}
	else {
		//Use custom labels
		for (auto&& label : m_xLabels) {
			if (label.pos >= m_xlim[0] && label.pos <= m_xlim[1]) {
				ImVec2 size = CalcTextSize(label.label.c_str());
				ImVec2 pos;
				pos.x = localToScreen({ label.pos, 0.0f })[0] - label.alignment * size.x;
				pos.y = BRSS.y + GetStyle().ItemInnerSpacing.y;
				drawList->AddText(pos, 0xff000000, label.label.c_str());
			}
		}
	}

	SetCursorPos({ BRWS.x, BRWS.y + GetStyle().ItemSpacing.y });
	NewLine();
	*/
	
	ImVec2 TLSS = gui_type_conversion<ImVec2>::from(fd.toGlobal(m_translation));
	ImVec2 BRSS = gui_type_conversion<ImVec2>::from(fd.toGlobal(m_translation + m_size * m_scale));
	
	ImDrawList* drawList = GetWindowDrawList();
	drawList->AddRectFilled(TLSS, BRSS, ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_FrameBg]));

	Composite::frame(fd);

	//I would much prefer to separate this from the frame event, 
	//and process each input to completion before considering the next.
	//Like this it's hard to control and overview who receives what input.
	if (m_mouseHandler) {

		ImGuiContext* g = GetCurrentContext();
		assert(g);

		enum class Capturing 
		{
			NONE,
			WE,
			CHILD,
			OTHER
		};

		Capturing who;
		if (Mouse::getCapture() == this)
			who = Capturing::WE;
		else if (Mouse::getCapture() == nullptr)
			//need to account for items that are still using imgui here (not sure if this will be enough)
			who = g->ActiveId ? Capturing::OTHER : Capturing::NONE;
		else if (Mouse::getCapture()->hasAncestor(this))
			who = Capturing::CHILD;
		else {
			who = Capturing::OTHER;
		}

		if (who == Capturing::OTHER) {
			//We should not recieve inputs, and should lose focus if we had it
			if (m_mouseFocus) {
				m_mouseFocus = false;
				m_mouseHandler->onMouseLeave();
			}
		}
		else if (who == Capturing::CHILD) {
			//We should not receive input, but should also not lose focus
		}
		else {
			//We should receive inputs

			ImGuiIO& io = GetIO();

			bool capturing = who == Capturing::WE;

			ImGuiWindow* window = g->CurrentWindow;
			assert(window);

			bool inside = false;
			if (g->HoveredWindow == window)
				//TODO: deal with popups
				inside = io.MousePos.x >= TLSS.x && io.MousePos.x <= BRSS.x && io.MousePos.y >= TLSS.y && io.MousePos.y <= BRSS.y;

			if (capturing || inside)
			{
				//Moving
				if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
					m_mouseHandler->onMouseMove(gui_type_conversion<Floats<2>>::from(io.MouseDelta));

				//Clicking
				if (IsMouseClicked(ImGuiMouseButton_Left))
					m_mouseHandler->onMouseDown(Mouse::Button::LEFT);
				else if (IsMouseReleased(ImGuiMouseButton_Left))
					m_mouseHandler->onMouseUp(Mouse::Button::LEFT);

				if (IsMouseClicked(ImGuiMouseButton_Middle))
					m_mouseHandler->onMouseDown(Mouse::Button::MIDDLE);
				else if (IsMouseReleased(ImGuiMouseButton_Middle))
					m_mouseHandler->onMouseUp(Mouse::Button::MIDDLE);

				if (IsMouseClicked(ImGuiMouseButton_Right))
					m_mouseHandler->onMouseDown(Mouse::Button::RIGHT);
				else if (IsMouseReleased(ImGuiMouseButton_Right))
					m_mouseHandler->onMouseUp(Mouse::Button::RIGHT);

				//Scrolling
				SetItemUsingMouseWheel();//prevents imgui from scrolling the window
				if (io.MouseWheel != 0.0f && !fd.isWheelHandled()) {
					m_mouseHandler->onMouseWheel(io.MouseWheel);
					fd.setWheelHandled();
				}
			}
			if (!capturing) {
				if (inside) {
					//Entering
					if (!m_mouseFocus) {
						m_mouseFocus = true;
						m_mouseHandler->onMouseEnter();
					}
				}
				else {
					//Leaving
					if (m_mouseFocus) {
						m_mouseFocus = false;
						m_mouseHandler->onMouseLeave();
					}
				}
			}

		}

		if (Mouse::getCapture() == this) {

		}
		else if (Mouse::getCapture() == nullptr) {
			ImGuiIO& io = GetIO();

			//Handle moves
			if (io.MousePos.x >= TLSS.x && io.MousePos.x <= BRSS.x &&
				io.MousePos.y >= TLSS.y && io.MousePos.y <= BRSS.y)
			{
				//Cursor inside plot area

				if (!m_mouseFocus) {
					//Cursor entered
					m_mouseFocus = true;
					m_mouseHandler->onMouseEnter();
				}

				if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
					m_mouseHandler->onMouseMove(gui_type_conversion<Floats<2>>::from(io.MouseDelta));
			}
			else {
				//Cursor outside plot area

				if (m_mouseFocus) {
					//Cursor left
					m_mouseFocus = false;
					m_mouseHandler->onMouseLeave();
				}
			}

			//Handle clicks
			if (IsMouseClicked(ImGuiMouseButton_Left))
				m_mouseHandler->onMouseDown(Mouse::Button::LEFT);
		}
		//else ignore (someone else is capturing the mouse)
	}
}

gui::ComponentPtr gui::PlotArea::removeChild(IComponent* c)
{
	//Axes must be kept at 0. These are not removable.
	auto&& children = getChildren();
	assert(children.size() >= AREA_MIN_SIZE);
	if (c == children[AREA_AXES_INDEX].get())
		return ComponentPtr();
	else
		return Composite::removeChild(c);
}

void gui::PlotArea::clearChildren()
{
	//Axes must be kept at 0. These are not removable.
	auto&& children = getChildren();
	assert(children.size() >= AREA_MIN_SIZE);

	for (size_t i = AREA_MIN_SIZE; i < children.size(); i++) {
		assert(children[i]);
		children[i]->setParent(nullptr);
	}
	children.resize(AREA_MIN_SIZE);
}

void gui::PlotArea::setSize(const Floats<2>& size)
{
	assert(size[0] > 0.0f && size[1] > 0.0f);//deal with this later

	if (size.matrix() != m_size.matrix()) {
		//Scale the axis translation and scale proportionately to our size
		Floats<2> scale = size / m_size;
		getAxes().setTranslation(getAxes().getTranslation() * scale);
		getAxes().setScale(getAxes().getScale() * scale);

		m_size = size;
	}
}

gui::Axes& gui::PlotArea::getAxes() const
{
	auto&& children = getChildren();
	assert(children.size() >= AREA_MIN_SIZE && children[AREA_AXES_INDEX]);
	return static_cast<Axes&>(*children[AREA_AXES_INDEX]);
}

void gui::PlotArea::addCurve(std::unique_ptr<Curve>&& curve)
{
	getAxes().addCurve(std::move(curve));
}

void gui::PlotArea::removeCurve(Curve* curve)
{
	getAxes().removeCurve(curve);
}

//The limit functions assume linear axes for now
gui::Floats<2> gui::PlotArea::getXLimits() const
{
	float xscale = getAxes().getScale()[0];
	float xlow = -getAxes().getTranslation()[0] * xscale;
	return { xlow, xlow + m_size[0] * xscale };
}

void gui::PlotArea::setXLimits(const Floats<2>& lims)
{
	float xscale = m_size[0] / (lims[1] - lims[0]);
	getAxes().setScaleX(xscale);
	getAxes().setTranslationX(-lims[0] * xscale);
}

gui::Floats<2> gui::PlotArea::getYLimits() const
{
	//account for reversed axis convention
	float yscale = getAxes().getScale()[1];
	float yhigh = -getAxes().getTranslation()[1] / yscale;
	return { yhigh + m_size[1] / yscale, yhigh };
}

void gui::PlotArea::setYLimits(const Floats<2>& lims)
{
	//account for reversed axis convention
	float yscale = -m_size[1] / (lims[1] - lims[0]);
	getAxes().setScaleY(yscale);
	getAxes().setTranslationY(-lims[1] * yscale);
}


class DefaultXLabels final : public gui::Component
{
public:
	DefaultXLabels(const gui::Axes& axes) : m_axes{ axes }
	{
		m_sizeHint[1] = 20.0f;
	}

	virtual void frame(gui::FrameDrawer& fd) override
	{
	}

private:
	const gui::Axes& m_axes;
};

class DefaultYLabels final : public gui::Component
{
public:
	DefaultYLabels(const gui::Axes& axes) : m_axes{ axes }
	{
		m_sizeHint[0] = 20.0f;
	}

private:
	const gui::Axes& m_axes;
};

gui::Plot::Plot()
{
	//Add plot area and default labels
	auto area = newChild<PlotArea>();
	addChild(std::make_unique<DefaultXLabels>(area->getAxes()));
	addChild(std::make_unique<DefaultYLabels>(area->getAxes()));
}

void gui::Plot::frame(FrameDrawer& fd)
{
	using namespace ImGui;
	//We do the layout. 
	//Maybe we could have the components prompt this calculation when they are resized/replaced,
	//rather than doing it every frame. We could store a "dirty" flag on Component.
	//This should also be done by a replaceable component, rather than being hardcoded here.

	//Our layout won't make sense without a parent
	IComponent* parent = getParent();
	assert(parent);

	//Ideally, our translation should have been set by the layout manager of our parent.
	//Until we implement that concept, we do this the old-fashioned way:
	//(this assumes that ours is the only transform between us and our ImGui window)
	Floats<2> scale = fd.getCurrentScale();
	ImVec2 cursorPos = GetCursorPos();
	m_translation = gui_type_conversion<Floats<2>>::from(cursorPos) / scale;

	Floats<2> parentSize = parent->getSize();
	float xHeight = getXLabels().getSizeHint()[1];
	float yWidth = getYLabels().getSizeHint()[0];

	assert(scale[0] == scale[1]);//assume isotropic scale
	//More generally we might not want to add window padding. A solution could be to have a Component that
	//represents the client area of Window, that all widgets of the window are parented to.
	//That way, our parent's size already includes padding (if it is indeed a window) and we never have to worry about it.
	ImVec2 windowPadding = GetStyle().WindowPadding / scale[0];
	ImVec2 labelPadding = GetStyle().ItemInnerSpacing / scale[0];//space between label widget and plot area

	//If our x-size hint is non-positive (or larger than parent), fit width to parent.
	float maxWidth = parentSize[0] - m_translation[0] - windowPadding.x;
	m_size[0] = m_sizeHint[0] > 0.0f ? std::min(m_sizeHint[0], maxWidth) : maxWidth;

	//If our y-size hint is non-positive (or larger than parent), make the plot area a square that fits the width.
	float maxHeight = parentSize[1] - m_translation[1] - windowPadding.y;
	float plotAreaX = yWidth + labelPadding.x;
	float plotAreaW = std::max(m_size[0] - plotAreaX, 0.0f);
	if (m_sizeHint[1] > 0.0f)
		m_size[1] = std::min(m_sizeHint[1], maxHeight);
	else
		m_size[1] = std::min(plotAreaW + xHeight + labelPadding.y, maxHeight);

	float plotAreaH = std::max(m_size[1] - xHeight - labelPadding.y, 0.0f);

	//deal with these problems later
	assert(plotAreaW > 0.0f);
	assert(plotAreaH > 0.0f);

	getPlotArea().setSize({ plotAreaW, plotAreaH });
	getPlotArea().setTranslation({ plotAreaX, 0.0f });

	getXLabels().setSize({ plotAreaW, xHeight });
	getXLabels().setTranslation({ plotAreaX, plotAreaH + labelPadding.y });

	getYLabels().setSize({ yWidth, plotAreaH });
	getYLabels().setTranslation({ 0.0f, 0.0f });

	ImGui::Dummy(gui_type_conversion<ImVec2>::from((m_size * scale).eval()));
	ImVec2 cursorPosEnd = GetCursorPos();

	Composite::frame(fd);

	SetCursorPos(cursorPosEnd);//make sure we leave it where it should be
}

gui::ComponentPtr gui::Plot::removeChild(IComponent* c)
{
	//Plot area and labels must be kept at 0, 1, 2. These are not removable.
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE);
	if (c == children[PLOT_PLOTAREA_INDEX].get() || c == children[PLOT_XLABEL_INDEX].get() || c == children[PLOT_YLABEL_INDEX].get())
		return ComponentPtr();
	else
		return Composite::removeChild(c);
}

void gui::Plot::clearChildren()
{
	//Plot area and labels must be kept at 0, 1, 2. These are not removable.
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE);

	for (size_t i = PLOT_MIN_SIZE; i < children.size(); i++) {
		assert(children[i]);
		children[i]->setParent(nullptr);
	}
	children.resize(PLOT_MIN_SIZE);
}

gui::PlotArea& gui::Plot::getPlotArea() const
{
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE);
	return static_cast<PlotArea&>(*children[PLOT_PLOTAREA_INDEX]);
}

gui::IComponent& gui::Plot::getXLabels() const
{
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE);
	return *children[PLOT_XLABEL_INDEX];
}

void gui::Plot::setXLabels(ComponentPtr&& c)
{
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE && children[PLOT_XLABEL_INDEX]);
	children[PLOT_XLABEL_INDEX]->setParent(nullptr);
	children[PLOT_XLABEL_INDEX] = std::move(c);
}

gui::IComponent& gui::Plot::getYLabels() const
{
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE);
	return *children[PLOT_YLABEL_INDEX];
}

void gui::Plot::setYLabels(ComponentPtr&& c)
{
	auto&& children = getChildren();
	assert(children.size() >= PLOT_MIN_SIZE && children[PLOT_YLABEL_INDEX]);
	children[PLOT_YLABEL_INDEX]->setParent(nullptr);
	children[PLOT_YLABEL_INDEX] = std::move(c);
}


gui::CustomXLabels::CustomXLabels(const Axes& axes, std::vector<AxisLabel>&& labels) : 
	m_axes{ axes }, m_labels{ labels }
{
	m_sizeHint = getDefaultHeightConst();
}

void gui::CustomXLabels::frame(FrameDrawer& fd)
{
	ImDrawList* dl = ImGui::GetWindowDrawList();
	assert(dl);
	//Transform our boundaries to global
	Floats<2> globalPos = fd.toGlobal(m_translation).round();
	float limR = std::round(fd.toGlobal(m_translation + m_size * m_scale)[0]);
	for (auto&& label : m_labels) {
		//Transform label value to global and test if it's within our boundaries
		float globalVal = std::round(m_axes.toGlobalSpace({ label.value, 0.0f })[0]);
		if (globalVal >= std::min(globalPos[0], limR) && globalVal <= std::max(globalPos[0], limR)) {
			ImVec2 size = ImGui::CalcTextSize(label.label.c_str());
			ImVec2 pos{ globalVal - label.alignment * size.x, globalPos[1] };
			dl->AddText(pos, 0xff000000, label.label.c_str());
		}
	}
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
	using namespace ImGui;

	float size = m_handleSize * m_scale[0];
	float buttonSize = m_handleSize * s_buttonMult;

	ImVec2 pos = GetCursorPos();//we should probably reset this when we're done

	for (size_t i = 0; i < m_points.size(); i++) {
		ImVec2 p = gui_type_conversion<ImVec2>::from(fd.toGlobal(m_points[i]));
		p[0] = std::round(p[0]);
		p[1] = std::round(p[1]);

		ImVec2 buttonTL = { p.x - buttonSize / 2.0f, p.y - buttonSize / 2.0f };
		ImVec2 buttonBR = { p.x + buttonSize / 2.0f, p.y + buttonSize / 2.0f };

		//Expand the clip region if we are inside the previous one
		ImGuiWindow* window = GetCurrentWindow();
		assert(window);
		ImVec4 lims = window->ClipRect.ToVec4();
		lims.x = std::round(lims.x);
		lims.y = std::round(lims.y);
		lims.z = std::round(lims.z);
		lims.w = std::round(lims.w);
		util::CallWrapper end;
		if (p[0] >= lims.x && p[0] <= lims.z && p[1] >= lims.y && p[1] <= lims.w) {
			PushClipRect(buttonTL, buttonBR, false);
			end = util::CallWrapper(&PopClipRect);
		}

		//Apparently, there are Push/PopID functions to replace this
		SetCursorScreenPos(buttonTL);
		char buf[16];
		snprintf(buf, sizeof(buf), "##point%d", static_cast<int>(i));

		InvisibleButton(buf, { buttonSize, buttonSize }, ImGuiButtonFlags_MouseButtonLeft);

		bool highlight = false;

		if (IsItemActivated())
			onClick(i, MouseButton::LEFT);

		if (IsItemActive()) {
			highlight = true;
			Floats<2> scale = fd.getCurrentScale();
			float x;
			float y;
			if (GetIO().MouseDelta.x != 0.0f)
				x = m_points[i][0] + GetIO().MouseDelta.x / scale[0];
			else
				x = m_points[i][0];
			if (GetIO().MouseDelta.y != 0.0f)
				y = m_points[i][1] + GetIO().MouseDelta.y / scale[1];
			else
				y = m_points[i][1];

			if (x != m_points[i][0] || y != m_points[i][1])
				onMove(i, { x, y });
		}

		if (IsItemDeactivated())
			onRelease(i, MouseButton::LEFT);

		if (!highlight && IsItemHovered())
			highlight = true;

		ImU32 col = highlight ? ColorConvertFloat4ToU32({ 1.0f, 1.0f, 1.0f, 1.0f }) : ColorConvertFloat4ToU32(GetStyle().Colors[ImGuiCol_Text]);

		window->DrawList->AddRectFilled(
			{ p.x - size / 2.0f, p.y - size / 2.0f },
			{ p.x + size / 2.0f, p.y + size / 2.0f },
			col);
	}

	SetCursorPos(pos);
}
