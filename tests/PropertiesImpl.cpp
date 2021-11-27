#include "pch.h"
#include "CppUnitTest.h"
#include "ConnectorTester.h"
#include "FactoryTester.h"
#include "ForwardTester.h"
#include "Constructor.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool objects::FactoryTester<BSEffectShaderProperty>::operator()(const BSEffectShaderProperty& obj, const TestConstructor& ctor)
{
	Assert::IsTrue(ctor.node.first == &obj);
	Assert::IsNotNull(dynamic_cast<node::EffectShader*>(ctor.node.second.get()));

	return false;
}
