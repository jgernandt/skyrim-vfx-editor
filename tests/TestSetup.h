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
#include "nif.h"

namespace objects
{
	template<typename T>
	struct TestSetup : VerticalTraverser<T, TestSetup>
	{
		//Should prepare object to have a certain structure
		bool operator() (T&, File&) { return false; }
	};

	template<>
	struct TestSetup<NiObjectNET> : VerticalTraverser<NiObjectNET, TestSetup>
	{
		bool operator() (NiObjectNET& obj, File& file);
	};

	template<>
	struct TestSetup<NiNode> : VerticalTraverser<NiNode, TestSetup>
	{
		bool operator() (NiNode& obj, File& file);
	};

	template<>
	struct TestSetup<NiParticleSystem> : VerticalTraverser<NiParticleSystem, TestSetup>
	{
		bool operator() (NiParticleSystem& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysModifier> : VerticalTraverser<NiPSysModifier, TestSetup>
	{
		bool operator() (NiPSysModifier& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, TestSetup>
	{
		bool operator() (NiPSysGravityModifier& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, TestSetup>
	{
		bool operator() (NiPSysRotationModifier& obj, File& file);
	};

	template<>
	struct TestSetup<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, TestSetup>
	{
		bool operator() (BSPSysScaleModifier&, File&) { return true; }//Fall through to setup ctlrs
	};

	template<>
	struct TestSetup<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, TestSetup>
	{
		bool operator() (BSPSysSimpleColorModifier&, File&) { return true; }//Fall through to setup ctlrs
	};

	template<>
	struct TestSetup<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, TestSetup>
	{
		bool operator() (NiPSysEmitter& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, TestSetup>
	{
		bool operator() (NiPSysVolumeEmitter& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, TestSetup>
	{
		bool operator() (NiPSysBoxEmitter& obj, File& file) { return TestSetup<NiPSysVolumeEmitter>::operator() (obj, file); }
	};

	template<>
	struct TestSetup<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, TestSetup>
	{
		bool operator() (NiPSysCylinderEmitter& obj, File& file) { return TestSetup<NiPSysVolumeEmitter>::operator() (obj, file); }
	};

	template<>
	struct TestSetup<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, TestSetup>
	{
		bool operator() (NiPSysSphereEmitter& obj, File& file) { return TestSetup<NiPSysVolumeEmitter>::operator() (obj, file); }
	};

	template<>
	struct TestSetup<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, TestSetup>
	{
		bool operator() (NiPSysEmitterCtlr& obj, File& file);
	};

	template<>
	struct TestSetup<NiPSysGravityStrengthCtlr> : VerticalTraverser<NiPSysGravityStrengthCtlr, TestSetup>
	{
		bool operator() (NiPSysGravityStrengthCtlr& obj, File& file);
	};
}
