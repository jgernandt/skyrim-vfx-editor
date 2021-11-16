#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nif_tests
{
	using namespace nif;

	TEST_CLASS(DataFieldTests)
	{
	public:
		
		TEST_METHOD(AssignableTest)
		{
			struct Listener : AssignableListener<NiObject>
			{
				virtual void onAssign(NiObject* obj) override
				{
					signalled = true;
					assigned = obj;
				}

				bool wasAssigned()
				{
					bool result = signalled;
					signalled = false;
					assigned = nullptr;
					return result;
				}

				bool wasAssigned(NiObject* obj)
				{
					bool result = signalled && assigned == obj;
					signalled = false;
					assigned = nullptr;
					return result;
				}

			private:
				bool signalled{ false };
				NiObject* assigned{ nullptr };
			};

			Listener lsnr;
			{
				Assignable<NiObject> ass;
				ass.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAssigned());

				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsFalse(ass.isAssigned(o1.get()));

				//Assign
				ass.assign(o1);
				Assert::IsTrue(ass.isAssigned(o1.get()));
				Assert::IsTrue(lsnr.wasAssigned(o1.get()));

				//Reassigning
				ass.assign(o1);
				Assert::IsFalse(lsnr.wasAssigned(o1.get()));

				auto o2 = std::make_shared<NiObject>();

				//Assign replacer
				ass.assign(o2);
				Assert::IsFalse(ass.isAssigned(o1.get()));
				Assert::IsTrue(ass.isAssigned(o2.get()));
				Assert::IsTrue(lsnr.wasAssigned(o2.get()));

				//Assign null
				ass.assign(nullptr);
				Assert::IsFalse(ass.isAssigned(o2.get()));
				Assert::IsTrue(lsnr.wasAssigned(nullptr));

				//Remove listener
				ass.removeListener(lsnr);
				ass.assign(o2);
				Assert::IsFalse(lsnr.wasAssigned(o2.get()));
				ass.assign(nullptr);

				//Test ownership
				ass.assign(o1);
				o1.reset();
				Assert::IsFalse(weak1.expired());
				ass.assign(nullptr);
				Assert::IsTrue(weak1.expired());

				ass.assign(o2);
				ass.addListener(lsnr);
			}
			//Signal unassign on destruction
			Assert::IsTrue(lsnr.wasAssigned(nullptr));
		}

		TEST_METHOD(PropertyTest)
		{
			struct Listener : PropertyListener<float>
			{
				virtual void onSet(const float& f) override
				{
					m_signalled = true;
					m_set = f;
				}

				bool wasSet()
				{
					bool result = m_signalled;
					m_signalled = false;
					m_set = std::numeric_limits<float>::quiet_NaN();
					return result;
				}

				bool wasSet(float f)
				{
					bool result = m_signalled && m_set == f;
					m_signalled = false;
					m_set = std::numeric_limits<float>::quiet_NaN();
					return result;
				}

			private:
				float m_set{ std::numeric_limits<float>::quiet_NaN() };
				bool m_signalled{ false };
			};

			Property<float> prop;
			Listener lsnr;
			prop.addListener(lsnr);
			Assert::IsFalse(lsnr.wasSet());

			std::mt19937 rng;
			std::uniform_real_distribution<float> D;

			for (int i = 0; i < 3; i++) {
				float val = D(rng);
				prop.set(val);

				Assert::IsTrue(prop.get() == val);
				Assert::IsTrue(lsnr.wasSet(val));

				prop.set(val);
				Assert::IsFalse(lsnr.wasSet(val));
			}

			prop.removeListener(lsnr);
			float val = D(rng);
			prop.set(val);
			Assert::IsFalse(lsnr.wasSet(val));
		}

		TEST_METHOD(SequenceTest)
		{
			struct Listener : SequenceListener<NiObject>
			{
				virtual void onInsert(int pos) override
				{
					m_inserted = pos;
				}
				virtual void onErase(int pos) override
				{
					m_erased = pos;
				}

				bool wasInserted()
				{
					bool result = m_inserted != -1;
					m_inserted = -1;
					return result;
				}

				bool wasInserted(int pos)
				{
					bool result = m_inserted == pos;
					m_inserted = -1;
					return result;
				}

				bool wasErased()
				{
					bool result = m_erased != -1;
					m_erased = -1;
					return result;
				}

				bool wasErased(int pos)
				{
					bool result = m_erased == pos;
					m_erased = -1;
					return result;
				}

			private:
				int m_inserted{ -1 };
				int m_erased{ -1 };
			};

			Listener lsnr;
			{
				Sequence<NiObject> seq;
				seq.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsFalse(seq.find(o1.get()));

				//Insert in empty
				Assert::IsTrue(seq.insert(-1, o1) == 0);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.size() == 1);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasErased());

				auto o2 = std::make_shared<NiObject>();
				Assert::IsTrue(seq.find(o2.get()) == -1);

				//Insert at -1 (end)
				Assert::IsTrue(seq.insert(-1, o2) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasErased());

				auto o3 = std::make_shared<NiObject>();
				Assert::IsTrue(seq.find(o3.get()) == -1);

				//Insert in middle
				Assert::IsTrue(seq.insert(1, o3) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.find(o2.get()) == 2);
				Assert::IsTrue(seq.find(o3.get()) == 1);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(1) == 1);
				Assert::IsFalse(lsnr.wasErased());

				//Erase at beginning
				Assert::IsTrue(seq.erase(0) == 0);
				Assert::IsTrue(seq.find(o1.get()) == -1);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));

				//Insert past the end
				Assert::IsTrue(seq.insert(10, o1) == 2);
				Assert::IsTrue(seq.find(o1.get()) == 2);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(2));
				Assert::IsFalse(lsnr.wasErased());

				//Reinsert in new position leaves sequence unchanged
				Assert::IsTrue(seq.insert(0, o2) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 2);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				//Test ownership
				o1.reset();
				Assert::IsFalse(weak1.expired());
				Assert::IsTrue(seq.erase(2) == -1);//erase o1
				Assert::IsTrue(weak1.expired());
				Assert::IsTrue(seq.size() == 2);
				Assert::IsTrue(lsnr.wasErased(2));

				//Remove listener
				seq.removeListener(lsnr);
				Assert::IsTrue(seq.erase(1) == -1);
				Assert::IsFalse(lsnr.wasErased());

				seq.addListener(lsnr);
			}
			//signal erase on destruction (we could make this test more precise)
			Assert::IsTrue(lsnr.wasErased());
		}

		TEST_METHOD(SetTest)
		{
			struct Listener : SetListener<NiObject>
			{
				virtual void onAdd(NiObject* obj) override
				{
					Assert::IsNotNull(obj);
					m_added = obj;
				}
				virtual void onRemove(NiObject* obj)
				{
					Assert::IsNotNull(obj);
					m_removed = obj;
				}

				bool wasAdded()
				{
					bool result = m_added != nullptr;
					m_added = nullptr;
					return result;
				}

				bool wasAdded(NiObject* obj)
				{
					bool result = m_added == obj;
					m_added = nullptr;
					return result;
				}

				bool wasRemoved()
				{
					bool result = m_removed != nullptr;
					m_removed = nullptr;
					return result;
				}

				bool wasRemoved(NiObject* obj)
				{
					bool result = m_removed == obj;
					m_removed = nullptr;
					return result;
				}

			private:
				NiObject* m_added{ nullptr };
				NiObject* m_removed{ nullptr };
			};

			Listener lsnr;
			{
				Set<NiObject> set;
				set.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Add null
				set.add(nullptr);
				Assert::IsFalse(set.has(nullptr));
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Add object
				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsFalse(set.has(o1.get()));

				set.add(o1);
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsTrue(set.size() == 1);
				Assert::IsTrue(lsnr.wasAdded(o1.get()));
				Assert::IsFalse(lsnr.wasRemoved());

				//Add second object
				auto o2 = std::make_shared<NiObject>();
				Assert::IsFalse(set.has(o2.get()));

				set.add(o2);
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsTrue(set.has(o2.get()));
				Assert::IsTrue(set.size() == 2);
				Assert::IsTrue(lsnr.wasAdded(o2.get()));
				Assert::IsFalse(lsnr.wasRemoved());

				//Re-adding should do nothing
				set.add(o2);
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsTrue(set.has(o2.get()));
				Assert::IsTrue(set.size() == 2);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Remove
				set.remove(o2.get());
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsFalse(set.has(o2.get()));
				Assert::IsTrue(set.size() == 1);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsTrue(lsnr.wasRemoved(o2.get()));

				//Re-removing should do nothing
				set.remove(o2.get());
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsFalse(set.has(o2.get()));
				Assert::IsTrue(set.size() == 1);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Test ownership
				o1.reset();
				Assert::IsFalse(weak1.expired());
				auto raw1 = weak1.lock().get();
				set.remove(raw1);
				Assert::IsTrue(weak1.expired());
				Assert::IsFalse(set.has(raw1));
				Assert::IsTrue(set.size() == 0);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsTrue(lsnr.wasRemoved(raw1));

				//Remove listener
				set.removeListener(lsnr);
				set.add(o2);
				Assert::IsTrue(set.has(o2.get()));
				Assert::IsFalse(lsnr.wasAdded(o2.get()));

				set.addListener(lsnr);
			}
			//signal remove on destruction (we could make this test more precise)
			Assert::IsTrue(lsnr.wasRemoved());
		}

		TEST_METHOD(VectorTest)
		{
			struct Listener : VectorListener<float>
			{
				virtual void onInsert(int pos) override
				{
					m_inserted = pos;
				}
				virtual void onErase(int pos) override
				{
					m_erased = pos;
				}

				bool wasInserted()
				{
					bool result = m_inserted != -1;
					m_inserted = -1;
					return result;
				}

				bool wasInserted(int pos)
				{
					bool result = m_inserted == pos;
					m_inserted = -1;
					return result;
				}

				bool wasErased()
				{
					bool result = m_erased != -1;
					m_erased = -1;
					return result;
				}

				bool wasErased(int pos)
				{
					bool result = m_erased == pos;
					m_erased = -1;
					return result;
				}

			private:
				int m_inserted{ -1 };
				int m_erased{ -1 };
			};

			Listener lsnr;
			{
				Vector<float> vec;
				vec.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				std::mt19937 rng;
				std::uniform_real_distribution<float> D;

				std::vector<float> expected;

				//push_back
				float val = D(rng);
				expected.push_back(val);
				vec.push_back(val);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 1);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasErased());
				val = D(rng);
				expected.push_back(val);
				vec.push_back(val);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasErased());

				//insert
				val = D(rng);
				expected.insert(expected.begin(), val);
				vec.insert(0, val);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasErased());

				val = D(rng);
				expected.insert(expected.begin() + 1, val);
				vec.insert(1, val);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 4);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasErased());

				//erase
				expected.erase(expected.end());
				vec.erase(-1);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 3);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(4));

				expected.erase(expected.begin());
				vec.erase(0);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));

				//Remove listener
				vec.removeListener(lsnr);
				expected.erase(expected.end());
				vec.erase(-1);
				Assert::IsTrue(vec == expected);
				Assert::IsTrue(vec.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				vec.addListener(lsnr);
			}
			//signal erase on destruction
			Assert::IsTrue(lsnr.wasErased(0));
		}
	};
}
