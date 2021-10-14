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

#pragma once

namespace gui
{
	class Component;
	class Composite;
	class Connector;
	class ConnectionHandler;

	class Visitor
	{
	public:
		virtual ~Visitor() {}

		virtual void visit(Connector& c) = 0;
		virtual void visit(ConnectionHandler& c) = 0;
		virtual void visit(Composite& c) = 0;
		virtual void visit(Component& c) = 0;
	};

	class AscendingVisitor :
		public Visitor
	{
	public:
		virtual ~AscendingVisitor() {}

		virtual void visit(Connector& c) override;
		virtual void visit(ConnectionHandler& c) override;
		virtual void visit(Composite& c) override;
		virtual void visit(Component& c) override;
	};

	class DescendingVisitor :
		public Visitor
	{
	public:
		virtual ~DescendingVisitor() {}

		virtual void visit(Connector& c) override;
		virtual void visit(ConnectionHandler& c) override;
		virtual void visit(Composite& c) override;
		virtual void visit(Component& c) override;
	};
}

