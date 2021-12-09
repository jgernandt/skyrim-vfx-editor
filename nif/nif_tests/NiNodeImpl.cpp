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
#include "EquivalenceTester.h"
#include"ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

bool common::EquivalenceTester<nif::NiNode>::operator()(const NiNode& object, const Niflib::NiNode* native, File& file)
{
	auto&& children = native->GetChildren();
	Assert::IsTrue(children.size() == object.children.size());
	for (auto&& child : children)
		Assert::IsTrue(object.children.has(file.get<NiAVObject>(child).get()));

	return true;
}

bool common::ForwardOrderTester<NiNode>::operator()(const NiNode& object, std::vector<nif::NiObject*>::iterator& it, const std::vector<nif::NiObject*>::iterator end)
{
	fwdSet(object.children, it, end);

	return true;
}

bool common::Randomiser<NiNode>::operator()(NiNode& object, File& file, std::mt19937& rng)
{
	randomiseSet(object.children, file, rng);

	return true;
}

bool common::Randomiser<NiNode>::operator()(const NiNode&, Niflib::NiNode* native, File&, std::mt19937& rng)
{
	native->ClearChildren();
	for (auto&& obj : randomObjVector<NiAVObject>(rng))
		native->AddChild(obj);

	return true;
}


bool common::EquivalenceTester<NiBillboardNode>::operator()(const NiBillboardNode& object, const Niflib::NiBillboardNode* native, File& file)
{
	Assert::IsTrue(object.mode.get() == nif_type_conversion<BillboardMode>::from(native->GetBillboardMode()));

	return true;
}

bool common::Randomiser<NiBillboardNode>::operator()(NiBillboardNode& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.mode, rng);
	return true;
}

bool common::Randomiser<NiBillboardNode>::operator()(const NiBillboardNode&, Niflib::NiBillboardNode* native, File& file, std::mt19937& rng)
{
	native->SetBillboardMode(static_cast<Niflib::BillboardMode>(randi<std::underlying_type<Niflib::BillboardMode>::type>(rng)));
	return true;
}
