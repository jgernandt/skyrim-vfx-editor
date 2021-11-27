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
#include "ForwardOrderTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace math;
using namespace nif;

bool common::EquivalenceTester<NiPSysEmitter>::operator()(const NiPSysEmitter& object, const Niflib::NiPSysEmitter* native, File& file)
{
	Assert::IsTrue(object.colour.get() == nif_type_conversion<ColRGBA>::from(native->GetInitialColor()));
	Assert::IsTrue(object.lifeSpan.get() == native->GetLifeSpan());
	Assert::IsTrue(object.lifeSpanVar.get() == native->GetLifeSpanVar());
	Assert::IsTrue(object.size.get() == native->GetInitialRadius());
	Assert::IsTrue(object.sizeVar.get() == native->GetInitialRadiusVar());
	Assert::IsTrue(object.speed.get() == native->GetSpeed());
	Assert::IsTrue(object.speedVar.get() == native->GetSpeedVar());
	Assert::AreEqual(radf(object.azimuth.get()).value, native->GetPlanarAngle(), native->GetPlanarAngle() * 1.0e-5f);
	Assert::AreEqual(radf(object.azimuthVar.get()).value, native->GetPlanarAngleVar(), native->GetPlanarAngleVar() * 1.0e-5f);
	Assert::AreEqual(radf(degf(90.0f) - object.elevation.get()).value, native->GetDeclination(), native->GetDeclination() * 1.0e-5f);
	Assert::AreEqual(radf(object.elevationVar.get()).value, native->GetDeclinationVar(), native->GetDeclinationVar() * 1.0e-5f);

	return true;
}

bool common::Randomiser<NiPSysEmitter>::operator()(NiPSysEmitter& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.colour, rng);
	randomiseProperty(object.lifeSpan, rng);
	randomiseProperty(object.lifeSpanVar, rng);
	randomiseProperty(object.size, rng);
	randomiseProperty(object.sizeVar, rng);
	randomiseProperty(object.speed, rng);
	randomiseProperty(object.speedVar, rng);
	object.azimuth.set(degf(randf<float>(rng, { 0.0f, 360.0f })));
	object.azimuthVar.set(degf(randf<float>(rng, { 0.0f, 180.0f })));
	object.elevation.set(degf(randf<float>(rng, { 0.0f, 180.0f })));
	object.elevationVar.set(degf(randf<float>(rng, { 0.0f, 90.0f })));

	return true;
}

bool common::Randomiser<NiPSysEmitter>::operator()(const NiPSysEmitter&, Niflib::NiPSysEmitter* native, File& file, std::mt19937& rng)
{
	native->SetInitialColor(randf<Niflib::Color4>(rng));
	native->SetLifeSpan(randf<float>(rng));
	native->SetLifeSpanVar(randf<float>(rng));
	native->SetInitialRadius(randf<float>(rng));
	native->SetInitialRadiusVar(randf<float>(rng));
	native->SetSpeed(randf<float>(rng));
	native->SetSpeedVar(randf<float>(rng));
	native->SetPlanarAngle(randf<float>(rng));
	native->SetPlanarAngleVar(randf<float>(rng));
	native->SetDeclination(randf<float>(rng));
	native->SetDeclinationVar(randf<float>(rng));

	return true;
}


bool common::EquivalenceTester<NiPSysVolumeEmitter>::operator()(const NiPSysVolumeEmitter& object, const Niflib::NiPSysVolumeEmitter* native, File& file)
{
	Assert::IsTrue(object.emitterObject.assigned() == file.get<NiNode>(native->GetEmitterObject()));

	return true;
}

bool common::Randomiser<NiPSysVolumeEmitter>::operator()(NiPSysVolumeEmitter& object, File& file, std::mt19937& rng)
{
	auto emitterObject = file.create<NiNode>();
	file.getRoot()->children.add(emitterObject);
	object.emitterObject.assign(emitterObject);

	return true;
}

bool common::Randomiser<NiPSysVolumeEmitter>::operator()(const NiPSysVolumeEmitter&, Niflib::NiPSysVolumeEmitter* native, File& file, std::mt19937& rng)
{
	//weak ref
	Niflib::Ref<Niflib::NiNode> native_emitterObject = new Niflib::NiNode;
	file.getNative<NiNode>(file.getRoot().get())->AddChild(Niflib::StaticCast<Niflib::NiAVObject>(native_emitterObject));
	native->SetEmitterObject(native_emitterObject);
	auto emitterObject = file.get<NiNode>(native_emitterObject);
	file.getRoot()->children.add(emitterObject);

	return true;
}


bool common::EquivalenceTester<NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter& object, const Niflib::NiPSysBoxEmitter* native, File& file)
{
	Assert::IsTrue(object.width.get() == native->GetWidth());
	Assert::IsTrue(object.height.get() == native->GetHeight());
	Assert::IsTrue(object.depth.get() == native->GetDepth());

	return true;
}

bool common::Randomiser<NiPSysBoxEmitter>::operator()(NiPSysBoxEmitter& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.width, rng);
	randomiseProperty(object.height, rng);
	randomiseProperty(object.depth, rng);

	return true;
}

bool common::Randomiser<NiPSysBoxEmitter>::operator()(const NiPSysBoxEmitter&, Niflib::NiPSysBoxEmitter* native, File& file, std::mt19937& rng)
{
	native->SetWidth(randf<float>(rng));
	native->SetHeight(randf<float>(rng));
	native->SetDepth(randf<float>(rng));

	return true;
}


bool common::EquivalenceTester<NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter& object, const Niflib::NiPSysCylinderEmitter* native, File& file)
{
	Assert::IsTrue(object.radius.get() == native->GetRadius());
	Assert::IsTrue(object.length.get() == native->GetHeight());

	return true;
}

bool common::Randomiser<NiPSysCylinderEmitter>::operator()(NiPSysCylinderEmitter& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.radius, rng);
	randomiseProperty(object.length, rng);

	return true;
}

bool common::Randomiser<NiPSysCylinderEmitter>::operator()(const NiPSysCylinderEmitter&, Niflib::NiPSysCylinderEmitter* native, File& file, std::mt19937& rng)
{
	native->SetRadius(randf<float>(rng));
	native->SetHeight(randf<float>(rng));

	return true;
}


bool common::EquivalenceTester<NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter& object, const Niflib::NiPSysSphereEmitter* native, File& file)
{
	Assert::IsTrue(object.radius.get() == native->GetRadius());

	return true;
}

bool common::Randomiser<NiPSysSphereEmitter>::operator()(NiPSysSphereEmitter& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.radius, rng);

	return true;
}

bool common::Randomiser<NiPSysSphereEmitter>::operator()(const NiPSysSphereEmitter&, Niflib::NiPSysSphereEmitter* native, File& file, std::mt19937& rng)
{
	native->SetRadius(randf<float>(rng));

	return true;
}


bool common::EquivalenceTester<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr& object, const Niflib::NiPSysEmitterCtlr* native, File& file)
{
	Assert::IsTrue(object.visIplr.assigned() == file.get<NiInterpolator>(native->GetVisibilityInterpolator()));

	return true;
}

bool common::ForwardOrderTester<NiPSysEmitterCtlr>::operator()(
	const NiPSysEmitterCtlr& object, std::vector<nif::NiObject*>::iterator& it, std::vector<nif::NiObject*>::iterator end)
{
	fwdRef(object.visIplr, it, end);

	return true;
}

bool common::Randomiser<NiPSysEmitterCtlr>::operator()(NiPSysEmitterCtlr& object, File& file, std::mt19937& rng)
{
	object.visIplr.assign(file.create<NiInterpolator>());

	return true;
}

bool common::Randomiser<NiPSysEmitterCtlr>::operator()(const NiPSysEmitterCtlr&, Niflib::NiPSysEmitterCtlr* native, File& file, std::mt19937& rng)
{
	native->SetVisibilityInterpolator(new Niflib::NiInterpolator);

	return true;
}
