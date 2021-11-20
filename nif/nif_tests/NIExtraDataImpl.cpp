#include "pch.h"
#include "CppUnitTest.h"
#include "EquivalenceTester.h"
#include "ObjectRandomiser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void common::EquivalenceTester<nif::NiExtraData>::operator()(const NiExtraData& object, const Niflib::NiExtraData* native, File& file)
{
	Assert::IsTrue(object.name.get() == native->GetName());
}

void common::Randomiser<NiExtraData>::operator()(NiExtraData& object, File& file, std::mt19937& rng)
{
	object.name.set(rands(rng));
}

void common::Randomiser<NiExtraData>::operator()(const NiExtraData&, Niflib::NiExtraData* native, File&, std::mt19937& rng)
{
	native->SetName(rands(rng));
}

void common::EquivalenceTester<nif::NiStringExtraData>::operator()(const NiStringExtraData& object, const Niflib::NiStringExtraData* native, File& file)
{
	Assert::IsTrue(object.value.get() == native->GetData());
}

void common::Randomiser<NiStringExtraData>::operator()(NiStringExtraData& object, File& file, std::mt19937& rng)
{
	object.value.set(rands(rng));
}

void common::Randomiser<NiStringExtraData>::operator()(const NiStringExtraData&, Niflib::NiStringExtraData* native, File&, std::mt19937& rng)
{
	native->SetData(rands(rng));
}
