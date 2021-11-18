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
#include "SyncTraversers.h"
#include "File.h"

void nif::NiReadSyncer::traverse(NiObjectNET& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiObjectNET>{}.syncRead(m_file, &obj, m_file.get<Niflib::NiObjectNET>(&obj));
}

void nif::NiReadSyncer::traverse(NiAVObject& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiAVObject>{}.syncRead(m_file, &obj, m_file.get<Niflib::NiAVObject>(&obj));
}

void nif::NiReadSyncer::traverse(NiNode& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiNode>{}.syncRead(m_file, &obj, m_file.get<Niflib::NiNode>(&obj));
}

void nif::NiReadSyncer::traverse(BSFadeNode& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<BSFadeNode>{}.syncRead(m_file, &obj, m_file.get<Niflib::BSFadeNode>(&obj));
}

void nif::NiWriteSyncer::traverse(NiObjectNET& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiObjectNET>{}.syncWrite(m_file, &obj, m_file.get<Niflib::NiObjectNET>(&obj));
}

void nif::NiWriteSyncer::traverse(NiAVObject& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiAVObject>{}.syncWrite(m_file, &obj, m_file.get<Niflib::NiAVObject>(&obj));
}

void nif::NiWriteSyncer::traverse(NiNode& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<NiNode>{}.syncWrite(m_file, &obj, m_file.get<Niflib::NiNode>(&obj));
}

void nif::NiWriteSyncer::traverse(BSFadeNode& obj)
{
	NiTraverser::traverse(obj);
	NiSyncer<BSFadeNode>{}.syncWrite(m_file, &obj, m_file.get<Niflib::BSFadeNode>(&obj));
}