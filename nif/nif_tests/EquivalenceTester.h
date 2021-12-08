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
#include "CppUnitTest.h"
#include "nif.h"

namespace common
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	//Test an object and a native for equivalence in all fields
	template<typename T>
	struct EquivalenceTester : VerticalTraverser<T, EquivalenceTester>
	{
		bool operator() (const T& object, const typename type_map<T>::type* native, File& file) { return true; }
	};

	template<>
	struct EquivalenceTester<NiObjectNET> : VerticalTraverser<NiObjectNET, EquivalenceTester>
	{
		bool operator() (const NiObjectNET& object, const Niflib::NiObjectNET* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiAVObject> : VerticalTraverser<NiAVObject, EquivalenceTester>
	{
		bool operator() (const NiAVObject& object, const Niflib::NiAVObject* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiNode> : VerticalTraverser<NiNode, EquivalenceTester>
	{
		bool operator() (const NiNode& object, const Niflib::NiNode* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, EquivalenceTester>
	{
		bool operator() (const NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, EquivalenceTester>
	{
		bool operator() (const BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiBoolData> : VerticalTraverser<NiBoolData, EquivalenceTester>
	{
		bool operator() (const NiBoolData& object, const Niflib::NiBoolData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiFloatData> : VerticalTraverser<NiFloatData, EquivalenceTester>
	{
		bool operator() (const NiFloatData& object, const Niflib::NiFloatData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, EquivalenceTester>
	{
		bool operator() (const NiBoolInterpolator& object, const Niflib::NiBoolInterpolator* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, EquivalenceTester>
	{
		bool operator() (const NiFloatInterpolator& object, const Niflib::NiFloatInterpolator* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiTimeController> : VerticalTraverser<NiTimeController, EquivalenceTester>
	{
		bool operator() (const NiTimeController& object, const Niflib::NiTimeController* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, EquivalenceTester>
	{
		bool operator() (const NiSingleInterpController& object, const Niflib::NiSingleInterpController* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiParticleSystem> : VerticalTraverser<NiParticleSystem, EquivalenceTester>
	{
		bool operator() (const NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysData> : VerticalTraverser<NiPSysData, EquivalenceTester>
	{
		bool operator() (const NiPSysData& object, const Niflib::NiPSysData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysModifier> : VerticalTraverser<NiPSysModifier, EquivalenceTester>
	{
		bool operator() (const NiPSysModifier& object, const Niflib::NiPSysModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, EquivalenceTester>
	{
		bool operator() (const NiPSysGravityModifier& object, const Niflib::NiPSysGravityModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, EquivalenceTester>
	{
		bool operator() (const NiPSysRotationModifier& object, const Niflib::NiPSysRotationModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, EquivalenceTester>
	{
		bool operator() (const BSPSysScaleModifier& object, const Niflib::BSPSysScaleModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, EquivalenceTester>
	{
		bool operator() (const BSPSysSimpleColorModifier& object, const Niflib::BSPSysSimpleColorModifier* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, EquivalenceTester>
	{
		bool operator() (const NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, EquivalenceTester>
	{
		bool operator() (const NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, EquivalenceTester>
	{
		bool operator() (const NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, EquivalenceTester>
	{
		bool operator() (const NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, EquivalenceTester>
	{
		bool operator() (const NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, EquivalenceTester>
	{
		bool operator() (const NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, EquivalenceTester>
	{
		bool operator() (const NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiExtraData> : VerticalTraverser<NiExtraData, EquivalenceTester>
	{
		bool operator() (const NiExtraData& object, const Niflib::NiExtraData* native, File& file);
	};

	template<>
	struct EquivalenceTester<NiStringExtraData> : VerticalTraverser<NiStringExtraData, EquivalenceTester>
	{
		bool operator() (const NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file);
	};
}