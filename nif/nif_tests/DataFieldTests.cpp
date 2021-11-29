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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace fields
{
	using namespace nif;

	static_assert(std::is_move_assignable<Key<float>>::value);
	static_assert(std::is_move_constructible<Key<float>>::value);

	bool operator==(const Key<float>& lhs, const Key<float>& rhs)
	{
		return lhs.time.get() == rhs.time.get() &&
			lhs.value.get() == rhs.value.get() &&
			lhs.fwdTan.get() == rhs.fwdTan.get() &&
			lhs.bwdTan.get() == rhs.bwdTan.get() &&
			lhs.tension.get() == rhs.tension.get() &&
			lhs.bias.get() == rhs.bias.get() &&
			lhs.continuity.get() == rhs.continuity.get();
	}
	bool operator!=(const Key<float>& lhs, const Key<float>& rhs) { return !(lhs == rhs); }

	void randomiseKey(Key<float>& key, std::mt19937& rng)
	{
		std::uniform_real_distribution<float> D;
		key.time.set(D(rng));
		key.value.set(D(rng));
		key.fwdTan.set(D(rng));
		key.bwdTan.set(D(rng));
		key.tension.set(D(rng));
		key.bias.set(D(rng));
		key.continuity.set(D(rng));
	}

	void copyKey(Key<float>& key, const Key<float>& from)
	{
		key.time.set(from.time.get());
		key.value.set(from.value.get());
		key.fwdTan.set(from.fwdTan.get());
		key.bwdTan.set(from.bwdTan.get());
		key.tension.set(from.tension.get());
		key.bias.set(from.bias.get());
		key.continuity.set(from.continuity.get());
	}

	//Should probably split into smaller tests, if I can be bothered

	TEST_CLASS(Assignable) 
	{
	public:

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

		TEST_METHOD(RefTest)
		{
			Listener lsnr;
			{
				Ref<NiObject> ass;
				ass.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAssigned());

				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsTrue(ass.assigned() == nullptr);

				//Assign
				ass.assign(o1);
				Assert::IsTrue(ass.assigned() == o1);
				Assert::IsTrue(lsnr.wasAssigned(o1.get()));

				//Reassigning
				ass.assign(o1);
				Assert::IsFalse(lsnr.wasAssigned(o1.get()));

				auto o2 = std::make_shared<NiObject>();

				//Assign replacer
				ass.assign(o2);
				Assert::IsFalse(ass.assigned() == o1);
				Assert::IsTrue(ass.assigned() == o2);
				Assert::IsTrue(lsnr.wasAssigned(o2.get()));

				//Assign null
				ass.assign(nullptr);
				Assert::IsFalse(ass.assigned() == o2);
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

		TEST_METHOD(PtrTest)
		{
			Listener lsnr;
			{
				Ptr<NiObject> ass;
				ass.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAssigned());

				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsTrue(ass.assigned() == nullptr);

				//Assign
				ass.assign(o1);
				Assert::IsTrue(ass.assigned() == o1);
				Assert::IsTrue(lsnr.wasAssigned(o1.get()));

				//Reassigning
				ass.assign(o1);
				Assert::IsFalse(lsnr.wasAssigned(o1.get()));

				auto o2 = std::make_shared<NiObject>();

				//Assign replacer
				ass.assign(o2);
				Assert::IsFalse(ass.assigned() == o1);
				Assert::IsTrue(ass.assigned() == o2);
				Assert::IsTrue(lsnr.wasAssigned(o2.get()));

				//Assign null
				ass.assign(nullptr);
				Assert::IsFalse(ass.assigned() == o2);
				Assert::IsTrue(lsnr.wasAssigned(nullptr));

				//Remove listener
				ass.removeListener(lsnr);
				ass.assign(o2);
				Assert::IsFalse(lsnr.wasAssigned(o2.get()));
				ass.assign(nullptr);

				//Test ownership
				ass.assign(o1);
				o1.reset();
				Assert::IsTrue(weak1.expired());
				Assert::IsTrue(ass.assigned() == nullptr);

				ass.assign(o2);
				ass.addListener(lsnr);
			}
			//Signal unassign on destruction
			Assert::IsTrue(lsnr.wasAssigned(nullptr));
		}
	};

	TEST_CLASS(FlagSet)
	{
	public:

		TEST_METHOD(FlagSetTest)
		{
			struct Listener : FlagSetListener<unsigned int>
			{
				virtual void onRaise(unsigned int flags) override
				{
					m_set = flags;
				}
				virtual void onClear(unsigned int flags) override
				{
					m_cleared = flags;
				}

				bool wasSet()
				{
					bool result = m_set != 0;
					m_set = 0;
					return result;
				}
				bool wasSet(unsigned int flags)
				{
					bool result = m_set == flags;
					m_set = 0;
					return result;
				}
				bool wasCleared()
				{
					bool result = m_cleared != 0;
					m_cleared = 0;
					return result;
				}
				bool wasCleared(unsigned int flags)
				{
					bool result = m_cleared == flags;
					m_cleared = 0;
					return result;
				}

			private:
				unsigned int m_set{ 0 };
				unsigned int m_cleared{ 0 };
			};

			constexpr unsigned int F1 = 0x0001;
			constexpr unsigned int F2 = 0x8000;

			Listener lsnr;
			{
				nif::FlagSet<unsigned int> flags;
				Assert::IsFalse(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));

				flags.addListener(lsnr);
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsFalse(lsnr.wasCleared());

				//set one
				flags.raise(F1);
				Assert::IsTrue(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsTrue(lsnr.wasSet(F1));
				Assert::IsFalse(lsnr.wasCleared());

				//reset should not call
				flags.raise(F1);
				Assert::IsTrue(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsFalse(lsnr.wasCleared());

				//clear
				flags.clear(F1);
				Assert::IsFalse(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsTrue(lsnr.wasCleared(F1));

				//clearing unset should not call
				flags.clear(F1);
				Assert::IsFalse(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsFalse(lsnr.wasCleared());

				//set two
				flags.raise(F1 | F2);
				Assert::IsTrue(flags.hasRaised(F1));
				Assert::IsTrue(flags.hasRaised(F2));
				Assert::IsTrue(flags.hasRaised(F1 | F2));
				Assert::IsTrue(lsnr.wasSet(F1 | F2));//or should this call twice?
				Assert::IsFalse(lsnr.wasCleared());

				//clear one
				flags.clear(F1);
				Assert::IsFalse(flags.hasRaised(F1));
				Assert::IsTrue(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsTrue(lsnr.wasCleared(F1));

				//Set two, one already set
				flags.raise(F1 | F2);
				Assert::IsTrue(flags.hasRaised(F1));
				Assert::IsTrue(flags.hasRaised(F2));
				Assert::IsTrue(flags.hasRaised(F1 | F2));
				Assert::IsTrue(lsnr.wasSet(F1));//only signal the one that was set
				Assert::IsFalse(lsnr.wasCleared());

				//Clear two
				flags.clear(F1 | F2);
				Assert::IsFalse(flags.hasRaised(F1));
				Assert::IsFalse(flags.hasRaised(F2));
				Assert::IsFalse(flags.hasRaised(F1 | F2));
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsTrue(lsnr.wasCleared(F1 | F2));//or should this call twice?

				//remove lsnr
				flags.removeListener(lsnr);
				flags.raise(F1 | F2);
				Assert::IsFalse(lsnr.wasSet());
				Assert::IsFalse(lsnr.wasCleared());

				flags.addListener(lsnr);
			}
			//Clear on destruction
			Assert::IsFalse(lsnr.wasSet());
			Assert::IsTrue(lsnr.wasCleared(F1 | F2));//or should this call twice?
		}
	};

	TEST_CLASS(List)
	{
	public:

		TEST_METHOD(ListTest)
		{
			struct Listener : ListListener<Key<float>>
			{
				virtual void onInsert(int pos) override
				{
					m_inserted.push_back(pos);
				}
				virtual void onErase(int pos) override
				{
					m_erased.push_back(pos);
				}

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

			Listener lsnr;
			{
				nif::List<Key<float>> list;
				list.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				std::mt19937 rng;
				std::uniform_real_distribution<float> D;

				//push_back
				list.push_back();
				Assert::IsTrue(list.size() == 1);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Key<float>* key0 = &list.back();
				Assert::IsNotNull(key0);

				list.push_back();
				Assert::IsTrue(list.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Key<float>* key1 = &list.back();
				Assert::IsNotNull(key1);

				//access
				Assert::IsTrue(key0 == &list.front() && key0 == &list.at(0));
				Assert::IsTrue(key1 != key0 && key1 == &list.at(1));

				//push_front
				list.push_front();
				Assert::IsTrue(list.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Key<float>* key_ = &list.front();
				Assert::IsTrue(key_ == &list.front() && key_ == &list.at(0));
				Assert::IsTrue(key0 == &list.at(1));
				Assert::IsTrue(key1 == &list.back() && key1 == &list.at(2));

				//pop_front
				list.pop_front();
				Assert::IsTrue(list.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.front());
				Assert::IsTrue(key1 == &list.back());

				//pop_back
				list.pop_back();
				Assert::IsTrue(list.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.front());

				//insert
				list.insert(1);
				Assert::IsTrue(list.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				key1 = &list.back();
				Assert::IsTrue(key0 == &list.front());

				list.insert(0);
				Assert::IsTrue(list.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.at(1));
				Assert::IsTrue(key1 == &list.at(2));

				//erase
				list.erase(0);
				Assert::IsTrue(list.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.at(0));
				Assert::IsTrue(key1 == &list.at(1));

				list.erase(1);
				Assert::IsTrue(list.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.at(0));

				//resize larger
				list.resize(3);
				Assert::IsTrue(list.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsTrue(lsnr.wasInserted(2));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.at(0));

				//resize smaller
				list.resize(1);
				Assert::IsTrue(list.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(2));
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == &list.at(0));

				//clear
				list.insert(1);
				Assert::IsTrue(lsnr.wasInserted());
				list.clear();
				Assert::IsTrue(list.size() == 0);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());

				//Remove listener
				list.removeListener(lsnr);
				list.push_back();
				list.push_back();
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				list.addListener(lsnr);
			}
			//signal erase on destruction
			Assert::IsFalse(lsnr.wasInserted());
			Assert::IsTrue(lsnr.wasErased(1));
			Assert::IsTrue(lsnr.wasErased(0));
			Assert::IsFalse(lsnr.wasErased());
		}
	};

	TEST_CLASS(Property)
	{
	public:

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

			nif::Property<float> prop;
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
	};

	TEST_CLASS(Sequence)
	{
	public:

		TEST_METHOD(SequenceTest)
		{
			struct Listener : SequenceListener<NiObject>
			{
				virtual void onInsert(int pos) override
				{
					m_inserted.push_back(pos);
				}
				virtual void onErase(int pos) override
				{
					m_erased.push_back(pos);
				}

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

			Listener lsnr;
			{
				nif::Sequence<NiObject> seq;
				seq.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				auto o1 = std::make_shared<NiObject>();
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsTrue(seq.find(o1.get()) == -1);

				//Insert in empty
				Assert::IsTrue(seq.insert(seq.size(), o1) == 0);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.size() == 1);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				auto o2 = std::make_shared<NiObject>();
				Assert::IsTrue(seq.find(o2.get()) == -1);

				//Insert at end
				Assert::IsTrue(seq.insert(seq.size(), o2) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				auto o3 = std::make_shared<NiObject>();
				Assert::IsTrue(seq.find(o3.get()) == -1);

				//Insert in middle
				Assert::IsTrue(seq.insert(1, o3) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 0);
				Assert::IsTrue(seq.find(o2.get()) == 2);
				Assert::IsTrue(seq.find(o3.get()) == 1);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				//Erase at beginning
				Assert::IsTrue(seq.erase(0) == 0);
				Assert::IsTrue(seq.find(o1.get()) == -1);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());

				//Insert (past the end - forbid this!) AT the end (again)
				Assert::IsTrue(seq.insert(seq.size(), o1) == 2);
				Assert::IsTrue(seq.find(o1.get()) == 2);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(2));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				//Reinsert in new position leaves sequence unchanged
				Assert::IsTrue(seq.insert(0, o2) == 1);
				Assert::IsTrue(seq.find(o1.get()) == 2);
				Assert::IsTrue(seq.find(o2.get()) == 1);
				Assert::IsTrue(seq.find(o3.get()) == 0);
				Assert::IsTrue(seq.size() == 3);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				//clear
				seq.clear();
				Assert::IsTrue(seq.find(o1.get()) == -1);
				Assert::IsTrue(seq.find(o2.get()) == -1);
				Assert::IsTrue(seq.find(o3.get()) == -1);
				Assert::IsTrue(seq.size() == 0);
				Assert::IsFalse(lsnr.wasInserted());
				//calls should be sent back to front
				Assert::IsTrue(lsnr.wasErased(2));
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());

				//Test ownership
				seq.insert(seq.size(), o3);
				seq.insert(seq.size(), o2);
				seq.insert(seq.size(), o1);
				Assert::IsTrue(lsnr.wasInserted());
				o1.reset();
				Assert::IsFalse(weak1.expired());
				Assert::IsTrue(seq.erase(2) == 2);//erase o1
				Assert::IsTrue(weak1.expired());
				Assert::IsTrue(seq.size() == 2);
				Assert::IsTrue(lsnr.wasErased(2));
				Assert::IsFalse(lsnr.wasErased());

				//Remove listener
				seq.removeListener(lsnr);
				Assert::IsTrue(seq.erase(1) == 1);
				Assert::IsFalse(lsnr.wasErased());

				seq.insert(seq.size(), o2);
				seq.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
			}
			//signal erase on destruction
			Assert::IsFalse(lsnr.wasInserted());
			Assert::IsTrue(lsnr.wasErased(1));
			Assert::IsTrue(lsnr.wasErased(0));
			Assert::IsFalse(lsnr.wasErased());
		}
	};

	TEST_CLASS(Set)
	{
	public:

		TEST_METHOD(SetTest)
		{
			struct Listener : SetListener<NiObject>
			{
				virtual void onAdd(NiObject* obj) override
				{
					m_added.insert(obj);
				}
				virtual void onRemove(NiObject* obj)
				{
					m_removed.insert(obj);
				}

				bool wasAdded()
				{
					bool result = !m_added.empty();
					m_added.clear();
					return result;
				}
				bool wasAdded(NiObject* obj)
				{
					if (auto it = m_added.find(obj); it != m_added.end()) {
						m_added.erase(it);
						return true;
					}
					else
						return false;
				}
				bool wasRemoved()
				{
					bool result = !m_removed.empty();
					m_removed.clear();
					return result;
				}
				bool wasRemoved(NiObject* obj)
				{
					if (auto it = m_removed.find(obj); it != m_removed.end()) {
						m_removed.erase(it);
						return true;
					}
					else
						return false;
				}

			private:
				std::set<NiObject*> m_added;
				std::set<NiObject*> m_removed;
			};

			Listener lsnr;
			auto o1 = std::make_shared<NiObject>();
			auto o2 = std::make_shared<NiObject>();
			{
				nif::Set<NiObject> set;
				set.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Add null
				set.add(nullptr);
				Assert::IsFalse(set.has(nullptr));
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Add object
				std::weak_ptr<NiObject> weak1 = o1;
				Assert::IsFalse(set.has(o1.get()));

				set.add(o1);
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsTrue(set.size() == 1);
				Assert::IsTrue(lsnr.wasAdded(o1.get()));
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//Add second object
				Assert::IsFalse(set.has(o2.get()));

				set.add(o2);
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsTrue(set.has(o2.get()));
				Assert::IsTrue(set.size() == 2);
				Assert::IsTrue(lsnr.wasAdded(o2.get()));
				Assert::IsFalse(lsnr.wasAdded());
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
				Assert::IsFalse(lsnr.wasRemoved());

				//Re-removing should do nothing
				set.remove(o2.get());
				Assert::IsTrue(set.has(o1.get()));
				Assert::IsFalse(set.has(o2.get()));
				Assert::IsTrue(set.size() == 1);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				//clear
				set.add(o2);
				Assert::IsTrue(lsnr.wasAdded());
				set.clear();
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsTrue(lsnr.wasRemoved(o1.get()));
				Assert::IsTrue(lsnr.wasRemoved(o2.get()));
				Assert::IsFalse(lsnr.wasRemoved());

				set.add(o1);
				set.add(o2);
				Assert::IsTrue(lsnr.wasAdded());

				//Test ownership
				auto raw1 = o1.get();
				o1.reset();
				Assert::IsFalse(weak1.expired());
				set.remove(raw1);
				Assert::IsTrue(weak1.expired());
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsTrue(lsnr.wasRemoved());

				//Remove listener
				set.removeListener(lsnr);
				set.add(o2);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());

				o1 = std::make_shared<NiObject>();
				set.add(o1);
				set.addListener(lsnr);
				Assert::IsFalse(lsnr.wasAdded());
				Assert::IsFalse(lsnr.wasRemoved());
			}

			//signal remove on destruction
			Assert::IsFalse(lsnr.wasAdded());
			Assert::IsTrue(lsnr.wasRemoved(o1.get()));
			Assert::IsTrue(lsnr.wasRemoved(o2.get()));
			Assert::IsFalse(lsnr.wasRemoved());
		}
	};

	TEST_CLASS(Vector)
	{
	public:

		TEST_METHOD(VectorTest)
		{
			struct Listener : VectorListener<Key<float>>
			{
				virtual void onInsert(int pos) override
				{
					m_inserted.push_back(pos);
				}
				virtual void onErase(int pos) override
				{
					m_erased.push_back(pos);
				}

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

			struct PListener : PropertyListener<float>
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

			Listener lsnr;
			PListener plsnr;
			{
				nif::Vector<Key<float>> vec;
				vec.addListener(lsnr);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				std::mt19937 rng;

				//push_back
				vec.push_back();
				Assert::IsTrue(vec.size() == 1);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				randomiseKey(vec.at(0), rng);
				Key<float> key0;
				copyKey(key0, vec.at(0));

				//listen to this key
				vec.at(0).time.addListener(plsnr);
				Key<float>* key0_ptr = &vec.at(0);

				vec.push_back();
				Assert::IsTrue(vec.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				randomiseKey(vec.at(1), rng);
				Key<float> key1;
				copyKey(key1, vec.at(1));

				//access
				Assert::IsTrue(key0 == vec.front() && key0 == vec.at(0));
				Assert::IsTrue(key1 == vec.at(1) && key1 == vec.back());

				//push_front
				vec.push_front();
				Assert::IsTrue(vec.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(1));
				Assert::IsTrue(key1 == vec.at(2));

				//pop_front
				vec.pop_front();
				Assert::IsTrue(vec.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.front());
				Assert::IsTrue(key1 == vec.back());

				//pop_back
				vec.pop_back();
				Assert::IsTrue(vec.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.front());

				//insert
				vec.insert(1);
				Assert::IsTrue(vec.size() == 2);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				randomiseKey(vec.at(1), rng);
				copyKey(key1, vec.at(1));
				Assert::IsTrue(key0 == vec.front());

				vec.insert(0);
				Assert::IsTrue(vec.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(0));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(1));
				Assert::IsTrue(key1 == vec.at(2));

				//erase
				vec.erase(0);
				Assert::IsTrue(vec.size() == 2);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(0));
				Assert::IsTrue(key1 == vec.at(1));

				vec.erase(1);
				Assert::IsTrue(vec.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(0));

				//resize larger
				vec.resize(3);
				Assert::IsTrue(vec.size() == 3);
				Assert::IsTrue(lsnr.wasInserted(1));
				Assert::IsTrue(lsnr.wasInserted(2));
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(0));

				//resize smaller
				vec.resize(1);
				Assert::IsTrue(vec.size() == 1);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(2));
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsFalse(lsnr.wasErased());
				Assert::IsTrue(key0 == vec.at(0));

				//make sure we have not lost the listener (despite moving the property)
				Assert::IsTrue(&vec.at(0) != key0_ptr);
				Assert::IsFalse(plsnr.wasSet());
				vec.at(0).time.set(0.0f);
				Assert::IsTrue(plsnr.wasSet(0.0f));

				//clear
				vec.insert(1);
				Assert::IsTrue(lsnr.wasInserted());
				vec.clear();
				Assert::IsTrue(vec.size() == 0);
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsTrue(lsnr.wasErased(1));
				Assert::IsTrue(lsnr.wasErased(0));
				Assert::IsFalse(lsnr.wasErased());

				//Remove listener
				vec.removeListener(lsnr);
				vec.push_back();
				vec.push_back();
				Assert::IsFalse(lsnr.wasInserted());
				Assert::IsFalse(lsnr.wasErased());

				vec.addListener(lsnr);
			}
			//signal erase on destruction
			Assert::IsFalse(lsnr.wasInserted());
			Assert::IsTrue(lsnr.wasErased(1));
			Assert::IsTrue(lsnr.wasErased(0));
			Assert::IsFalse(lsnr.wasErased());
		}
	};
}
