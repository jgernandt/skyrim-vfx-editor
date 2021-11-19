#include "pch.h"
#include "CppUnitTest.h"
#include "SyncTestTraverser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nif;

void nif_tests::SyncTestTraverser<NiExtraData>::operator()(NiExtraData& object, Niflib::NiExtraData* native, File& file)
{
	name.preRead(native, &Niflib::NiExtraData::SetName);
}

void nif_tests::SyncTestTraverser<NiExtraData>::operator()(NiExtraData& object, Niflib::NiExtraData* native, File& file, int)
{
	test(object, native);
	name.preWrite(object.name);
}

void nif_tests::SyncTestTraverser<NiExtraData>::operator()(NiExtraData& object, Niflib::NiExtraData* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<NiExtraData>::test(NiExtraData& object, Niflib::NiExtraData* native)
{
	name.test(object.name, native, &Niflib::NiExtraData::GetName);
}

void nif_tests::SyncTestTraverser<NiStringExtraData>::operator()(NiStringExtraData& object, Niflib::NiStringExtraData* native, File& file)
{
	value.preRead(native, &Niflib::NiStringExtraData::SetData);
}

void nif_tests::SyncTestTraverser<NiStringExtraData>::operator()(NiStringExtraData& object, Niflib::NiStringExtraData* native, File& file, int)
{
	test(object, native);
	value.preWrite(object.value);
}

void nif_tests::SyncTestTraverser<NiStringExtraData>::operator()(NiStringExtraData& object, Niflib::NiStringExtraData* native, File& file, int, int)
{
	test(object, native);
}

void nif_tests::SyncTestTraverser<NiStringExtraData>::test(NiStringExtraData& object, Niflib::NiStringExtraData* native)
{
	value.test(object.value, native, &Niflib::NiStringExtraData::GetData);
}
