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

	TEST_CLASS(Observables)
	{
	public:
		struct Listener : IListener<void>
		{
			virtual void receive(const Event<void>& f, Observable<void>&) override
			{
				m_signalled = true;
			}

			bool received()
			{
				bool result = m_signalled;
				m_signalled = false;
				return result;
			}

		private:
			bool m_signalled{ false };
		};

		TEST_METHOD(add_remove)
		{
			Observable<void> o;
			Listener l1;
			o.addListener(l1);
			o.signal(Event<void>());
			Assert::IsTrue(l1.received());

			Listener l2;
			o.addListener(l2);
			o.signal(Event<void>());
			Assert::IsTrue(l1.received());
			Assert::IsTrue(l2.received());

			o.removeListener(l2);
			o.signal(Event<void>());
			Assert::IsTrue(l1.received());
			Assert::IsFalse(l2.received());

			o.removeListener(l1);
			o.signal(Event<void>());
			Assert::IsFalse(l1.received());
		}

		TEST_METHOD(add_loop)
		{
			struct LoopListener : Listener
			{
				virtual void receive(const Event<void>& e, Observable<void>& o) override
				{
					//adds new listeners to the sender
					if (m_added) {
						o.removeListener(l1);
						o.removeListener(l2);
						m_added = false;
					}
					else {
						o.addListener(l1);
						o.addListener(l2);
						m_added = true;
					}

					Listener::receive(e, o);
				}

				Listener l1;
				Listener l2;

			private:
				bool m_added{ false };
			};

			Observable<void> o1;
			LoopListener l1;
			LoopListener l2;
			o1.addListener(l1);
			o1.addListener(l2);

			o1.signal(Event<void>());
			Assert::IsTrue(l1.received());
			Assert::IsFalse(l1.l1.received());
			Assert::IsFalse(l1.l2.received());
			Assert::IsTrue(l2.received());
			Assert::IsFalse(l2.l1.received());
			Assert::IsFalse(l2.l2.received());

			o1.signal(Event<void>());
			Assert::IsTrue(l1.received());
			Assert::IsTrue(l1.l1.received());
			Assert::IsTrue(l1.l2.received());
			Assert::IsTrue(l2.received());
			Assert::IsTrue(l2.l1.received());
			Assert::IsTrue(l2.l2.received());
		}

		TEST_METHOD(Move_construct)
		{
			Observable<void> o1;
			Listener l;
			o1.addListener(l);

			Observable<void> o2(std::move(o1));

			o1.signal(Event<void>());
			Assert::IsFalse(l.received());

			o2.signal(Event<void>());
			Assert::IsTrue(l.received());
		}

		TEST_METHOD(Move_assign)
		{
			Observable<void> o1;
			Listener l;
			o1.addListener(l);

			Observable<void> o2 = std::move(o1);

			o1.signal(Event<void>());
			Assert::IsFalse(l.received());

			o2.signal(Event<void>());
			Assert::IsTrue(l.received());
		}
	};
}
