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

namespace file
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	using namespace nif;

	TEST_CLASS(ObjectIndexTests)
	{
	public:
		nif::File file{ nif::File::Version::SKYRIM_SE };

		//File should correctly map the objects it creates
		TEST_METHOD(Consistency)
		{
			Niflib::Ref<Niflib::NiObject> native0 = new Niflib::BSFadeNode;
			auto object0 = file.get<NiObject>(native0);

			auto native1 = file.getNative<NiObject>(object0.get());
			Assert::IsTrue(native0 == native1);

			auto object1 = file.get<BSFadeNode>(Niflib::StaticCast<Niflib::BSFadeNode>(native0));
			Assert::IsTrue(object0 == object1);
		}

		//Unused objects should be deleted
		TEST_METHOD(Cleanup)
		{
			Niflib::Ref<Niflib::NiObject> ref = new Niflib::BSFadeNode;
			auto shared = file.get<NiObject>(ref);

			std::weak_ptr<NiObject> weak = shared;
			Niflib::NiObject* ptr = ref;

			//Clearing the Ref should not destroy anything
			ref = nullptr;
			Assert::IsTrue(ptr->GetNumRefs() != 0);//could give a false positive

			//this should destroy the objects
			shared.reset();
			Assert::IsTrue(weak.expired());
			//How to reliably test for destruction of the niflib object?
		}
	};
}
