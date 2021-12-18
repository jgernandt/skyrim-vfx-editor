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
#include "CommonTests.h"

#include "nodes_internal.h"
#include "AnimationManager.h"

namespace nodes
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	TEST_CLASS(AnimationManager)
	{
	public:

		class MockAnimationManager : public HorizontalTraverser<MockAnimationManager>
		{
		public:
			std::vector<NiObject*> visited;
			std::vector<node::AnimationManager::BlockInfo> blocks;
			std::vector<ni_ptr<NiAVObject>> objectMap;
			ControlledBlock* current{ nullptr };

			template<typename T>
			void invoke(T& obj)
			{
				//log obj so we can track who was visited
				visited.push_back(&obj);
			}

			void registerBlock(const node::AnimationManager::BlockInfo& b)
			{
				blocks.push_back(b);
			}
			void unregisterBlock(const node::AnimationManager::BlockInfo&)
			{
				Assert::Fail();
			}

			ControlledBlock* getCurrentBlock() const { return current; }
			void setCurrentBlock(ControlledBlock* block) { current = block; }

			void addObject(const ni_ptr<NiAVObject>& obj) 
			{
				if (auto it = std::find(objectMap.begin(), objectMap.end(), obj); it == objectMap.end())
					objectMap.push_back(obj);
			}
			ni_ptr<NiAVObject> findObject(const std::string& name) const 
			{
				auto up = [&name](const ni_ptr<NiAVObject>& obj) { return obj->name.get() == name; };
				if (auto it = std::find_if(objectMap.begin(), objectMap.end(), up); it != objectMap.end())
					return *it;
				else
					return ni_ptr<NiAVObject>(); 
			}
		};

		TEST_METHOD(TraverseNiNode)
		{
			//add children to object map, forward to children and to ctlr manager (last)
			//(the node itself (scene root) will not be added to then map, is that a problem?)

			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<NiNode>();

			auto ch0 = file.create<NiAVObject>();
			ch0->name.set("child0");
			obj->children.add(ch0);
			auto ch1 = file.create<NiAVObject>();
			ch1->name.set("child1");
			obj->children.add(ch1);

			obj->controllers.insert(0, file.create<NiTimeController>());
			obj->controllers.insert(1, file.create<NiControllerManager>());
			obj->controllers.insert(2, file.create<NiTimeController>());

			MockAnimationManager v;
			node::AnimationInit<NiNode>{}.up(*obj, v);

			Assert::IsTrue(v.objectMap.size() == 2);
			Assert::IsTrue(ch0 == v.objectMap[0] || ch0 == v.objectMap[1]);
			Assert::IsTrue(ch1 == v.objectMap[0] || ch1 == v.objectMap[1]);

			Assert::IsTrue(v.visited.size() == 3);
			for (auto&& child : obj->children)
				Assert::IsTrue(child.get() == v.visited[0] || child.get() == v.visited[1]);
			Assert::IsTrue(v.visited.back() == obj->controllers.at(1).get());
			Assert::IsTrue(v.blocks.empty());
		}

		TEST_METHOD(TraverseNiControllerManager)
		{
			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<NiControllerManager>();

			//Add two controller sequences with a total of 3 unique controllers
			obj->ctlrSequences.clear();
			auto seq0 = file.create<NiControllerSequence>();
			seq0->blocks.resize(2);
			auto ctlr0 = file.create<NiTimeController>();
			auto ctlr1 = file.create<NiTimeController>();
			seq0->blocks.front().controller.assign(ctlr0);
			seq0->blocks.back().controller.assign(ctlr1);
			obj->ctlrSequences.add(seq0);

			auto seq1 = file.create<NiControllerSequence>();
			seq1->blocks.resize(2);
			auto ctlr2 = file.create<NiTimeController>();
			seq1->blocks.front().controller.assign(ctlr0);
			seq1->blocks.back().controller.assign(ctlr2);
			obj->ctlrSequences.add(seq1);

			MockAnimationManager v;
			node::AnimationInit<NiControllerManager>{}.up(*obj, v);

			Assert::IsTrue(v.objectMap.empty());

			//Should have forwarded to each unique controller
			Assert::IsTrue(v.visited.size() == 3);
			for (auto&& seq : obj->ctlrSequences) {
				Assert::IsTrue(std::find(v.visited.begin(), v.visited.end(), seq->blocks.front().controller.assigned().get()) != v.visited.end());
				Assert::IsTrue(std::find(v.visited.begin(), v.visited.end(), seq->blocks.back().controller.assigned().get()) != v.visited.end());
			}
			Assert::IsTrue(v.blocks.empty());
		}

		TEST_METHOD(TraverseNiTimeController)
		{
			//should register a block with the target located and all strings forwarded
			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<NiTimeController>();
			auto av = file.create<NiAVObject>();
			av->name.set("TestNodeName");

			MockAnimationManager v;
			v.objectMap.push_back(av);

			ControlledBlock block;
			block.controller.assign(obj);
			block.nodeName.set("TestNodeName");
			block.propertyType.set("TestPropertyType");
			block.ctlrType.set("TestControllerType");
			block.ctlrID.set("TestControllerID");
			block.iplrID.set("TestInterpolatorID");
			v.setCurrentBlock(&block);

			node::AnimationInit<NiTimeController>{}.up(*obj, v);

			Assert::IsTrue(v.objectMap.size() == 1);
			Assert::IsTrue(v.visited.empty());
			Assert::IsTrue(v.blocks.size() == 1);
			Assert::IsTrue(v.blocks.front().ctlr == obj);
			Assert::IsTrue(v.blocks.front().ctlrIDProperty == nullptr);
			Assert::IsTrue(v.blocks.front().target == av);
			Assert::IsTrue(v.blocks.front().propertyType == "TestPropertyType");
			Assert::IsTrue(v.blocks.front().ctlrType == "TestControllerType");
			Assert::IsTrue(v.blocks.front().ctlrID == "TestControllerID");
			Assert::IsTrue(v.blocks.front().iplrID == "TestInterpolatorID");
		}

		TEST_METHOD(TraverseNiPSysModifierCtlr)
		{
			//should register a block with the target located and modifierName as controller id
			nif::File file(nif::File::Version::SKYRIM_SE);
			auto obj = file.create<NiPSysModifierCtlr>();
			obj->modifierName.set("TargetModifier");
			auto av = file.create<NiAVObject>();
			av->name.set("TestNodeName");

			MockAnimationManager v;
			v.objectMap.push_back(av);

			ControlledBlock block;
			block.controller.assign(obj);
			block.nodeName.set("TestNodeName");
			block.propertyType.set("TestPropertyType");
			block.ctlrType.set("TestControllerType");
			block.iplrID.set("TestInterpolatorID");
			v.setCurrentBlock(&block);

			node::AnimationInit<NiPSysModifierCtlr>{}.up(*obj, v);

			Assert::IsTrue(v.objectMap.size() == 1);
			Assert::IsTrue(v.visited.empty());
			Assert::IsTrue(v.blocks.size() == 1);
			Assert::IsTrue(v.blocks.front().ctlr == obj);
			Assert::IsTrue(v.blocks.front().ctlrIDProperty.get() == &obj->modifierName);
			Assert::IsTrue(v.blocks.front().target == av);
			//propertyType should typically be empty, but if it's not we'll just assume that
			//whoever made this file knew what they were doing
			Assert::IsTrue(v.blocks.front().propertyType == "TestPropertyType");
			Assert::IsTrue(v.blocks.front().ctlrType == "TestControllerType");
			Assert::IsTrue(v.blocks.front().ctlrID == "TargetModifier");
			Assert::IsTrue(v.blocks.front().iplrID == "TestInterpolatorID");
		}

		TEST_METHOD(RegisterBlock)
		{
			//Registering a block should populate our blocks
			File file{ File::Version::SKYRIM_SE };
			node::AnimationManager am;

			//no ctlr id property
			node::AnimationManager::BlockInfo b0;
			b0.ctlr = file.create<NiTimeController>();
			b0.target = file.create<NiAVObject>();
			b0.target->name.set("avnroeigae");
			b0.propertyType = "bnaobner";
			b0.ctlrType = "nabouner";
			b0.ctlrID = "nbstroin";
			b0.iplrID = "nbaebnoli";

			auto it0 = am.registerBlock(b0);

			Assert::IsTrue(am.blocks().size() == 1);
			Assert::IsTrue(it0 == &am.blocks().at(0));
			Assert::IsTrue(it0->controller == b0.ctlr);
			Assert::IsTrue(it0->nodeName.get() == b0.target->name.get());
			Assert::IsTrue(it0->propertyType.get() == b0.propertyType);
			Assert::IsTrue(it0->ctlrType.get() == b0.ctlrType);
			Assert::IsTrue(it0->ctlrID.get() == b0.ctlrID);
			Assert::IsTrue(it0->iplrID.get() == b0.iplrID);
			//should hear name change
			b0.target->name.set("oibviseurb");
			Assert::IsTrue(it0->nodeName.get() == b0.target->name.get());
			//should hear target change
			auto av0 = file.create<NiAVObject>();
			av0->name.set("vmaoebn");
			it0->target.assign(av0);
			Assert::IsTrue(it0->nodeName.get() == av0->name.get());

			//no duplicates
			b0.target = it0->target.assigned();
			auto it0_2 = am.registerBlock(b0);
			Assert::IsTrue(am.blocks().size() == 1);
			Assert::IsTrue(it0_2 == it0);

			//ctlr id property
			node::AnimationManager::BlockInfo b1;
			auto ctlr1 = file.create<NiPSysModifierCtlr>();
			
			b1.ctlr = ctlr1;
			b1.target = file.create<NiAVObject>();
			b1.target->name.set("nvbaosbnr");
			b1.ctlrIDProperty = make_ni_ptr(ctlr1, &NiPSysModifierCtlr::modifierName);
			b1.ctlrIDProperty->set("vnoabvnois");
			b1.propertyType = "nbutrnhu";
			b1.ctlrType = "eshbntu";
			b1.iplrID = "nbridunti";

			auto it1 = am.registerBlock(b1);

			Assert::IsTrue(am.blocks().size() == 2);
			Assert::IsTrue(it1 == &am.blocks().at(1));
			Assert::IsTrue(it1->controller == ctlr1);
			Assert::IsTrue(it1->nodeName.get() == b1.target->name.get());
			Assert::IsTrue(it1->propertyType.get() == b1.propertyType);
			Assert::IsTrue(it1->ctlrType.get() == b1.ctlrType);
			Assert::IsTrue(it1->ctlrID.get() == ctlr1->modifierName.get());
			Assert::IsTrue(it1->iplrID.get() == b1.iplrID);
			//should hear ctlrID change
			ctlr1->modifierName.set("avnleib");
			Assert::IsTrue(it1->ctlrID.get() == ctlr1->modifierName.get());

			//no duplicates
			b1.ctlrID = it1->ctlrID.get();
			auto it1_2 = am.registerBlock(b1);
			Assert::IsTrue(am.blocks().size() == 2);
			Assert::IsTrue(it1_2 == it1);

			am.unregisterBlock(it1);
			Assert::IsTrue(am.blocks().size() == 1);
			Assert::IsTrue(it0 == &am.blocks().at(0));

			am.unregisterBlock(it0);
			Assert::IsTrue(am.blocks().size() == 0);

			//Controller required
			b0.ctlr.reset();
			it0 = am.registerBlock(b0);
			Assert::IsTrue(am.blocks().size() == 0);
			Assert::IsTrue(it0 == nullptr);

			//Target required
			b0.ctlr = file.create<NiTimeController>();
			b0.target.reset();
			it0 = am.registerBlock(b0);
			Assert::IsTrue(am.blocks().size() == 0);
			Assert::IsTrue(it0 == nullptr);
		}

		TEST_METHOD(RegisterObject)
		{
			struct Listener : SequenceListener<NiAVObject>
			{
				virtual void onInsert(int pos) override { m_inserted.push_back(pos); }
				virtual void onErase(int pos) override { m_erased.push_back(pos); }

				bool wasInserted()
				{
					bool result = !m_inserted.empty();
					m_inserted.clear();
					return result;
				}
				bool wasInserted(int pos)
				{
					if (!m_inserted.empty()) {
						bool result = m_inserted.front() == pos;
						m_inserted.pop_front();
						return result;
					}
					else
						return false;
				}
				bool wasErased()
				{
					bool result = !m_erased.empty();
					m_erased.clear();
					return result;
				}
				bool wasErased(int pos)
				{
					if (!m_erased.empty()) {
						bool result = m_erased.front() == pos;
						m_erased.pop_front();
						return result;
					}
					else
						return false;
				}

			private:
				std::deque<int> m_inserted;
				std::deque<int> m_erased;
			};

			//Registering a block should populate our objects
			File file{ File::Version::SKYRIM_SE };
			node::AnimationManager am;
			Listener l;
			am.objects().addListener(l);

			node::AnimationManager::BlockInfo b0;
			b0.ctlr = file.create<NiTimeController>();
			auto av0 = file.create<NiAVObject>();
			b0.target = av0;

			auto it0 = am.registerBlock(b0);

			Assert::IsTrue(am.objects().size() == 1);
			Assert::IsTrue(am.objects().at(0) == av0);
			Assert::IsTrue(l.wasInserted());
			Assert::IsFalse(l.wasErased());

			//should hear target switching
			auto av1 = file.create<NiAVObject>();
			it0->target.assign(av1);
			Assert::IsTrue(am.objects().size() == 1);
			Assert::IsTrue(am.objects().at(0) == av1);
			//this should happen by erase/insert
			Assert::IsTrue(l.wasErased(0));
			Assert::IsFalse(l.wasErased());
			Assert::IsTrue(l.wasInserted(0));
			Assert::IsFalse(l.wasInserted());

			//Another controller on the same target should not extend the list
			node::AnimationManager::BlockInfo b1;
			b1.ctlr = file.create<NiTimeController>();
			b1.target = av1;

			auto it1 = am.registerBlock(b1);

			Assert::IsTrue(am.objects().size() == 1);
			Assert::IsTrue(am.objects().at(0) == av1);
			Assert::IsFalse(l.wasErased());
			Assert::IsFalse(l.wasInserted());

			//Switching this new target should extend the list
			it1->target.assign(av0);
			Assert::IsTrue(am.objects().size() == 2);
			Assert::IsTrue(am.objects().at(0) == av1);
			Assert::IsTrue(am.objects().at(1) == av0);
			Assert::IsFalse(l.wasErased());
			Assert::IsTrue(l.wasInserted(1));
			Assert::IsFalse(l.wasInserted());

			//Switch to same should remove one entry from list
			it0->target.assign(av0);
			Assert::IsTrue(am.objects().size() == 1);
			Assert::IsTrue(am.objects().at(0) == av0);
			Assert::IsTrue(l.wasErased(0));
			Assert::IsFalse(l.wasErased());
			Assert::IsFalse(l.wasInserted());

			//Unregister one should not remove from list
			am.unregisterBlock(it1);
			Assert::IsTrue(am.objects().size() == 1);
			Assert::IsTrue(am.objects().at(0) == av0);
			Assert::IsFalse(l.wasErased());
			Assert::IsFalse(l.wasInserted());

			//Unregister second to remove object from list
			am.unregisterBlock(it0);
			Assert::IsTrue(am.objects().size() == 0);
			Assert::IsTrue(l.wasErased());
			Assert::IsFalse(l.wasInserted());
		}
	};

	TEST_CLASS(ControllerManager)
	{
	public:

		class MockRoot final : public node::IControllableRoot
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

			MockRoot target0;
			MockRoot target;

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

		TEST_METHOD(SetAnimationManager)
		{
			//Should populate our object palette and set up to listen to the manager
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiControllerManager>();
			auto node = node::Default<node::ControllerManager>{}.create(file, obj);
			auto am = std::make_shared<node::AnimationManager>();

			am->objects().insert(0, file.create<NiAVObject>());
			am->objects().insert(1, file.create<NiAVObject>());

			node->setAnimationManager(am);
			auto&& pal = obj->objectPalette.assigned();

			Assert::IsNotNull(pal.get());
			Assert::IsTrue(pal->objects.size() == 2);
			for (size_t i = 0; i < pal->objects.size(); i++)
				Assert::IsTrue(pal->objects.at(i).assigned() == am->objects().at(i));

			//should hear insertion
			am->objects().insert(0, file.create<NiAVObject>());
			Assert::IsTrue(pal->objects.size() == 3);
			for (size_t i = 0; i < pal->objects.size(); i++)
				Assert::IsTrue(pal->objects.at(i).assigned() == am->objects().at(i));

			//should hear erase
			am->objects().erase(0);
			Assert::IsTrue(pal->objects.size() == 2);
			for (size_t i = 0; i < pal->objects.size(); i++)
				Assert::IsTrue(pal->objects.at(i).assigned() == am->objects().at(i));
		}

		TEST_METHOD(Actions)
		{
			//send Set<NiControllerSequence>, receive Ptr<NiControllerManager>

			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiControllerManager>();
			ConnectorTester<node::ControllerManager> tester(
				node::Default<node::ControllerManager>{}.create(file, obj));

			Ptr<NiControllerManager> target0;
			Ptr<NiControllerManager> target1;

			tester.tryConnect<Set<NiControllerSequence>, Ptr<NiControllerManager>>(
				node::ControllerManager::Actions::ID, true, &target0);
			auto ifc = tester.tryConnect<Set<NiControllerSequence>, Ptr<NiControllerManager>>(
				node::ControllerManager::Actions::ID, true, &target1);

			Assert::IsTrue(ifc == &obj->ctlrSequences);

			Assert::IsTrue(target0.assigned() == obj);
			Assert::IsTrue(target1.assigned() == obj);

			tester.disconnect<Ptr<NiControllerManager>>(&target0);
			Assert::IsFalse(target0.assigned() == obj);

			tester.disconnect<Ptr<NiControllerManager>>(&target1);
			Assert::IsFalse(target1.assigned() == obj);
		}
	};

	TEST_CLASS(ControllerSequence)
	{
	public:

		TEST_METHOD(Default)
		{
			{//complete
				File file{ File::Version::SKYRIM_SE };
				auto obj = file.create<NiControllerSequence>();
				obj->startTime.set(0.1f);
				obj->stopTime.set(1.5f);
				auto keys = file.create<NiTextKeyExtraData>();
				obj->textKeys.assign(keys);
				keys->keys.resize(2);
				keys->keys.front().time.set(0.1f);
				keys->keys.front().value.set("start");
				keys->keys.back().time.set(1.5f);
				keys->keys.back().value.set("end");

				auto node = node::Default<node::ControllerSequence>{}.create(file, obj);

				//keys should be unchanged
				Assert::IsTrue(obj->textKeys.assigned() == keys);
				Assert::IsTrue(keys->keys.size() == 2);

				//start/end keys should match start/stop time
				obj->startTime.set(0.0f);
				obj->stopTime.set(1.6f);
				Assert::IsTrue(keys->keys.front().time.get() == 0.0f);
				Assert::IsTrue(keys->keys.back().time.get() == 1.6f);
			}
			{//missing TextKeysData
				File file{ File::Version::SKYRIM_SE };
				auto obj = file.create<NiControllerSequence>();
				obj->startTime.set(0.1f);
				obj->stopTime.set(1.5f);

				auto node = node::Default<node::ControllerSequence>{}.create(file, obj);

				//keys should be added
				Assert::IsNotNull(obj->textKeys.assigned().get());
				Assert::IsTrue(obj->textKeys.assigned()->keys.size() == 2);
				Assert::IsTrue(obj->textKeys.assigned()->keys.front().time.get() == 0.1f);
				Assert::IsTrue(obj->textKeys.assigned()->keys.front().value.get() == "start");
				Assert::IsTrue(obj->textKeys.assigned()->keys.back().time.get() == 1.5f);
				Assert::IsTrue(obj->textKeys.assigned()->keys.back().value.get() == "end");

				//start/end keys should match start/stop time
				obj->startTime.set(0.0f);
				obj->stopTime.set(1.6f);
				Assert::IsTrue(obj->textKeys.assigned()->keys.front().time.get() == 0.0f);
				Assert::IsTrue(obj->textKeys.assigned()->keys.back().time.get() == 1.6f);
			}
			{//unexpected keys
				File file{ File::Version::SKYRIM_SE };
				auto obj = file.create<NiControllerSequence>();
				obj->startTime.set(0.1f);
				obj->stopTime.set(1.5f);
				auto keys = file.create<NiTextKeyExtraData>();
				obj->textKeys.assign(keys);
				keys->keys.resize(3);
				keys->keys.front().time.set(0.2f);
				keys->keys.front().value.set("Event0");
				keys->keys.at(1).time.set(0.2f);//"start" not matching start time, not first
				keys->keys.at(1).value.set("start");
				keys->keys.back().time.set(1.5f);
				keys->keys.back().value.set("Event1");
				//"end" missing

				auto node = node::Default<node::ControllerSequence>{}.create(file, obj);

				//Complete with end, move start to front
				Assert::IsTrue(obj->textKeys.assigned() == keys);
				Assert::IsTrue(keys->keys.size() == 4);
				Assert::IsTrue(keys->keys.at(0).time.get() == 0.1f);
				Assert::IsTrue(keys->keys.at(0).value.get() == "start");
				Assert::IsTrue(keys->keys.at(1).time.get() == 0.2f);
				Assert::IsTrue(keys->keys.at(1).value.get() == "Event0");
				Assert::IsTrue(keys->keys.at(2).time.get() == 1.5f);
				Assert::IsTrue(keys->keys.at(2).value.get() == "Event1");
				Assert::IsTrue(keys->keys.at(3).time.get() == 1.5f);
				Assert::IsTrue(keys->keys.at(3).value.get() == "end");

				//start/end keys should match start/stop time
				obj->startTime.set(0.0f);
				obj->stopTime.set(1.6f);
				Assert::IsTrue(keys->keys.front().time.get() == 0.0f);
				Assert::IsTrue(keys->keys.back().time.get() == 1.6f);
			}
		}

		TEST_METHOD(Behaviour)
		{
			//send Ptr<NiControllerManager>, receive Set<NiControllerSequence>

			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiControllerSequence>();
			ConnectorTester<node::ControllerSequence> tester(
				node::Default<node::ControllerSequence>{}.create(file, obj));

			Set<NiControllerSequence> target0;
			Set<NiControllerSequence> target;

			tester.tryConnect<Ptr<NiControllerManager>, Set<NiControllerSequence>>(
				node::ControllerSequence::Behaviour::ID, false, &target0);
			auto ifc = tester.tryConnect<Ptr<NiControllerManager>, Set<NiControllerSequence>>(
				node::ControllerSequence::Behaviour::ID, false, &target);

			Assert::IsNotNull(ifc);
			//interface should assign to obj->manager
			auto mngr = file.create<NiControllerManager>();
			mngr->target.assign(file.getRoot());
			file.getRoot()->controllers.insert(0, mngr);
			file.getRoot()->name.set("nvaeorbnv");
			ifc->assign(mngr);
			Assert::IsTrue(obj->manager.assigned() == mngr);
			//and sync the name of the root to obj->accumRootName (unsure how important this is)
			Assert::IsTrue(obj->accumRootName.get() == file.getRoot()->name.get());
			//should hear the root name
			file.getRoot()->name.set("vnabnab");
			Assert::IsTrue(obj->accumRootName.get() == file.getRoot()->name.get());
			//should hear manager target switching
			mngr->target.assign(nullptr);
			Assert::IsTrue(obj->accumRootName.get() == "");
			mngr->target.assign(file.getRoot());
			Assert::IsTrue(obj->accumRootName.get() == file.getRoot()->name.get());

			ifc->assign(nullptr);
			Assert::IsFalse(obj->manager.assigned() == mngr);
			Assert::IsFalse(obj->accumRootName.get() == file.getRoot()->name.get());

			Assert::IsFalse(target0.has(obj.get()));
			Assert::IsTrue(target.has(obj.get()));

			tester.disconnect<Set<NiControllerSequence>>(&target);
			Assert::IsFalse(target.has(obj.get()));
		}

		TEST_METHOD(SetAnimationManager)
		{
			//should populate/sync our block list

			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<NiControllerSequence>();
			auto node = node::Default<node::ControllerSequence>{}.create(file, obj);

			//manager has 3 blocks (would be cleaner to insert these manually)
			auto am = std::make_shared<node::AnimationManager>();
			auto av0 = file.create<NiAVObject>();
			av0->name.set("AV0");
			auto b0 = am->registerBlock(
				{ file.create<NiTimeController>(), nullptr, av0, "PType0", "CType0", "CID0", "IID0" });
			auto av1 = file.create<NiAVObject>();
			av1->name.set("AV1");
			auto b1 = am->registerBlock(
				//a controller that should create an interpolator for us (not sure we should be testing this here)
				{ file.create<NiPSysGravityStrengthCtlr>(), nullptr, av1, "PType1", "CType1", "CID1", "IID1" });
			auto av2 = file.create<NiAVObject>();
			av2->name.set("AV2");
			auto b2 = am->registerBlock(
				{ file.create<NiTimeController>(), nullptr, av2, "PType2", "CType2", "CID2", "IID2" });

			//Sequence has 2 of those 3, but in different order
			obj->blocks.resize(2);

			auto iplr2 = file.create<NiInterpolator>();
			obj->blocks.front().interpolator.assign(iplr2);
			obj->blocks.front().controller.assign(b2->controller);
			obj->blocks.front().nodeName.set(b2->nodeName.get());
			obj->blocks.front().propertyType.set(b2->propertyType.get());
			obj->blocks.front().ctlrType.set(b2->ctlrType.get());
			obj->blocks.front().ctlrID.set(b2->ctlrID.get());
			obj->blocks.front().iplrID.set(b2->iplrID.get());

			auto iplr0 = file.create<NiInterpolator>();
			obj->blocks.back().interpolator.assign(iplr0);
			obj->blocks.back().controller.assign(b0->controller);
			obj->blocks.back().nodeName.set(b0->nodeName.get());
			obj->blocks.back().propertyType.set(b0->propertyType.get());
			obj->blocks.back().ctlrType.set(b0->ctlrType.get());
			obj->blocks.back().ctlrID.set(b0->ctlrID.get());
			obj->blocks.back().iplrID.set(b0->iplrID.get());
			
			//should add the third and rearrange to same order
			node->setAnimationManager(am);
			Assert::IsTrue(obj->blocks.size() == 3);
			node::AnimationManager::Block* blocks[3]{ b0, b1, b2 };
			for (int i = 0; i < 3; i++) {
				Assert::IsTrue(obj->blocks.at(i).controller.assigned() == blocks[i]->controller);
				Assert::IsTrue(obj->blocks.at(i).nodeName.get() == blocks[i]->nodeName.get());
				Assert::IsTrue(obj->blocks.at(i).propertyType.get() == blocks[i]->propertyType.get());
				Assert::IsTrue(obj->blocks.at(i).ctlrType.get() == blocks[i]->ctlrType.get());
				Assert::IsTrue(obj->blocks.at(i).ctlrID.get() == blocks[i]->ctlrID.get());
				Assert::IsTrue(obj->blocks.at(i).iplrID.get() == blocks[i]->iplrID.get());
			}
			Assert::IsTrue(obj->blocks.at(0).interpolator.assigned() == iplr0);
			Assert::IsTrue(obj->blocks.at(1).interpolator.assigned() != nullptr);
			Assert::IsTrue(obj->blocks.at(2).interpolator.assigned() == iplr2);

			//should hear insertions
			auto b3 = am->registerBlock(
				{ file.create<NiTimeController>(), nullptr, file.create<NiAVObject>(), "PType3", "CType3", "CID3", "IID3" });
			Assert::IsTrue(obj->blocks.size() == 4);

			//should hear erases
			am->unregisterBlock(b1);
			Assert::IsTrue(obj->blocks.size() == 3);

			//should hear changes
			b3->nodeName.set("nvabnsmb");
			b3->propertyType.set("nbaoerbe");
			b3->ctlrType.set("aohbre");
			b3->ctlrID.set("oibnhsieb");
			b3->iplrID.set("uivanbirue");

			blocks[1] = b2;
			blocks[2] = b3;
			for (int i = 0; i < 3; i++) {
				Assert::IsTrue(obj->blocks.at(i).controller.assigned() == blocks[i]->controller);
				Assert::IsTrue(obj->blocks.at(i).nodeName.get() == blocks[i]->nodeName.get());
				Assert::IsTrue(obj->blocks.at(i).propertyType.get() == blocks[i]->propertyType.get());
				Assert::IsTrue(obj->blocks.at(i).ctlrType.get() == blocks[i]->ctlrType.get());
				Assert::IsTrue(obj->blocks.at(i).ctlrID.get() == blocks[i]->ctlrID.get());
				Assert::IsTrue(obj->blocks.at(i).iplrID.get() == blocks[i]->iplrID.get());
			}
		}
	};
}
