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

//These should be templated or subclassed/decorated to allow different behaviour. Let's start with this.
namespace gui
{
	class Curve;

	class PlotArea : public Composite
	{
	public:
		virtual ~PlotArea() = default;
		virtual void frame(FrameDrawer& fd) override;

		void addCurve(std::unique_ptr<Curve>&& curve);
		void removeCurve(Curve* curve);

		//{ x_min, x_max, y_min, y_max }
		Floats<4> getLimits() const { return { m_xlim[0], m_xlim[1], m_ylim[0], m_ylim[1] }; }

		//Convert plot-local coordinates to screen space. Call from a child, or it may lag one frame.
		Floats<2> localToScreen(const Floats<2>& p) const;
		//Don't know if it makes sense to expose these (but our controls want them)
		Floats<2> getScale() const { return m_scale; }
		Floats<2> getTranslation() const { return m_translation; }

	private:
		std::vector<std::unique_ptr<Curve>> m_curves;

		Floats<2> m_majorGrid{ 1.0f, 1.0f };
		float m_majorGridWidth{ 1.0f };

		//Axis limits, local coords
		Floats<2> m_xlim{ 0.0f, 1.0f };
		Floats<2> m_ylim{ 0.0f, 1.0f };

		//position of local origin in screen space (updates every frame)
		Floats<2> m_translation{ 0.0f, 0.0f };
		//length of the local unit in screen space coords (updates every frame)
		Floats<2> m_scale{ 1.0f, 1.0f };

	};

	class Curve
	{
	public:
		virtual ~Curve() = default;
		//This is an inert graphical component, so this api makes sense (?)
		virtual void draw(Drawer&) const = 0;
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
		SimpleHandles(const PlotArea& area, const std::vector<Floats<2>>& points) : 
			m_area{ area }, m_points{ points } {}
		virtual ~SimpleHandles() = default;

		virtual void frame(FrameDrawer& fd) override;

		virtual void onClick(size_t, MouseButton) = 0;
		virtual void onMove(size_t, const Floats<2>& pos) = 0;
		virtual void onRelease(size_t, MouseButton) = 0;

	protected:
		const std::vector<Floats<2>>& m_points;

	private:
		const PlotArea& m_area;//we need some sensible way to limit test

		float m_size{ 5.0f };
	};
}