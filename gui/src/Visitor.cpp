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
#include "Visitor.h"
#include "Composition.h"
#include "Connector.h"
#include "ConnectionHandler.h"

void gui::AscendingVisitor::visit(Connector& c)
{
	visit(static_cast<Component&>(c));
}

void gui::AscendingVisitor::visit(ConnectionHandler& c)
{
	visit(static_cast<Component&>(c));
}

void gui::AscendingVisitor::visit(Composite& c)
{
	visit(static_cast<Component&>(c));
}

void gui::AscendingVisitor::visit(Component& c)
{
	if (IComponent* parent = c.getParent())
		parent->accept(*this);
}


void gui::DescendingVisitor::visit(Connector& c)
{
	visit(static_cast<Component&>(c));
}

void gui::DescendingVisitor::visit(ConnectionHandler& c)
{
	visit(static_cast<Composite&>(c));
}

void gui::DescendingVisitor::visit(Composite& c)
{
	for (auto& child : c.getChildren()) {
		assert(child);
		child->accept(*this);
	}
}

void gui::DescendingVisitor::visit(Component& c)
{
}
