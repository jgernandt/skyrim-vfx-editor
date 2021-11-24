#pragma once
#include "NiProperties.h"
#include "NiObject_internal.h"

namespace nif
{
	//NiProperty
	template<> struct type_map<Niflib::NiProperty> { using type = NiProperty; };
	template<> struct type_map<NiProperty> { using type = Niflib::NiProperty; };

	//NiAlphaProperty
	template<> struct type_map<Niflib::NiAlphaProperty> { using type = NiAlphaProperty; };
	template<> struct type_map<NiAlphaProperty> { using type = Niflib::NiAlphaProperty; };
	template<> struct ReadSyncer<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, ReadSyncer>
	{
		bool operator() (NiAlphaProperty& object, const Niflib::NiAlphaProperty* native, File& file);
	};
	template<> struct WriteSyncer<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, WriteSyncer>
	{
		bool operator() (const NiAlphaProperty& object, Niflib::NiAlphaProperty* native, const File& file);
	};

	//BSShaderProperty
	template<> struct type_map<Niflib::BSShaderProperty> { using type = BSShaderProperty; };
	template<> struct type_map<BSShaderProperty> { using type = Niflib::BSShaderProperty; };

	//BSEffectShaderProperty
	template<> struct type_map<Niflib::BSEffectShaderProperty> { using type = BSEffectShaderProperty; };
	template<> struct type_map<BSEffectShaderProperty> { using type = Niflib::BSEffectShaderProperty; };
	template<> struct ReadSyncer<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, ReadSyncer>
	{
		bool operator() (BSEffectShaderProperty& object, const Niflib::BSEffectShaderProperty* native, File& file);
	};
	template<> struct WriteSyncer<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, WriteSyncer>
	{
		bool operator() (const BSEffectShaderProperty& object, Niflib::BSEffectShaderProperty* native, const File& file);
	};
}
