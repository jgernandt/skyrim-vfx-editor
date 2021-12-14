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
#include "CppUnitTest.h"
#include "nodes_internal.h"
#include "CommonTests.h"

namespace nodes
{
	TEST_CLASS(ControllerManager)
	{
	public:

		class MockInterface final : public node::IControllableRoot
		{
		public:
			virtual void addController(const ni_ptr<NiTimeController>& ctlr) override 
			{
				added.push_back(ctlr.get());
			}
			virtual void removeController(NiTimeController* ctlr) override
			{
				removed.push_back(ctlr);
			}

			virtual Set<NiExtraData>& extraData() override { return m_extraData; }

			std::vector<NiTimeController*> added;
			std::vector<NiTimeController*> removed;
			Set<NiExtraData> m_extraData;
		};

		TEST_METHOD(Default)
		{
			File file{ File::Version::SKYRIM_SE };
			{//missing BGED and palette
				auto obj = file.create<NiControllerManager>();

				auto node = node::Default<node::ControllerManager>{}.create(file, obj);

				Assert::IsNotNull(obj->objectPalette.assigned().get());
				Assert::IsNotNull(node->getBGED().get());
			}
			{//complete
				auto obj = file.create<NiControllerManager>();
				auto pal = file.create<NiDefaultAVObjectPalette>();
				obj->objectPalette.assign(pal);
				auto bged = file.create<BSBehaviorGraphExtraData>();

				auto node = node::Default<node::ControllerManager>{}.create(file, obj, bged);

				Assert::IsTrue(obj->objectPalette.assigned() == pal);
				Assert::IsTrue(node->getBGED() == bged);
			}
		}

		TEST_METHOD(Root)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiControllerManager>();
			auto bged = file.create<BSBehaviorGraphExtraData>();
			bged->name.set(node::BGED_NAME);
			bged->fileName.set("file");

			MockInterface target0;
			MockInterface target;

			ConnectorTester<node::ControllerManager> tester(node::Default<node::ControllerManager>{}.create(file, obj, bged));
			tester.tryConnect<Ptr<NiAVObject>, node::IControllableRoot>(node::ControllerManager::Root::ID, false, &target0);
			auto ifc = tester.tryConnect<Ptr<NiAVObject>, node::IControllableRoot>(node::ControllerManager::Root::ID, false, &target);
			Assert::IsNotNull(ifc);

			//target (not target0) should have the controller and extra data
			Assert::IsTrue(!target0.added.empty() && target0.added.front() == obj.get());
			Assert::IsTrue(!target0.removed.empty() && target0.removed.front() == obj.get());
			Assert::IsTrue(target0.extraData().size() == 0);

			Assert::IsTrue(!target.added.empty() && target.added.front() == obj.get());
			Assert::IsTrue(target.removed.empty());
			Assert::IsTrue(target.extraData().has(bged.get()));

			//Just realised that some controller managers don't use a BGED. Meaning, we should only
			//insert one if it points to a file.
			bged->fileName.set("");
			Assert::IsFalse(target.extraData().has(bged.get()));
			bged->fileName.set("newFile");
			Assert::IsTrue(target.extraData().has(bged.get()));

			//the interface should expose the object palette scene
			Assert::IsTrue(ifc == &obj->objectPalette.assigned()->scene);
			//assigning to it should also create an entry in the palette
			/*Edit: may not actually be required, let's skip for now
			auto av = file.create<NiAVObject>();
			av->name.set("ahgoregh");
			ifc->assign(av);
			Assert::IsTrue(obj->objectPalette.assigned()->objects.size() == 1);
			Assert::IsTrue(obj->objectPalette.assigned()->objects.back().name.get() == av->name.get());
			Assert::IsTrue(obj->objectPalette.assigned()->objects.back().object.assigned() == av);
			//changing name should update the palette
			av->name.set("aivbrieuvb");
			Assert::IsTrue(obj->objectPalette.assigned()->objects.back().name.get() == av->name.get());
			//unassigning should remove the entry
			ifc->assign(nullptr);
			Assert::IsTrue(obj->objectPalette.assigned()->objects.size() == 0);*/

			tester.disconnect<node::IControllableRoot>(&target);
			Assert::IsTrue(!target.removed.empty() && target.removed.front() == obj.get());
			Assert::IsTrue(target0.extraData().size() == 0);
		}
	};
}
