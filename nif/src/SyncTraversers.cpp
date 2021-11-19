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
	ReadSyncer<NiObjectNET>{}.down(obj, m_file.get<Niflib::NiObjectNET>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiAVObject& obj)
{
	ReadSyncer<NiAVObject>{}.down(obj, m_file.get<Niflib::NiAVObject>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiNode& obj)
{
	ReadSyncer<NiNode>{}.down(obj, m_file.get<Niflib::NiNode>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(BSFadeNode& obj)
{
	ReadSyncer<BSFadeNode>{}.down(obj, m_file.get<Niflib::BSFadeNode>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiProperty& obj)
{
	ReadSyncer<NiProperty>{}.down(obj, m_file.get<Niflib::NiProperty>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiAlphaProperty& obj)
{
	ReadSyncer<NiAlphaProperty>{}.down(obj, m_file.get<Niflib::NiAlphaProperty>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(BSEffectShaderProperty& obj)
{
	ReadSyncer<BSEffectShaderProperty>{}.down(obj, m_file.get<Niflib::BSEffectShaderProperty>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiBoolData& obj)
{
	ReadSyncer<NiBoolData>{}.down(obj, m_file.get<Niflib::NiBoolData>(&obj), m_file);
}

void nif::NiReadSyncer::traverse(NiFloatData& obj)
{
	ReadSyncer<NiFloatData>{}.down(obj, m_file.get<Niflib::NiFloatData>(&obj), m_file);
}



void nif::NiWriteSyncer::traverse(NiObjectNET& obj)
{
	WriteSyncer<NiObjectNET>{}.down(obj, m_file.get<Niflib::NiObjectNET>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiAVObject& obj)
{
	WriteSyncer<NiAVObject>{}.down(obj, m_file.get<Niflib::NiAVObject>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiNode& obj)
{
	WriteSyncer<NiNode>{}.down(obj, m_file.get<Niflib::NiNode>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(BSFadeNode& obj)
{
	WriteSyncer<BSFadeNode>{}.down(obj, m_file.get<Niflib::BSFadeNode>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiProperty& obj)
{
	WriteSyncer<NiProperty>{}.down(obj, m_file.get<Niflib::NiProperty>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiAlphaProperty& obj)
{
	WriteSyncer<NiAlphaProperty>{}.down(obj, m_file.get<Niflib::NiAlphaProperty>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(BSEffectShaderProperty& obj)
{
	WriteSyncer<BSEffectShaderProperty>{}.down(obj, m_file.get<Niflib::BSEffectShaderProperty>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiBoolData& obj)
{
	WriteSyncer<NiBoolData>{}.down(obj, m_file.get<Niflib::NiBoolData>(&obj), m_file);
}

void nif::NiWriteSyncer::traverse(NiFloatData& obj)
{
	WriteSyncer<NiFloatData>{}.down(obj, m_file.get<Niflib::NiFloatData>(&obj), m_file);
}
