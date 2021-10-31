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


//This is an adaptation of an algorithm from GSL by Brian Gough

#include "pch.h"
#include "Optimisation.h"

int math::Quadratic::roots(double& r1, double& r2) const
{
    int result;

    //Linear case
    if (m_c[2] == 0.0) {
        if (m_c[1] != 0.0) {
            r1 = -m_c[0] / m_c[1];
            result = 1;
        }
        else
            result = 0;
    }
    else if (double disc = m_c[1] * m_c[1] - 4.0 * m_c[2] * m_c[0]; disc > 0.0) {
        //two distinct roots
        if (m_c[1] == 0.0) {
            double r = std::sqrt(-m_c[0] / m_c[2]);
            r1 = -r;
            r2 = r;
        }
        else {
            double temp = -0.5 * (m_c[1] + std::copysign(std::sqrt(disc), m_c[1]));
            double root1 = temp / m_c[2];
            double root2 = m_c[0] / temp;

            if (root1 < root2) {
                r1 = root1;
                r2 = root2;
            }
            else {
                r1 = root2;
                r2 = root1;
            }
        }
        result = 2;
    }
    else if (disc == 0) {
        //two identical roots
        r1 = -0.5 * m_c[1] / m_c[2];
        r2 = r1;
        result = 2;
    }
    else
        //complex
        result = 0;

    return result;
}

double math::Quadratic::localMin(double a, double b) const
{
	double result;
	if (double extr; curvature() > 0.0 && (extr = extremum()) > a && extr < b) {
		result = extr;
	}
	else {
		double fa = eval(a);
		double fb = eval(b);
		if (fa < fb)
			result = a;
		else
			result = b;
	}

	return result;
}

math::Cubic::Cubic(const Eigen::Vector2d& p0, const Eigen::Vector2d& p1) :
    m_c{ p0[0], p0[1], 3.0 * (p1[0] - p0[0]) - 2.0 * p0[1] - p1[1], p0[1] + p1[1] - 2.0 * (p1[0] - p0[0]) }
{
}

double math::Cubic::localMin(double a, double b) const
{
    double result;

    double fmin = eval(a);
    if (double ftmp = eval(b); ftmp < fmin) {
        result = b;
        fmin = ftmp;
    }
    else
        result = a;

    double r1, r2;
    int n = derivative().roots(r1, r2);
    if (n == 2) {
        if (r2 > a && r2 < b) {
            double ftmp = eval(r2);
            if (ftmp < fmin) {
                result = r2;
                fmin = ftmp;
            }

        }
    }
    if (n > 0) {
        if (r1 > a && r1 < b) {
            double ftmp = eval(r1);
            if (ftmp < fmin) {
                result = r1;
                fmin = ftmp;
            }
        }
    }

    return result;
}
