#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void common::EquivalenceTester<NiParticleSystem>::operator()(const NiParticleSystem& object, const Niflib::NiParticleSystem* native, File& file)
{
	Assert::IsTrue(object.data.assigned() == file.get<NiPSysData>(native->GetData()).get());

	auto&& mods = native->GetModifiers();
	Assert::IsTrue(mods.size() == object.modifiers.size());
	for (size_t i = 0; i < mods.size(); i++)
		Assert::IsTrue(object.modifiers.at(i) == file.get<NiPSysModifier>(mods[i]));

	Assert::IsTrue(object.shaderProperty.assigned() == file.get<BSShaderProperty>(native->GetShaderProperty()).get());
	Assert::IsTrue(object.alphaProperty.assigned() == file.get<NiAlphaProperty>(native->GetAlphaProperty()).get());
	Assert::IsTrue(object.worldSpace.get() == native->GetWorldSpace());
}

void common::Randomiser<NiParticleSystem>::operator()(NiParticleSystem& object, File& file, std::mt19937& rng)
{
	object.data.assign(file.create<NiPSysData>());
	randomiseSequence(object.modifiers, file, rng);
	object.shaderProperty.assign(file.create<BSShaderProperty>());
	object.alphaProperty.assign(file.create<NiAlphaProperty>());
	object.worldSpace.set(randi<int>(rng, { 0, 1 }));
}

void common::Randomiser<NiParticleSystem>::operator()(const NiParticleSystem&, Niflib::NiParticleSystem* native, File&, std::mt19937& rng)
{
	native->SetData(new Niflib::NiPSysData);

	auto&& mods = native->GetModifiers();
	mods.clear();
	for (auto&& mod : randomObjVector<NiPSysModifier>(rng))
		mods.push_back(mod);

	native->SetShaderProperty(new Niflib::BSShaderProperty);
	native->SetAlphaProperty(new Niflib::NiAlphaProperty);
	native->SetWorldSpace(randi<int>(rng, { 0, 1 }));
}


void common::EquivalenceTester<NiPSysData>::operator()(const NiPSysData& object, const Niflib::NiPSysData* native, File& file)
{
	Assert::IsTrue(object.maxCount.get() == native->GetBSMaxVertices());

	auto&& offsets1 = object.subtexOffsets.get();
	auto&& offsets2 = native->GetSubtextureOffsets();
	Assert::IsTrue(offsets1.size() == offsets2.size());
	for (size_t i = 0; i < offsets1.size(); i++)
		Assert::IsTrue(offsets1[i] == nif_type_conversion<SubtextureOffset>::from(offsets2[i]));

	Assert::IsTrue(object.hasColour.get() == native->GetHasVertexColors());
	Assert::IsTrue(object.hasRotationAngles.get() == native->GetHasRotationAngles());
	Assert::IsTrue(object.hasRotationSpeeds.get() == native->GetHasRotationSpeeds());
}

void common::Randomiser<NiPSysData>::operator()(NiPSysData& object, File& file, std::mt19937& rng)
{
	object.maxCount.set(randi<unsigned short>(rng));
	object.subtexOffsets.set(randfv<SubtextureOffset>(rng));
	object.hasColour.set(randi<int>(rng, { 0, 1 }));
	object.hasRotationAngles.set(randi<int>(rng, { 0, 1 }));
	object.hasRotationSpeeds.set(randi<int>(rng, { 0, 1 }));
}

void common::Randomiser<NiPSysData>::operator()(const NiPSysData&, Niflib::NiPSysData* native, File&, std::mt19937& rng)
{
	native->SetBSMaxVertices(randi<unsigned short>(rng));
	native->GetSubtextureOffsets() = randfv<Niflib::Vector4>(rng);
	native->SetHasVertexColors(randi<int>(rng, { 0, 1 }));
	native->SetHasRotationAngles(randi<int>(rng, { 0, 1 }));
	native->SetHasRotationSpeeds(randi<int>(rng, { 0, 1 }));
}

void common::EquivalenceTester<NiPSysModifier>::operator()(const NiPSysModifier& object, const Niflib::NiPSysModifier* native, File& file)
{
	Assert::IsTrue(object.name.get() == native->GetName());
	Assert::IsTrue(object.order.get() == native->GetOrder());
	Assert::IsTrue(object.target.assigned() == file.get<NiParticleSystem>(native->GetTarget()));
	Assert::IsTrue(object.active.get() == native->GetActive());
}

void common::Randomiser<NiPSysModifier>::operator()(NiPSysModifier& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.name, rng);
	randomiseProperty(object.order, rng);
	object.target.assign(file.create<NiParticleSystem>());
	object.active.set(randi<int>(rng, { 0, 1 }));
}

void common::Randomiser<NiPSysModifier>::operator()(const NiPSysModifier&, Niflib::NiPSysModifier* native, File& file, std::mt19937& rng)
{
	native->SetName(rands(rng));
	native->SetOrder(randi<unsigned int>(rng));

	//Target is a weak reference. We need to make our new object persistent.
	Niflib::Ref<Niflib::NiParticleSystem> target = new Niflib::NiParticleSystem;
	file.getNative<NiNode>(file.getRoot().get())->AddChild(Niflib::StaticCast<Niflib::NiAVObject>(target));
	native->SetTarget(target);

	native->SetActive(randi<int>(rng, { 0, 1 }));
}


void common::EquivalenceTester<NiPSysGravityModifier>::operator()(const NiPSysGravityModifier& object, const Niflib::NiPSysGravityModifier* native, File& file)
{
	Assert::IsTrue(object.gravityObject.assigned() == file.get<NiNode>(native->GetGravityObject()).get());
	Assert::IsTrue(object.gravityAxis.get() == nif_type_conversion<Floats<3>>::from(native->GetGravityAxis()));
	Assert::IsTrue(object.decay.get() == native->GetDecay());
	Assert::IsTrue(object.strength.get() == native->GetStrength());
	Assert::IsTrue(object.forceType.get() == native->GetForceType());
	Assert::IsTrue(object.turbulence.get() == native->GetTurbulence());
	Assert::IsTrue(object.turbulenceScale.get() == native->GetTurbulenceScale());
	Assert::IsTrue(object.worldAligned.get() == native->GetWorldAligned());
}

void common::Randomiser<NiPSysGravityModifier>::operator()(NiPSysGravityModifier& object, File& file, std::mt19937& rng)
{
	object.gravityObject.assign(file.create<NiNode>());
	randomiseProperty(object.gravityAxis, rng);
	randomiseProperty(object.decay, rng);
	randomiseProperty(object.strength, rng);
	randomiseProperty(object.forceType, rng);
	randomiseProperty(object.turbulence, rng);
	randomiseProperty(object.turbulenceScale, rng);
	randomiseProperty(object.worldAligned, rng);
}

void common::Randomiser<NiPSysGravityModifier>::operator()(const NiPSysGravityModifier&, Niflib::NiPSysGravityModifier* native, File& file, std::mt19937& rng)
{
	//weak ref
	Niflib::Ref<Niflib::NiNode> gravObj = new Niflib::NiNode;
	file.getNative<NiNode>(file.getRoot().get())->AddChild(Niflib::StaticCast<Niflib::NiAVObject>(gravObj));
	native->SetGravityObject(gravObj);

	native->SetGravityAxis(randf<Niflib::Vector3>(rng));
	native->SetDecay(randf<float>(rng));
	native->SetStrength(randf<float>(rng));
	native->SetForceType(static_cast<Niflib::ForceType>(randi<int>(rng, { 0, 2 })));
	native->SetTurbulence(randf<float>(rng));
	native->SetTurbulenceScale(randf<float>(rng));
	native->SetWorldAligned(randb(rng));
}


void common::EquivalenceTester<NiPSysRotationModifier>::operator()(const NiPSysRotationModifier& object, const Niflib::NiPSysRotationModifier* native, File& file)
{
	Assert::IsTrue(object.speed.get() == native->GetRotationSpeed());
	Assert::IsTrue(object.speedVar.get() == native->GetRotationSpeedVar());
	Assert::IsTrue(object.angle.get() == native->GetRotationAngle());
	Assert::IsTrue(object.angleVar.get() == native->GetRotationAngleVar());
	Assert::IsTrue(object.randomSign.get() == native->GetRandomSpeedSign());
}

void common::Randomiser<NiPSysRotationModifier>::operator()(NiPSysRotationModifier& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.speed, rng);
	randomiseProperty(object.speedVar, rng);
	randomiseProperty(object.angle, rng);
	randomiseProperty(object.angleVar, rng);
	randomiseProperty(object.randomSign, rng);
}

void common::Randomiser<NiPSysRotationModifier>::operator()(const NiPSysRotationModifier&, Niflib::NiPSysRotationModifier* native, File& file, std::mt19937& rng)
{
	native->SetRotationSpeed(randf<float>(rng));
	native->SetRotationSpeedVar(randf<float>(rng));
	native->SetRotationAngle(randf<float>(rng));
	native->SetRotationAngleVar(randf<float>(rng));
	native->SetRandomSpeedSign(randb(rng));
}


void common::EquivalenceTester<NiPSysModifierCtlr>::operator()(const NiPSysModifierCtlr& object, const Niflib::NiPSysModifierCtlr* native, File& file)
{
	Assert::IsTrue(object.modifierName.get() == native->GetModifierName());
}

void common::Randomiser<NiPSysModifierCtlr>::operator()(NiPSysModifierCtlr& object, File& file, std::mt19937& rng)
{
	object.modifierName.set(rands(rng));
}

void common::Randomiser<NiPSysModifierCtlr>::operator()(const NiPSysModifierCtlr&, Niflib::NiPSysModifierCtlr* native, File& file, std::mt19937& rng)
{
	native->SetModifierName(rands(rng));
}


void common::EquivalenceTester<BSPSysScaleModifier>::operator()(const BSPSysScaleModifier& object, const Niflib::BSPSysScaleModifier* native, File& file)
{
	Assert::IsTrue(object.scales.get() == native->GetScales());
}

void common::Randomiser<BSPSysScaleModifier>::operator()(BSPSysScaleModifier& object, File& file, std::mt19937& rng)
{
	object.scales.set(randfv<float>(rng));
}

void common::Randomiser<BSPSysScaleModifier>::operator()(const BSPSysScaleModifier&, Niflib::BSPSysScaleModifier* native, File& file, std::mt19937& rng)
{
	native->SetScales(randfv<float>(rng));
}


void common::EquivalenceTester<BSPSysSimpleColorModifier>::operator()(const BSPSysSimpleColorModifier& object, const Niflib::BSPSysSimpleColorModifier* native, File& file)
{
	Assert::IsTrue(object.col1.value.get() == nif_type_conversion<ColRGBA>::from(native->GetColor(0)));
	Assert::IsTrue(object.col1.RGBend.get() == native->GetColor1End());

	Assert::IsTrue(object.col2.value.get() == nif_type_conversion<ColRGBA>::from(native->GetColor(1)));
	Assert::IsTrue(object.col2.RGBbegin.get() == native->GetColor2Begin());
	Assert::IsTrue(object.col2.RGBend.get() == native->GetColor2End());
	Assert::IsTrue(object.col2.Abegin.get() == native->GetFadeInEnd());
	Assert::IsTrue(object.col2.Aend.get() == native->GetFadeOutBegin());

	Assert::IsTrue(object.col3.value.get() == nif_type_conversion<ColRGBA>::from(native->GetColor(2)));
	Assert::IsTrue(object.col3.RGBbegin.get() == native->GetColor3Begin());
}

void common::Randomiser<BSPSysSimpleColorModifier>::operator()(BSPSysSimpleColorModifier& object, File& file, std::mt19937& rng)
{
	randomiseProperty(object.col1.value, rng);
	randomiseProperty(object.col1.RGBend, rng);

	randomiseProperty(object.col2.value, rng);
	randomiseProperty(object.col2.RGBbegin, rng);
	randomiseProperty(object.col2.RGBend, rng);
	randomiseProperty(object.col2.Abegin, rng);
	randomiseProperty(object.col2.Aend, rng);

	randomiseProperty(object.col3.value, rng);
	randomiseProperty(object.col3.RGBbegin, rng);
}

void common::Randomiser<BSPSysSimpleColorModifier>::operator()(const BSPSysSimpleColorModifier&, Niflib::BSPSysSimpleColorModifier* native, File& file, std::mt19937& rng)
{
	native->SetColor(0, randf<Niflib::Color4>(rng));
	native->SetColor1End(randf<float>(rng));

	native->SetColor(1, randf<Niflib::Color4>(rng));
	native->SetColor2Begin(randf<float>(rng));
	native->SetColor2End(randf<float>(rng));
	native->SetFadeInEnd(randf<float>(rng));
	native->SetFadeOutBegin(randf<float>(rng));

	native->SetColor(2, randf<Niflib::Color4>(rng));
	native->SetColor3Begin(randf<float>(rng));
}
