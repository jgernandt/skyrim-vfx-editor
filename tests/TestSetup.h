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
}
