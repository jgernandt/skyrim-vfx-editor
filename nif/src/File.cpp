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
#include "File.h"
#include "SyncTraversers.h"


std::map<size_t, nif::File::CreateFcn> nif::File::s_typeRegistry;
void nif::File::registerTypes()
{
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObject::TYPE)] = &make_NiObject<nif::NiObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObjectNET::TYPE)] = &make_NiObject<nif::NiObjectNET>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAVObject::TYPE)] = &make_NiObject<nif::NiAVObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiNode::TYPE)] = &make_NiObject<nif::NiNode>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSFadeNode::TYPE)] = &make_NiObject<nif::BSFadeNode>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiExtraData::TYPE)] = &make_NiObject<nif::NiExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiStringExtraData::TYPE)] = &make_NiObject<nif::NiStringExtraData>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBoolData::TYPE)] = &make_NiObject<nif::NiBoolData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiFloatData::TYPE)] = &make_NiObject<nif::NiFloatData>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiInterpolator::TYPE)] = &make_NiObject<nif::NiInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBoolInterpolator::TYPE)] = &make_NiObject<nif::NiBoolInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiFloatInterpolator::TYPE)] = &make_NiObject<nif::NiFloatInterpolator>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBlendInterpolator::TYPE)] = &make_NiObject<nif::NiBlendInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBlendBoolInterpolator::TYPE)] = &make_NiObject<nif::NiBlendBoolInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBlendFloatInterpolator::TYPE)] = &make_NiObject<nif::NiBlendFloatInterpolator>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiTimeController::TYPE)] = &make_NiObject<nif::NiTimeController>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiSingleInterpController::TYPE)] = &make_NiObject<nif::NiSingleInterpController>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysData::TYPE)] = &make_NiObject<nif::NiPSysData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiParticleSystem::TYPE)] = &make_NiObject<nif::NiParticleSystem>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifierCtlr::TYPE)] = &make_NiObject<nif::NiPSysModifierCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysUpdateCtlr::TYPE)] = &make_NiObject<nif::NiPSysUpdateCtlr>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifier::TYPE)] = &make_NiObject<nif::NiPSysModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoundUpdateModifier::TYPE)] = &make_NiObject<nif::NiPSysBoundUpdateModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysAgeDeathModifier::TYPE)] = &make_NiObject<nif::NiPSysAgeDeathModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysPositionModifier::TYPE)] = &make_NiObject<nif::NiPSysPositionModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysGravityModifier::TYPE)] = &make_NiObject<nif::NiPSysGravityModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysRotationModifier::TYPE)] = &make_NiObject<nif::NiPSysRotationModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysScaleModifier::TYPE)] = &make_NiObject<nif::BSPSysScaleModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysSimpleColorModifier::TYPE)] = &make_NiObject<nif::BSPSysSimpleColorModifier>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitterCtlr::TYPE)] = &make_NiObject<nif::NiPSysEmitterCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitter::TYPE)] = &make_NiObject<nif::NiPSysEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysVolumeEmitter::TYPE)] = &make_NiObject<nif::NiPSysVolumeEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoxEmitter::TYPE)] = &make_NiObject<nif::NiPSysBoxEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysCylinderEmitter::TYPE)] = &make_NiObject<nif::NiPSysCylinderEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysSphereEmitter::TYPE)] = &make_NiObject<nif::NiPSysSphereEmitter>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiProperty::TYPE)] = &make_NiObject<nif::NiProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAlphaProperty::TYPE)] = &make_NiObject<nif::NiAlphaProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSEffectShaderProperty::TYPE)] = &make_NiObject<nif::BSEffectShaderProperty>;
}

namespace Niflib
{
	NiObjectRef FindRoot(std::vector<NiObjectRef> const& objects);
}

nif::File::File(Version version) : m_version{ version }
{
	makeRoot(new Niflib::BSFadeNode());
	if (m_rootNode)
		m_rootNode->name.set("NewFile.nif");
}

nif::File::File(const std::filesystem::path& path)
{
	if (!path.empty()) {
		std::ifstream in(path, std::ifstream::binary);
		Niflib::NifInfo fileInfo;
		auto objects = Niflib::ReadNifList(in, &fileInfo);
		in.close();

		if (fileInfo.version == 0x14020007 && fileInfo.userVersion == 12) {
			if (fileInfo.userVersion2 == 83)
				m_version = Version::SKYRIM;
			else if (fileInfo.userVersion2 == 100)
				m_version = Version::SKYRIM_SE;
		}

		makeRoot(Niflib::DynamicCast<Niflib::NiNode>(Niflib::FindRoot(objects)));
	}
}

nif::File::~File()
{
}

void nif::File::makeRoot(const Niflib::Ref<Niflib::NiNode>& node)
{
	if (node) {
		m_rootNode = make_ni<NiNode>(node);

		//will get any referenced blocks, adding them to our index
		NiReadSyncer syncer(*this);
		m_rootNode->receive(syncer);
	}
}

void nif::File::write(const std::filesystem::path& path)
{
	if (m_rootNode && !path.empty()) {
		if (auto it = m_objectIndex.find(m_rootNode.get()); it != m_objectIndex.end()) {
			if (auto block = it->second.lock()) {

				NiWriteSyncer syncer(*this);
				m_rootNode->receive(syncer);

				Niflib::NifInfo fileInfo;
				switch (m_version) {
				case Version::SKYRIM:
					fileInfo.version = 0x14020007;
					fileInfo.userVersion = 12;
					fileInfo.userVersion2 = 83;
					break;
				case Version::SKYRIM_SE:
					fileInfo.version = 0x14020007;
					fileInfo.userVersion = 12;
					fileInfo.userVersion2 = 100;
					break;
				}
				fileInfo.exportInfo1 = "SVFX Editor";
				fileInfo.exportInfo2 = "Niflib";

				std::ofstream out(path, std::ofstream::binary);
				Niflib::WriteNifTree(out, block->native, fileInfo);
			}
		}
	}
}
