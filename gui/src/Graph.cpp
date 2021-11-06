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

//We make use of Eigen::Array methods for rounding
static_assert(TO_PIXEL == static_cast<decltype(TO_PIXEL)>(&std::floor));

void gui::Axes::frame(FrameDrawer& fd)
{
	assert(getParent() && dynamic_cast<PlotArea*>(getParent()));
	PlotArea* area = static_cast<PlotArea*>(getParent());

	Floats<2> parentSize = area->getSize();
	Floats<2> globalTL = fd.toGlobal({ 0.0f, 0.0f }).floor();
	Floats<2> globalBR = fd.toGlobal(parentSize).floor();

	ImU32 axisCol = 0xff000000;
	ImU32 majorCol = 0xff7f7f7f;
	ImU32 minorCol = 0xffbfbfbf;

	auto dl = ImGui::GetWindowDrawList();
	assert(dl);

	auto xlims = area->getXLimits();
	auto ylims = area->getYLimits();

	fd.pushTransform(m_translation, m_scale);
	{
		util::CallWrapper pop(&FrameDrawer::popTransform, &fd);
		auto points = getGridPointsMinorX(xlims);
		for (auto&& p : points) {
			p = TO_PIXEL(fd.toGlobal({ p, 0.0f })[0]);
			dl->AddLine({ p, globalTL[1] }, { p, globalBR[1] }, minorCol);
		}
		points = getGridPointsMinorY(ylims);
		for (auto&& p : points) {
			p = TO_PIXEL(fd.toGlobal({ 0.0f, p })[1]);
			dl->AddLine({ globalTL[0], p }, { globalBR[0], p }, minorCol);
		}

		points = getGridPointsMajorX(xlims);
		for (auto&& p : points) {
			p = TO_PIXEL(fd.toGlobal({ p, 0.0f })[0]);
			dl->AddLine({ p, globalTL[1] }, { p, globalBR[1] }, majorCol);
		}
		points = getGridPointsMajorY(ylims);
		for (auto&& p : points) {
			p = TO_PIXEL(fd.toGlobal({ 0.0f, p })[1]);
			dl->AddLine({ globalTL[0], p }, { globalBR[0], p }, majorCol);
		}
	}

	//Main axes
	Floats<2> globalTrans = fd.toGlobal(m_translation).floor();
	if (globalTrans[0] >= globalTL[0] && globalTrans[0] <= globalBR[0]) {
		ImGui::GetWindowDrawList()->AddLine(
			{ globalTrans[0], globalTL[1] },
			{ globalTrans[0], globalBR[1] },
			axisCol);
	}
	if (globalTrans[1] >= globalTL[1] && globalTrans[1] <= globalBR[1]) {
		ImGui::GetWindowDrawList()->AddLine(
			{ globalTL[0], globalTrans[1] },
			{ globalBR[0], globalTrans[1] },
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
	util::CallWrapper pop(&FrameDrawer::popClipArea, &fd);
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

static std::vector<float> findPointsInInterval(const gui::Floats<2>& lims, float unit) 
{
	//This is not a drawing function. We do not care about pixel aliasing.
	//It's the caller's responsibility to account for that in the limits they pass.
	assert(unit >= 0.0f);

	std::vector<float> result;

	//If "upper" limit is smaller than "lower" limit, flip the sign of the axis.
	//This means that the output gridpoint always go in order from lim[0] to lim[1].
	float step = lims[1] >= lims[0] ? unit : -unit;

	//Index to first gridpoint inside interval and to first gridpoint past the end
	//gui::Ints<2> indices = (lims / step).ceil().cast<int>();
	int n = static_cast<int>(std::ceil(lims[0] / step));
	int nend = static_cast<int>(std::floor(lims[1] / step)) + 1;

	result.reserve(nend - n);

	for (; n < nend; n++)
		result.push_back(n * step);

	return result;
}

std::vector<float> gui::Axes::getGridPointsMajorX(const Floats<2>& lims) const
{
	return findPointsInInterval(lims, m_majorUnit[0]);
}

std::vector<float> gui::Axes::getGridPointsMinorX(const Floats<2>& lims) const
{
	return findPointsInInterval(lims, m_minorUnit[0]);
}

std::vector<float> gui::Axes::getGridPointsMajorY(const Floats<2>& lims) const
{
	return findPointsInInterval(lims, m_majorUnit[1]);
}

std::vector<float> gui::Axes::getGridPointsMinorY(const Floats<2>& lims) const
{
	return findPointsInInterval(lims, m_minorUnit[1]);
}


gui::PlotArea::PlotArea()
{
	auto axes = newChild<Axes>();

	//This will define our axis limits
	m_size = { 1.0f, 1.0f };
	axes->setTranslation({ 0.0f, 1.0f });
	axes->setScale({ 1.0f, -1.0f });
}

void gui::PlotArea::frame(FrameDrawer& fd)
{
	using namespace ImGui;
	
	ImVec2 TLSS = gui_type_conversion<ImVec2>::from(fd.toGlobal(m_translation).floor().eval());
	ImVec2 BRSS = gui_type_conversion<ImVec2>::from(fd.toGlobal(m_translation + m_size * m_scale).floor().eval());
	
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

//The axis limits should be insensitive to roundoff and small variations,
//which will inevitably result from the various transforms and the pixel 
//aliasing of our size (which we don't actually register yet).
//We achieve this by rounding the calculated limits to some small fraction
//of the minor unit.
gui::Floats<2> gui::PlotArea::getXLimits() const
{
	//Works regardless of the nature of the axis transform
	Floats<2> tl = getAxes().fromParentSpace({ 0.0f, 0.0f });
	Floats<2> br = getAxes().fromParentSpace(m_size);

	//Round to some fraction of the axis unit
	float eps = getAxes().getMinorUnitX() * 0.01f;
	return eps * (Floats<2>{ tl[0], br[0] } / eps).round();
}

void gui::PlotArea::setXLimits(const Floats<2>& lims)
{
	//Works for any axis transform that can be represented by the parameters "scale" and "translation".
	//For a logarithmic axis those would correspond to the base and the prefactor, respectively.
	//If we ever want something crazier (very unlikely) we may have to revise this procedure.
	float xscale = m_size[0] / (lims[1] - lims[0]);
	getAxes().setScaleX(xscale);
	getAxes().setTranslationX(-lims[0] * xscale);
}

gui::Floats<2> gui::PlotArea::getYLimits() const
{
	Floats<2> tl = getAxes().fromParentSpace({ 0.0f, 0.0f });
	Floats<2> br = getAxes().fromParentSpace(m_size);

	//Round to some fraction of the axis unit
	float eps = getAxes().getMinorUnitY() * 0.01f;
	//account for reversed axis convention
	return eps * (Floats<2>{ br[1], tl[1] } / eps).round();
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
	DefaultXLabels(const gui::PlotArea& area) : m_area{ area }
	{
		m_sizeHint[1] = gui::getDefaultHeightConst();
	}

	virtual void frame(gui::FrameDrawer& fd) override
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		assert(dl);

		auto points = m_area.getAxes().getGridPointsMajorY(m_area.getYLimits());
		float globalY = std::round(fd.toGlobal(m_translation)[1]);

		char buf[8];
		for (auto&& p : points) {
			float globalX = std::round(m_area.getAxes().toGlobalSpace({ p, 0.0f })[0]);

			snprintf(buf, sizeof(buf), "%.1f", p);
			dl->AddText({ globalX, globalY }, 0xff000000, buf);
		}
	}

private:
	const gui::PlotArea& m_area;
};

class DefaultYLabels final : public gui::Component
{
public:
	DefaultYLabels(const gui::PlotArea& area) : m_area{ area }
	{
		m_sizeHint[0] = gui::getDefaultHeightConst();
	}

	virtual void frame(gui::FrameDrawer& fd) override
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		assert(dl);

		auto points = m_area.getAxes().getGridPointsMajorY(m_area.getYLimits());
		float globalX = std::round(fd.toGlobal(m_translation)[0]);

		char buf[8];
		for (auto&& p : points) {
			float globalY = std::round(m_area.getAxes().toGlobalSpace({ 0.0f, p })[1]);

			snprintf(buf, sizeof(buf), "%.1f", p);
			dl->AddText({ globalX, globalY }, 0xff000000, buf);
		}
	}

private:
	const gui::PlotArea& m_area;
};

gui::Plot::Plot()
{
	//Add plot area and default labels
	auto area = newChild<PlotArea>();
	addChild(std::make_unique<DefaultXLabels>(*area));
	addChild(std::make_unique<DefaultYLabels>(*area));
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
