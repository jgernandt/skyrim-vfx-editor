#pragma once
#include "NiNode.h"
#include "NiObject_internal.h"

namespace nif
{
	//NiNode
	template<> struct type_map<Niflib::NiNode> { using type = NiNode; };
	template<> struct type_map<NiNode> { using type = Niflib::NiNode; };

	template<> struct ReadSyncer<NiNode> : VerticalTraverser<NiNode, ReadSyncer>
	{
		bool operator() (NiNode& object, const Niflib::NiNode* native, File& file);
	};
	template<> struct WriteSyncer<NiNode> : VerticalTraverser<NiNode, WriteSyncer>
	{
		bool operator() (const NiNode& object, Niflib::NiNode* native, const File& file);
	};

	//BSFadeNode
	template<> struct type_map<Niflib::BSFadeNode> { using type = BSFadeNode; };
	template<> struct type_map<BSFadeNode> { using type = Niflib::BSFadeNode; };
}
