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
#include "nif_internal.h"

const size_t nif::NiControllerManager::TYPE = std::hash<std::string>{}("NiControllerManager");
const size_t nif::NiControllerSequence::TYPE = std::hash<std::string>{}("NiControllerSequence");
const size_t nif::NiDefaultAVObjectPalette::TYPE = std::hash<std::string>{}("NiDefaultAVObjectPalette");

bool nif::Forwarder<nif::NiControllerManager>::operator()(NiControllerManager& object, NiTraverser& traverser)
{
	for (auto&& seq : object.ctlrSequences) {
		assert(seq);
		seq->receive(traverser);
	}

	if (auto&& p = object.objectPalette.assigned())
		p->receive(traverser);

	return true;
}

bool nif::ReadSyncer<nif::NiControllerManager>::operator()(
	NiControllerManager& object, const Niflib::NiControllerManager* native, File& file)
{
	assert(native);

	object.cumulative.set(native->GetCumulative());

	object.ctlrSequences.clear();
	for (auto&& seq : native->GetControllerSequences()) {
		if (seq)
			object.ctlrSequences.add(file.get<NiControllerSequence>(seq));
	}

	object.objectPalette.assign(file.get<NiDefaultAVObjectPalette>(native->GetObjectPalette()));

	return true;
}

bool nif::WriteSyncer<nif::NiControllerManager>::operator()(
	const NiControllerManager& object, Niflib::NiControllerManager* native, const File& file)
{
	assert(native);

	native->SetCumulative(object.cumulative.get());

	std::vector<Niflib::NiControllerSequenceRef> seqs;
	seqs.reserve(object.ctlrSequences.size());
	for (auto&& seq : object.ctlrSequences)
		seqs.push_back(file.getNative<NiControllerSequence>(seq.get()));
	native->SetControllerSequences(std::move(seqs));

	native->SetObjectPalette(file.getNative<NiDefaultAVObjectPalette>(object.objectPalette.assigned().get()));

	return true;
}


bool nif::Forwarder<nif::NiControllerSequence>::operator()(NiControllerSequence& object, NiTraverser& traverser)
{
	for (auto&& block : object.blocks) {
		if (auto&& iplr = block.interpolator.assigned())
			iplr->receive(traverser);
		//Don't forward to the controller, it should be owned by someone else as well
	}

	if (auto&& keys = object.textKeys.assigned())
		keys->receive(traverser);

	return true;
}

bool nif::ReadSyncer<nif::NiControllerSequence>::operator()(
	NiControllerSequence& object, const Niflib::NiControllerSequence* native, File& file)
{
	assert(native);

	object.name.set(native->GetName());
	object.arrayGrowBy.set(native->GetArrayGrowBy());

	object.blocks.clear();
	for (auto&& block : native->GetControlledBlocks()) {
		object.blocks.push_back();
		object.blocks.back().interpolator.assign(file.get<NiInterpolator>(block.interpolator));
		object.blocks.back().controller.assign(file.get<NiTimeController>(block.controller));
		object.blocks.back().priority.set(block.priority);
		object.blocks.back().nodeName.set(block.nodeName);
		object.blocks.back().propertyType.set(block.propertyType);
		object.blocks.back().ctlrType.set(block.controllerType);
		object.blocks.back().ctlrID.set(block.variable1);
		object.blocks.back().iplrID.set(block.variable2);
	}

	object.weight.set(native->GetWeight());
	object.textKeys.assign(file.get<NiTextKeyExtraData>(native->GetTextKeyExtraData()));
	object.cycleType.set(nif_type_conversion<CycleType>::from(native->GetCycleType()));
	object.frequency.set(native->GetFrequency());
	object.startTime.set(native->GetStartTime());
	object.stopTime.set(native->GetStopTime());

	auto manager = file.get<NiControllerManager>(native->GetParent());
	file.keepAlive(manager);
	object.manager.assign(manager);

	object.accumRootName.set(native->GetTargetName());

	return true;
}

bool nif::WriteSyncer<nif::NiControllerSequence>::operator()(
	const NiControllerSequence& object, Niflib::NiControllerSequence* native, const File& file)
{
	assert(native);

	native->SetName(object.name.get());
	native->SetArrayGrowBy(object.arrayGrowBy.get());

	std::vector<Niflib::ControllerLink> blocks;
	blocks.reserve(object.blocks.size());
	for (auto&& block : object.blocks) {
		blocks.push_back({});
		blocks.back().interpolator = file.getNative<NiInterpolator>(block.interpolator.assigned().get());
		blocks.back().controller = file.getNative<NiTimeController>(block.controller.assigned().get());
		blocks.back().priority = block.priority.get();
		blocks.back().nodeName = block.nodeName.get();
		blocks.back().propertyType = block.propertyType.get();
		blocks.back().controllerType = block.ctlrType.get();
		blocks.back().variable1 = block.ctlrID.get();
		blocks.back().variable2 = block.iplrID.get();
	}
	native->SetControlledBlocks(std::move(blocks));
	native->SetWeight(object.weight.get());
	native->SetTextKey(file.getNative<NiTextKeyExtraData>(object.textKeys.assigned().get()));
	native->SetCycleType(nif_type_conversion<Niflib::CycleType>::from(object.cycleType.get()));
	native->SetFrequency(object.frequency.get());
	native->SetStartTime(object.startTime.get());
	native->SetStopTime(object.stopTime.get());

	//if (native->GetParent()) {
		//It's not safe to access the target manager. It may have been destroyed.
		//If the manager has been changed since the last sync, we'll be included in both. 
		//This is fine. Only one will be exported.
	//}

	if (auto&& manager = object.manager.assigned()) {
		Niflib::NiControllerManagerRef native_manager = file.getNative<NiControllerManager>(manager.get());
		if (native_manager)
			native_manager->AddSequence(native);
	}

	native->SetTargetName(object.accumRootName.get());

	return true;
}


bool nif::ReadSyncer<nif::NiDefaultAVObjectPalette>::operator()(
	NiDefaultAVObjectPalette& object, const Niflib::NiDefaultAVObjectPalette* native, File& file)
{
	assert(native);

	auto scene = file.get<NiAVObject>(native->GetScene());
	file.keepAlive(scene);
	object.scene.assign(scene);

	object.objects.clear();
	for (auto&& obj : native->GetObjects()) {
		if (obj.avObject) {
			object.objects.push_back();

			auto o = file.get<NiAVObject>(obj.avObject);
			file.keepAlive(o);

			object.objects.back().assign(o);
		}
	}

	return true;
}

bool nif::WriteSyncer<nif::NiDefaultAVObjectPalette>::operator()(
	const NiDefaultAVObjectPalette& object, Niflib::NiDefaultAVObjectPalette* native, const File& file)
{
	assert(native);

	native->SetScene(file.getNative<NiAVObject>(object.scene.assigned().get()));

	std::vector<Niflib::AVObject> vec;
	vec.reserve(object.objects.size());
	for (auto&& ptr : object.objects) {
		if (auto assigned = ptr.assigned()) {
			vec.push_back({});
			vec.back().name = assigned->name.get();
			vec.back().avObject = file.getNative<NiAVObject>(assigned.get());
		}
	}
	native->SetObjects(std::move(vec));

	return true;
}
