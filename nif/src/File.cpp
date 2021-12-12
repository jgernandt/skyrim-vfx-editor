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
#include "File.inl"

#ifdef _DEBUG
int g_downwardsPtrs = 0;
#endif

using namespace nif;

template<typename T>
inline File::ObjectPair File::make_NiObject(const Niflib::Ref<Niflib::NiObject>& native)
{
	//We will be returning two shared_ptrs to the same resource, but aliased to different members.
	//One member is an object of type T, the other is a Niflib::Ref to a Niflib::NiObject.
	//We will alias the second shared_ptr to the object *referenced* by the Niflib::Ref.
	//This is safe since we know that the resource that the shared_ptr is managing keeps a 
	//Niflib::Ref to the object it is pointing to.

	struct NiObjectBlock
	{
		Niflib::Ref<Niflib::NiObject> native;
		T object;
	};

	auto block = std::make_shared<NiObjectBlock>();
	if (native) {
		assert(native->IsDerivedType(type_map<T>::type::TYPE));//Or we are mapped incorrectly
		block->native = native;
	}
	else
		block->native = new typename type_map<T>::type();

	return { std::shared_ptr<NiObject>(block, &block->object),
		std::shared_ptr<Niflib::NiObject>(block, block->native) };
}

template<> [[nodiscard]] std::shared_ptr<NiObject> File::create() { return make_ni<NiObject>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiObjectNET> File::create() { return make_ni<NiObjectNET>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiAVObject> File::create() { return make_ni<NiAVObject>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiNode> File::create() { return make_ni<NiNode>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiBillboardNode> File::create() { return make_ni<NiBillboardNode>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSFadeNode> File::create() { return make_ni<BSFadeNode>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiProperty> File::create() { return make_ni<NiProperty>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiAlphaProperty> File::create() { return make_ni<NiAlphaProperty>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSShaderProperty> File::create() { return make_ni<BSShaderProperty>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSEffectShaderProperty> File::create() { return make_ni<BSEffectShaderProperty>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiBoolData> File::create() { return make_ni<NiBoolData>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiFloatData> File::create() { return make_ni<NiFloatData>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiInterpolator> File::create() { return make_ni<NiInterpolator>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiBoolInterpolator> File::create() { return make_ni<NiBoolInterpolator>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiFloatInterpolator> File::create() { return make_ni<NiFloatInterpolator>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiBlendInterpolator> File::create() { return make_ni<NiBlendInterpolator>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiBlendBoolInterpolator> File::create() { return make_ni<NiBlendBoolInterpolator>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiBlendFloatInterpolator> File::create() { return make_ni<NiBlendFloatInterpolator>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiTimeController> File::create() { return make_ni<NiTimeController>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiSingleInterpController> File::create() { return make_ni<NiSingleInterpController>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiControllerManager> File::create() { return make_ni<NiControllerManager>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiControllerSequence> File::create() { return make_ni<NiControllerSequence>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiDefaultAVObjectPalette> File::create() { return make_ni<NiDefaultAVObjectPalette>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiParticleSystem> File::create() { return make_ni<NiParticleSystem>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysData> File::create() { return make_ni<NiPSysData>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiPSysModifier> File::create() { return make_ni<NiPSysModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysAgeDeathModifier> File::create() { return make_ni<NiPSysAgeDeathModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysBoundUpdateModifier> File::create() { return make_ni<NiPSysBoundUpdateModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysGravityModifier> File::create() { return make_ni<NiPSysGravityModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysPositionModifier> File::create() { return make_ni<NiPSysPositionModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysRotationModifier> File::create() { return make_ni<NiPSysRotationModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSPSysScaleModifier> File::create() { return make_ni<BSPSysScaleModifier>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSPSysSimpleColorModifier> File::create() { return make_ni<BSPSysSimpleColorModifier>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiPSysEmitter> File::create() { return make_ni<NiPSysEmitter>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysVolumeEmitter> File::create() { return make_ni<NiPSysVolumeEmitter>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysBoxEmitter> File::create() { return make_ni<NiPSysBoxEmitter>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysCylinderEmitter> File::create() { return make_ni<NiPSysCylinderEmitter>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysSphereEmitter> File::create() { return make_ni<NiPSysSphereEmitter>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiPSysUpdateCtlr> File::create() { return make_ni<NiPSysUpdateCtlr>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysModifierCtlr> File::create() { return make_ni<NiPSysModifierCtlr>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysEmitterCtlr> File::create() { return make_ni<NiPSysEmitterCtlr>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiPSysGravityStrengthCtlr> File::create() { return make_ni<NiPSysGravityStrengthCtlr>(nullptr); }

template<> [[nodiscard]] std::shared_ptr<NiExtraData> File::create() { return make_ni<NiExtraData>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiStringExtraData> File::create() { return make_ni<NiStringExtraData>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiStringsExtraData> File::create() { return make_ni<NiStringsExtraData>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<NiTextKeyExtraData> File::create() { return make_ni<NiTextKeyExtraData>(nullptr); }
template<> [[nodiscard]] std::shared_ptr<BSBehaviorGraphExtraData> File::create() { return make_ni<BSBehaviorGraphExtraData>(nullptr); }

std::map<size_t, nif::File::CreateFcn> nif::File::s_typeRegistry;
void nif::File::registerTypes()
{
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObject::TYPE)] = &make_NiObject<nif::NiObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiObjectNET::TYPE)] = &make_NiObject<nif::NiObjectNET>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAVObject::TYPE)] = &make_NiObject<nif::NiAVObject>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiNode::TYPE)] = &make_NiObject<nif::NiNode>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiBillboardNode::TYPE)] = &make_NiObject<nif::NiBillboardNode>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSFadeNode::TYPE)] = &make_NiObject<nif::BSFadeNode>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiProperty::TYPE)] = &make_NiObject<nif::NiProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiAlphaProperty::TYPE)] = &make_NiObject<nif::NiAlphaProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSShaderProperty::TYPE)] = &make_NiObject<nif::BSShaderProperty>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSEffectShaderProperty::TYPE)] = &make_NiObject<nif::BSEffectShaderProperty>;

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

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiControllerManager::TYPE)] = &make_NiObject<nif::NiControllerManager>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiControllerSequence::TYPE)] = &make_NiObject<nif::NiControllerSequence>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiDefaultAVObjectPalette::TYPE)] = &make_NiObject<nif::NiDefaultAVObjectPalette>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiParticleSystem::TYPE)] = &make_NiObject<nif::NiParticleSystem>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysData::TYPE)] = &make_NiObject<nif::NiPSysData>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifier::TYPE)] = &make_NiObject<nif::NiPSysModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoundUpdateModifier::TYPE)] = &make_NiObject<nif::NiPSysBoundUpdateModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysAgeDeathModifier::TYPE)] = &make_NiObject<nif::NiPSysAgeDeathModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysPositionModifier::TYPE)] = &make_NiObject<nif::NiPSysPositionModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysGravityModifier::TYPE)] = &make_NiObject<nif::NiPSysGravityModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysRotationModifier::TYPE)] = &make_NiObject<nif::NiPSysRotationModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysScaleModifier::TYPE)] = &make_NiObject<nif::BSPSysScaleModifier>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSPSysSimpleColorModifier::TYPE)] = &make_NiObject<nif::BSPSysSimpleColorModifier>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitter::TYPE)] = &make_NiObject<nif::NiPSysEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysVolumeEmitter::TYPE)] = &make_NiObject<nif::NiPSysVolumeEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysBoxEmitter::TYPE)] = &make_NiObject<nif::NiPSysBoxEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysCylinderEmitter::TYPE)] = &make_NiObject<nif::NiPSysCylinderEmitter>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysSphereEmitter::TYPE)] = &make_NiObject<nif::NiPSysSphereEmitter>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysUpdateCtlr::TYPE)] = &make_NiObject<nif::NiPSysUpdateCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysModifierCtlr::TYPE)] = &make_NiObject<nif::NiPSysModifierCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysEmitterCtlr::TYPE)] = &make_NiObject<nif::NiPSysEmitterCtlr>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiPSysGravityStrengthCtlr::TYPE)] = &make_NiObject<nif::NiPSysGravityStrengthCtlr>;

	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiExtraData::TYPE)] = &make_NiObject<nif::NiExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiStringExtraData::TYPE)] = &make_NiObject<nif::NiStringExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiStringsExtraData::TYPE)] = &make_NiObject<nif::NiStringsExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::NiTextKeyExtraData::TYPE)] = &make_NiObject<nif::NiTextKeyExtraData>;
	s_typeRegistry[std::hash<const Niflib::Type*>{}(&Niflib::BSBehaviorGraphExtraData::TYPE)] = &make_NiObject<nif::BSBehaviorGraphExtraData>;
}

namespace Niflib
{
	NiObjectRef FindRoot(std::vector<NiObjectRef> const& objects);
}

nif::File::File(Version version) : m_version{ version }
{
	m_rootNode = create<BSFadeNode>();
	m_rootNode->flags.raise(14);
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

		if (auto node = Niflib::DynamicCast<Niflib::NiNode>(Niflib::FindRoot(objects)))
			m_rootNode = make_ni<NiNode>(node);

		//All objects should have strong refs by now; empty the temp storage
		m_tmpStorage.clear();
	}
}

nif::File::~File()
{
}

void nif::File::write(const std::filesystem::path& path)
{
	if (m_rootNode && !path.empty()) {
		if (auto it = m_objectIndex.find(m_rootNode.get()); it != m_objectIndex.end()) {
			if (auto native = it->second.lock()) {

				ForwardingWriteSyncer syncer(*this);
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
				Niflib::WriteNifTree(out, native.get(), fileInfo);
			}
		}
	}
}

void nif::File::keepAlive(const std::shared_ptr<NiObject>& obj)
{
	m_tmpStorage.push_back(obj);
}
