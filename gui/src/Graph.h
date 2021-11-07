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
#include "Widget.h"
#include "Drawer.h"

namespace gui
{
	//Generalises to all widgets?
	class MouseHandler
	{
	public:
		virtual ~MouseHandler() = default;

		virtual void onMouseDown(Mouse::Button) {}
		virtual void onMouseUp(Mouse::Button) {}
		virtual void onMouseMove(const Floats<2>&) {}

		virtual void onMouseEnter() {}
		virtual void onMouseLeave() {}

		virtual void onMouseWheel(float) {}

		//Make a different handler for this?
		//virtual void onKeyDown(Key) {}
		//virtual void onKeyUp(Key) {}
	};

	class Curve
	{
	public:
		virtual ~Curve() = default;
		//This is an inert graphical component, so this api makes sense (?)
		virtual void draw(Drawer&) const = 0;
	};

	//Log axes could work too. We can't use the built-in linear transform, 
	//but we could wrap the FrameDrawer in a logarithmic one, which forwards
	//everything but the transform calls to the original.
	class Axes : public Composite
	{
	public:
		virtual ~Axes() = default;

		virtual void frame(FrameDrawer& fd) override;

		void addCurve(std::unique_ptr<Curve>&& curve);
		std::unique_ptr<Curve> removeCurve(Curve* curve);
		std::unique_ptr<Curve> removeCurve(int index);

		Floats<2> getMajorUnits() const { return m_majorUnit; }
		void setMajorUnits(const Floats<2>& units) { m_majorUnit = units; }
		Floats<2> getMinorUnits() const { return m_minorUnit; }
		void setMinorUnits(const Floats<2>& units) { m_minorUnit = units; }

		float getMajorUnitX() const { return m_majorUnit[0]; }
		void setMajorUnitX(float f) { m_majorUnit[0] = f; }
		float getMinorUnitX() const { return m_minorUnit[0]; }
		void setMinorUnitX(float f) { m_minorUnit[0] = f; }

		float getMajorUnitY() const { return m_majorUnit[1]; }
		void setMajorUnitY(float f) { m_majorUnit[1] = f; }
		float getMinorUnitY() const { return m_minorUnit[1]; }
		void setMinorUnitY(float f) { m_minorUnit[1] = f; }

		//Returns the grid points in order from lims[0] to lims[1]
		std::vector<float> getGridPointsMajorX(const Floats<2>& lims) const;
		std::vector<float> getGridPointsMajorY(const Floats<2>& lims) const;
		std::vector<float> getGridPointsMinorX(const Floats<2>& lims) const;
		std::vector<float> getGridPointsMinorY(const Floats<2>& lims) const;

	private:
		std::vector<std::unique_ptr<Curve>> m_curves;
		Floats<2> m_majorUnit{ 1.0f, 1.0f };
		Floats<2> m_minorUnit{ 0.25f, 0.25f };
	};

	class PlotArea : public Composite
	{
	public:
		PlotArea();
		virtual ~PlotArea() = default;

		virtual void frame(FrameDrawer& fd) override;

		//deal with someone trying to remove our axes
		virtual ComponentPtr removeChild(IComponent* c) override;
		virtual void clearChildren() override;

		virtual void setSize(const Floats<2>& size) override;

		void setMouseHandler(std::unique_ptr<MouseHandler>&& h) { m_mouseHandler = std::move(h); }

		Axes& getAxes() const;

		void addCurve(std::unique_ptr<Curve>&& curve);
		void removeCurve(Curve* curve);

		Floats<2> getXLimits() const;
		void setXLimits(const Floats<2>& lims);
		//Y limits follow normal plotting conventions (axis goes from bottom to top)
		Floats<2> getYLimits() const;
		void setYLimits(const Floats<2>& lims);

	private:
		std::unique_ptr<MouseHandler> m_mouseHandler;
		bool m_mouseFocus{ false };
	};


	class Plot : public Composite
	{
	public:
		Plot();
		virtual ~Plot() = default;

		virtual void frame(FrameDrawer& fd) override;

		//deal with someone trying to remove our plot area or labels
		virtual ComponentPtr removeChild(IComponent* c) override;
		virtual void clearChildren() override;

		PlotArea& getPlotArea() const;//access to set limits

		//labels are replaceable widgets
		IComponent& getXLabels() const;
		void setXLabels(ComponentPtr&& c);
		IComponent& getYLabels() const;
		void setYLabels(ComponentPtr&& c);

	private:
		//store this as a replaceable object, rather than subclassing (not on us though, should be on PlotArea and label widgets)
		//IComponent* m_inputHandler;
	};

	class CustomXLabels final : public Component
	{
	public:
		struct AxisLabel
		{
			std::string label;
			float value;
			float alignment{ 0.5f };
		};
	public:
		CustomXLabels(const Axes& axes, std::vector<AxisLabel>&& labels);
		virtual void frame(FrameDrawer& fd) override;
		
	private:
		const gui::Axes& m_axes;
		std::vector<AxisLabel> m_labels;
	};


	class SimpleCurve : public Curve
	{
	public:
		SimpleCurve(const std::vector<Floats<2>>& points) : m_points{ points } {}
		virtual ~SimpleCurve() = default;

		virtual void draw(Drawer& d) const override;

	private:
		const std::vector<Floats<2>>& m_points;
	};

	class SimpleHandles : public Component
	{
	public:
		SimpleHandles(const std::vector<Floats<2>>& points) : 
			m_points{ points } {}
		virtual ~SimpleHandles() = default;

		virtual void frame(FrameDrawer& fd) override;

		virtual void onClick(size_t, MouseButton) = 0;
		virtual void onMove(size_t, const Floats<2>& pos) = 0;
		virtual void onRelease(size_t, MouseButton) = 0;

	protected:
		const std::vector<Floats<2>>& m_points;

	private:
		float m_handleSize{ 5.0f };
		constexpr static float s_buttonMult{ 3.0f };//invisible button size = handle size x this
	};
}