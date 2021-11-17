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


std::map<size_t, nif::File::CreateFcn> nif::File::s_typeRegistry;
void nif::File::registerTypes()
{
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObject::TYPE)] = &nif::make_NiObject<nif::NiObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObjectNET::TYPE)] = &nif::make_NiObject<nif::NiObjectNET>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAVObject::TYPE)] = &nif::make_NiObject<nif::NiAVObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiNode::TYPE)] = &nif::make_NiObject<nif::NiNode>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSFadeNode::TYPE)] = &nif::make_NiObject<nif::BSFadeNode>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiExtraData::TYPE)] = &nif::make_NiObject<nif::NiExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiStringExtraData::TYPE)] = &nif::make_NiObject<nif::NiStringExtraData>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBoolData::TYPE)] = &nif::make_NiObject<nif::NiBoolData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiFloatData::TYPE)] = &nif::make_NiObject<nif::NiFloatData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiInterpolator::TYPE)] = &nif::make_NiObject<nif::NiInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBoolInterpolator::TYPE)] = &nif::make_NiObject<nif::NiBoolInterpolator>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiFloatInterpolator::TYPE)] = &nif::make_NiObject<nif::NiFloatInterpolator>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiTimeController::TYPE)] = &nif::make_NiObject<nif::NiTimeController>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiSingleInterpController::TYPE)] = &nif::make_NiObject<nif::NiSingleInterpController>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysData::TYPE)] = &nif::make_NiObject<nif::NiPSysData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiParticleSystem::TYPE)] = &nif::make_NiObject<nif::NiParticleSystem>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifierCtlr::TYPE)] = &nif::make_NiObject<nif::NiPSysModifierCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysUpdateCtlr::TYPE)] = &nif::make_NiObject<nif::NiPSysUpdateCtlr>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoundUpdateModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysBoundUpdateModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysAgeDeathModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysAgeDeathModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysPositionModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysPositionModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysGravityModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysGravityModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysRotationModifier::TYPE)] = &nif::make_NiObject<nif::NiPSysRotationModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysScaleModifier::TYPE)] = &nif::make_NiObject<nif::BSPSysScaleModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysSimpleColorModifier::TYPE)] = &nif::make_NiObject<nif::BSPSysSimpleColorModifier>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitterCtlr::TYPE)] = &nif::make_NiObject<nif::NiPSysEmitterCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitter::TYPE)] = &nif::make_NiObject<nif::NiPSysEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysVolumeEmitter::TYPE)] = &nif::make_NiObject<nif::NiPSysVolumeEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoxEmitter::TYPE)] = &nif::make_NiObject<nif::NiPSysBoxEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysCylinderEmitter::TYPE)] = &nif::make_NiObject<nif::NiPSysCylinderEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysSphereEmitter::TYPE)] = &nif::make_NiObject<nif::NiPSysSphereEmitter>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAlphaProperty::TYPE)] = &nif::make_NiObject<nif::NiAlphaProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSEffectShaderProperty::TYPE)] = &nif::make_NiObject<nif::BSEffectShaderProperty>;
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
		auto block = make_ni<NiNode>(node);

		assert(block);//make_ni should throw on allocation or ctor failure

		if (auto res = m_objectIndex.insert({ block->object, block }); !res.second) {
			//unexpected: the object has already been created
			throw std::runtime_error("Unexpected recreation of object");
		}
		if (auto res = m_nativeIndex.insert({ block->native, block }); !res.second) {
			//same here
			throw std::runtime_error("Unexpected recreation of object");
		}

		//make_ni<T> must guarantee that the created object is actually of type T (or derived).
		//(I don't like how these assumptions keep spilling out of the functions that make them)
		m_rootNode = std::shared_ptr<NiNode>(block, static_cast<NiNode*>(block->object));
		m_rootBlock = block;

		//will get any referenced blocks, adding them to our index
		syncRead();
	}
}

void nif::File::write(const std::filesystem::path& path)
{
	if (!path.empty() && syncWrite()) {
		assert(m_rootBlock);//or syncWrite should have failed
		
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
		Niflib::WriteNifTree(out, m_rootBlock->native, fileInfo);
	}
}

bool nif::File::syncRead()
{
	if (m_rootBlock) {
		assert(m_rootBlock->syncer);
		m_rootBlock->syncer->syncRead(*this, m_rootBlock->object, m_rootBlock->native);
		return true;
	}
	else
		return false;
}

bool nif::File::syncWrite() const
{
	if (m_rootBlock) {
		assert(m_rootBlock->syncer);
		m_rootBlock->syncer->syncWrite(*this, m_rootBlock->object, m_rootBlock->native);
		return true;
	}
	else
		return false;
}
