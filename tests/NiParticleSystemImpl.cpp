#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::ConnectorTester<NiParticleSystem>::operator()(NiParticleSystem& obj, File& file)
{
	obj.data.assign(file.create<NiPSysData>());

	obj.modifiers.clear();
	obj.modifiers.insert(0, file.create<NiPSysModifier>());
	obj.modifiers.insert(1, file.create<NiPSysModifier>());

	obj.shaderProperty.assign(file.create<BSShaderProperty>());

	obj.alphaProperty.assign(file.create<NiAlphaProperty>());

	return false;
}

bool objects::ConnectorTester<NiParticleSystem>::operator()(const NiParticleSystem& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.connections.size() == 2);

	//Should *not* connect to data. This will never be made into a separate node.

	//Should connect the modifiers
	Assert::IsTrue(ctor.modConnections.second.size() == 2);
	Assert::IsTrue(ctor.modConnections.first == &obj);
	Assert::IsTrue(ctor.modConnections.second[0] == obj.modifiers.at(0).get());
	Assert::IsTrue(ctor.modConnections.second[1] == obj.modifiers.at(1).get());

	//Should connect to the shader property
	Assert::IsTrue(ctor.connections[0].object1 == &obj);
	Assert::IsTrue(ctor.connections[0].field1 == node::ParticleSystem::SHADER);
	Assert::IsTrue(ctor.connections[0].object2 == obj.shaderProperty.assigned().get());
	Assert::IsTrue(ctor.connections[0].field2 == node::EffectShader::GEOMETRY);

	//Should connect to the alpha property. This is future-proofing, we will likely allow separate alpha nodes later.
	Assert::IsTrue(ctor.connections[1].object1 == &obj);
	Assert::IsTrue(ctor.connections[1].field1 == node::ParticleSystem::ALPHA);
	Assert::IsTrue(ctor.connections[1].object2 == obj.alphaProperty.assigned().get());
	Assert::IsTrue(ctor.connections[1].field2 == "");

	return false;
}

//The way we've made it so far, Default creates any missing objects. Factory doesn't need to worry about that.
//However, Factory is expected to sort the modifiers in order before passing to Default (does this division make sense?).
//Our FactoryTest must make sure mods are sorted.
void objects::FactoryTest<NiParticleSystem>::run()
{
	File file(File::Version::SKYRIM_SE);
	TestConstructor ctor(file);

	auto psys = file.create<NiParticleSystem>();

	auto mod23 = file.create<NiPSysModifier>();
	mod23->order.set(23);
	psys->modifiers.insert(psys->modifiers.size(), mod23);

	auto mod1_1 = file.create<NiPSysModifier>();
	mod1_1->order.set(1);
	psys->modifiers.insert(psys->modifiers.size(), mod1_1);

	auto mod14 = file.create<NiPSysModifier>();
	mod14->order.set(14);
	psys->modifiers.insert(psys->modifiers.size(), mod14);

	auto mod1_2 = file.create<NiPSysModifier>();
	mod1_2->order.set(1);
	psys->modifiers.insert(psys->modifiers.size(), mod1_2);

	ctor.pushObject(psys);
	node::Factory<NiParticleSystem>{}.up(*psys, ctor);

	//Make sure mods were sorted according to their order, and that order was set to their index.
	for (int i = 0; i < psys->modifiers.size(); i++)
		Assert::IsTrue(psys->modifiers.at(i)->order.get() == i);

	int i_mod23 = psys->modifiers.find(mod23.get());
	int i_mod1_1 = psys->modifiers.find(mod1_1.get());
	int i_mod14 = psys->modifiers.find(mod14.get());
	int i_mod1_2 = psys->modifiers.find(mod1_2.get());
	Assert::IsTrue(i_mod1_1 >= 0 && i_mod1_1 < i_mod1_2);
	Assert::IsTrue(i_mod1_2 >= 0 && i_mod1_2 < i_mod14);
	Assert::IsTrue(i_mod14 >= 0 && i_mod14 < i_mod23);
	Assert::IsTrue(i_mod23 >= 0);

	//And that a node was added, obviously
	Assert::IsTrue(ctor.node.first == psys.get());
	Assert::IsNotNull(dynamic_cast<node::ParticleSystem*>(ctor.node.second.get()));
}

bool objects::ForwardTester<NiParticleSystem>::operator()(NiParticleSystem& obj, TestConstructor& ctor, File& file)
{
	obj.data.assign(file.create<NiPSysData>());

	obj.modifiers.clear();
	obj.modifiers.insert(0, file.create<NiPSysModifier>());
	obj.modifiers.insert(1, file.create<NiPSysModifier>());

	obj.shaderProperty.assign(file.create<BSShaderProperty>());

	obj.alphaProperty.assign(file.create<NiAlphaProperty>());

	return false;
}

bool objects::ForwardTester<NiParticleSystem>::operator()(const NiParticleSystem& obj, const TestConstructor& ctor)
{
	//Should forward to modifiers and properties. Not data, no point.
	Assert::IsTrue(ctor.forwards.size() == 4);
	Assert::IsTrue(ctor.forwards[0] == obj.modifiers.at(0).get());
	Assert::IsTrue(ctor.forwards[1] == obj.modifiers.at(1).get());
	Assert::IsTrue(ctor.forwards[2] == obj.shaderProperty.assigned().get());
	Assert::IsTrue(ctor.forwards[3] == obj.alphaProperty.assigned().get());

	return false;
}
