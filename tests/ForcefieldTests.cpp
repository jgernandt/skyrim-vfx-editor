#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(NiPSysGravityModifierTests)
	{
		File file{ File::Version::SKYRIM_SE };
		std::mt19937 m_engine;

		TEST_METHOD(GravityObject)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			auto factory = [this]() { return file.create<NiNode>(); };
			AssignableTest<NiNode>(obj->gravityObject(), factory);
		}
		TEST_METHOD(GravityAxis)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::Floats<3>>(obj->gravityAxis(), m_engine);
		}
		TEST_METHOD(Decay)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->decay(), m_engine);
		}
		TEST_METHOD(Strength)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->strength(), m_engine);
		}
		TEST_METHOD(ForceType)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<nif::ForceType>(obj->forceType(), m_engine);
		}
		TEST_METHOD(Turbulence)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->turbulence(), m_engine);
		}
		TEST_METHOD(TurbulenceScale)
		{
			auto obj = file.create<NiPSysGravityModifier>();
			Assert::IsNotNull(obj.get());

			PropertyTest<float>(obj->turbulenceScale(), m_engine);
		}
	};
}

namespace node
{
	TEST_CLASS(GravityModifierTests)
	{
		//We should send a constant movement requirement
		TEST_METHOD(MovementRequirementTest)
		{
			class TestConnector : public Receiver<void>, public Sender<IModifiable>, public gui::SingleConnector
			{
			public:
				TestConnector(IModifiable& ifc) : Sender<IModifiable>(ifc), SingleConnector(*this, *this) {}
			};

			nif::File file{ nif::File::Version::SKYRIM_SE };

			MockModifiable target;
			TestRoot root;
			GravityModifier* mod1 = root.newChild<PlanarForceField>(file);

			gui::Connector* c1 = nullptr;
			gui::Connector* c2 = nullptr;
			if (Field* f1 = mod1->getField(Modifier::TARGET))
				if (f1->connector) {
					c1 = f1->connector;
					c2 = root.newChild<TestConnector>(target);
					c1->onClick();
					c2->onRelease();
				}

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 1);

			if (c1)
				c1->disconnect();

			Assert::IsTrue(target.requirements().count(Modifier::Requirement::MOVEMENT) == 0);
		}

		//Gravity object should receive Assignable<NiNode>, single
		TEST_METHOD(GravityObject)
		{
			nif::File file{ nif::File::Version::SKYRIM_SE };

			std::unique_ptr<GravityModifier> node = std::make_unique<PlanarForceField>(file);
			IAssignable<nif::NiNode>& ass = node->object().gravityObject();

			auto factory = [&file]() { return file.create<nif::NiNode>(); };
			AssignableSenderTest(GravityModifier::GRAVITY_OBJECT, false, std::move(node), ass, factory);
		}
	};
}