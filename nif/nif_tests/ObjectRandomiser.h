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

#pragma once
#include "nif.h"

namespace common
{
	using namespace nif;

	template<typename T, typename GeneratorType>
	void randomiseFlags(FlagSet<T>& flags, GeneratorType& rng, std::array<T, 2> limits = { std::numeric_limits<T>::min(), std::numeric_limits<T>::max() })
	{
		flags.clear();
		if constexpr (std::is_integral<T>::value) {
			std::uniform_int_distribution<T> D(limits[0], limits[1]);
			flags.raise(D(rng));
		}
		else {
			//enum
			std::uniform_int_distribution<typename std::underlying_type<T>::type> D;
			flags.raise(static_cast<T>(D(rng)));
		}
	}

	template<typename T, typename GeneratorType>
	void randomiseProperty(Property<T>& prop, GeneratorType& rng)
	{
		using value_type = typename util::array_traits<T>::element_type;

		T val{};
		if constexpr (std::is_same<value_type, std::string>::value) {
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(val, i) = rands(rng);
		}
		else if constexpr (std::is_same<value_type, bool>::value) {
			std::uniform_int_distribution<int> D(0, 1);
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(val, i) = D(rng);
		}
		else if constexpr (std::is_arithmetic<value_type>::value) {
			if constexpr (std::is_floating_point<value_type>::value) {
				std::uniform_real_distribution<value_type> D;
				for (size_t i = 0; i < util::array_traits<T>::size; i++)
					util::array_traits<T>::at(val, i) = D(rng);
			}
			else if constexpr (std::is_integral<value_type>::value) {
				std::uniform_int_distribution<value_type> D;
				for (size_t i = 0; i < util::array_traits<T>::size; i++)
					util::array_traits<T>::at(val, i) = D(rng);
			}
		}
		else if constexpr (std::is_enum<value_type>::value) {
			static_assert(false, "TODO");
		}
		else
			static_assert(false, "Property type cannot be randomised");

		prop.set(val);
	}

	template<typename T, typename GeneratorType>
	void randomiseSequence(Sequence<T>& seq, File& file, GeneratorType& rng)
	{
		std::uniform_int_distribution<int> D(2, 5);
		seq.clear();
		int size = D(rng);
		for (int i = 0; i < size; i++)
			seq.insert(seq.size(), file.create<T>());
	}

	template<typename T, typename GeneratorType>
	void randomiseSet(Set<T>& set, File& file, GeneratorType& rng)
	{
		std::uniform_int_distribution<int> D(2, 5);
		set.clear();
		int size = D(rng);
		for (int i = 0; i < size; i++)
			set.add(file.create<T>());
	}

	template<typename GeneratorType>
	bool randb(GeneratorType& rng)
	{
		std::uniform_int_distribution<int> D(0, 1);
		return D(rng);
	}

	template<typename T, typename GeneratorType>
	T randf(GeneratorType& rng, std::array<typename util::array_traits<T>::element_type, 2> limits = 
		{ typename util::array_traits<T>::element_type(0), typename util::array_traits<T>::element_type(1) })
	{
		using value_type = typename util::array_traits<T>::element_type;

		T val{};
		std::uniform_real_distribution<value_type> D(limits[0], limits[1]);
		for (size_t i = 0; i < util::array_traits<T>::size; i++)
			util::array_traits<T>::at(val, i) = D(rng);

		return val;
	}

	template<typename T, typename GeneratorType>
	T randi(GeneratorType& rng,
		std::array<T, 2> limits =
		{ std::numeric_limits<typename util::array_traits<T>::element_type>::min(),
		std::numeric_limits<typename util::array_traits<T>::element_type>::max() })
	{
		using value_type = typename util::array_traits<T>::element_type;

		T val{};
		std::uniform_int_distribution<value_type> D(limits[0], limits[1]);
		for (size_t i = 0; i < util::array_traits<T>::size; i++)
			util::array_traits<T>::at(val, i) = D(rng);

		return val;
	}

	template<typename GeneratorType>
	std::string rands(GeneratorType& rng)
	{
		std::uniform_int_distribution<int> I(3, 12);
		std::uniform_int_distribution<int> C('a', 'z');
		std::string s;
		s.resize(I(rng));
		for (size_t i = 0; i < s.size(); i++)
			s[i] = static_cast<char>(C(rng));
		return s;
	}

	template<typename T, typename GeneratorType>
	std::vector<T> randfv(GeneratorType& rng, std::array<typename util::array_traits<T>::element_type, 2> limits =
		{ typename util::array_traits<T>::element_type(0), typename util::array_traits<T>::element_type(1) })
	{
		using value_type = typename util::array_traits<T>::element_type;

		std::uniform_int_distribution<int> I(1, 5);
		std::uniform_real_distribution<value_type> D(limits[0], limits[1]);

		std::vector<T> out(I(rng));
		for (auto&& el : out) {
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(el, i) = D(rng);
		}

		return out;
	}

	template<typename T, typename GeneratorType>
	std::vector<Niflib::Ref<typename type_map<T>::type>> randomObjVector(GeneratorType& rng)
	{
		std::uniform_int_distribution<int> D(1, 5);
		std::vector<Niflib::Ref<typename type_map<T>::type>> out(D(rng));
		for (auto&& ref : out)
			ref = new typename type_map<T>::type;
		return out;
	}

	//Assign random values to all fields
	template<typename T>
	struct Randomiser : VerticalTraverser<T, Randomiser>
	{
		//Randomise object
		template<typename GeneratorType>
		bool operator() (T& object, File& file, GeneratorType& rng) { return true; }
		//Randomise native
		template<typename GeneratorType>
		bool operator() (const T& dummy, typename type_map<T>::type* native, File& file, GeneratorType& rng) { return true; }
	};

	template<>
	struct Randomiser<NiObjectNET> : VerticalTraverser<NiObjectNET, Randomiser>
	{
		bool operator() (NiObjectNET& object, File& file, std::mt19937& rng);
		bool operator() (const NiObjectNET&, Niflib::NiObjectNET* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiAVObject> : VerticalTraverser<NiAVObject, Randomiser>
	{
		bool operator() (NiAVObject& object, File& file, std::mt19937& rng);
		bool operator() (const NiAVObject&, Niflib::NiAVObject* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiNode> : VerticalTraverser<NiNode, Randomiser>
	{
		bool operator() (NiNode& object, File& file, std::mt19937& rng);
		bool operator() (const NiNode&, Niflib::NiNode* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiBillboardNode> : VerticalTraverser<NiBillboardNode, Randomiser>
	{
		bool operator() (NiBillboardNode& object, File& file, std::mt19937& rng);
		bool operator() (const NiBillboardNode&, Niflib::NiBillboardNode* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, Randomiser>
	{
		bool operator() (NiAlphaProperty& object, File& file, std::mt19937& rng);
		bool operator() (const NiAlphaProperty&, Niflib::NiAlphaProperty* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, Randomiser>
	{
		bool operator() (BSEffectShaderProperty& object, File& file, std::mt19937& rng);
		bool operator() (const BSEffectShaderProperty&, Niflib::BSEffectShaderProperty* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiBoolData> : VerticalTraverser<NiBoolData, Randomiser>
	{
		bool operator() (NiBoolData& object, File& file, std::mt19937& rng);
		bool operator() (const NiBoolData&, Niflib::NiBoolData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiFloatData> : VerticalTraverser<NiFloatData, Randomiser>
	{
		bool operator() (NiFloatData& object, File& file, std::mt19937& rng);
		bool operator() (const NiFloatData&, Niflib::NiFloatData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, Randomiser>
	{
		bool operator() (NiBoolInterpolator& object, File& file, std::mt19937& rng);
		bool operator() (const NiBoolInterpolator&, Niflib::NiBoolInterpolator* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, Randomiser>
	{
		bool operator() (NiFloatInterpolator& object, File& file, std::mt19937& rng);
		bool operator() (const NiFloatInterpolator&, Niflib::NiFloatInterpolator* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiTimeController> : VerticalTraverser<NiTimeController, Randomiser>
	{
		bool operator() (NiTimeController& object, File& file, std::mt19937& rng);
		bool operator() (const NiTimeController&, Niflib::NiTimeController* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, Randomiser>
	{
		bool operator() (NiSingleInterpController& object, File& file, std::mt19937& rng);
		bool operator() (const NiSingleInterpController&, Niflib::NiSingleInterpController* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiControllerManager> : VerticalTraverser<NiControllerManager, Randomiser>
	{
		bool operator() (NiControllerManager& object, File& file, std::mt19937& rng);
		bool operator() (const NiControllerManager&, Niflib::NiControllerManager* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiControllerSequence> : VerticalTraverser<NiControllerSequence, Randomiser>
	{
		bool operator() (NiControllerSequence& object, File& file, std::mt19937& rng);
		bool operator() (const NiControllerSequence&, Niflib::NiControllerSequence* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiDefaultAVObjectPalette> : VerticalTraverser<NiDefaultAVObjectPalette, Randomiser>
	{
		bool operator() (NiDefaultAVObjectPalette& object, File& file, std::mt19937& rng);
		bool operator() (const NiDefaultAVObjectPalette&, Niflib::NiDefaultAVObjectPalette* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiParticleSystem> : VerticalTraverser<NiParticleSystem, Randomiser>
	{
		bool operator() (NiParticleSystem& object, File& file, std::mt19937& rng);
		bool operator() (const NiParticleSystem&, Niflib::NiParticleSystem* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysData> : VerticalTraverser<NiPSysData, Randomiser>
	{
		bool operator() (NiPSysData& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysData&, Niflib::NiPSysData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysModifier> : VerticalTraverser<NiPSysModifier, Randomiser>
	{
		bool operator() (NiPSysModifier& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysModifier&, Niflib::NiPSysModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, Randomiser>
	{
		bool operator() (NiPSysGravityModifier& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysGravityModifier&, Niflib::NiPSysGravityModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, Randomiser>
	{
		bool operator() (NiPSysRotationModifier& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysRotationModifier&, Niflib::NiPSysRotationModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, Randomiser>
	{
		bool operator() (BSPSysScaleModifier& object, File& file, std::mt19937& rng);
		bool operator() (const BSPSysScaleModifier&, Niflib::BSPSysScaleModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, Randomiser>
	{
		bool operator() (BSPSysSimpleColorModifier& object, File& file, std::mt19937& rng);
		bool operator() (const BSPSysSimpleColorModifier&, Niflib::BSPSysSimpleColorModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysEmitter> : VerticalTraverser<NiPSysEmitter, Randomiser>
	{
		bool operator() (NiPSysEmitter& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysEmitter&, Niflib::NiPSysEmitter* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysVolumeEmitter> : VerticalTraverser<NiPSysVolumeEmitter, Randomiser>
	{
		bool operator() (NiPSysVolumeEmitter& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysVolumeEmitter&, Niflib::NiPSysVolumeEmitter* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysBoxEmitter> : VerticalTraverser<NiPSysBoxEmitter, Randomiser>
	{
		bool operator() (NiPSysBoxEmitter& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysBoxEmitter&, Niflib::NiPSysBoxEmitter* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysCylinderEmitter> : VerticalTraverser<NiPSysCylinderEmitter, Randomiser>
	{
		bool operator() (NiPSysCylinderEmitter& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysCylinderEmitter&, Niflib::NiPSysCylinderEmitter* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysSphereEmitter> : VerticalTraverser<NiPSysSphereEmitter, Randomiser>
	{
		bool operator() (NiPSysSphereEmitter& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysSphereEmitter&, Niflib::NiPSysSphereEmitter* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, Randomiser>
	{
		bool operator() (NiPSysModifierCtlr& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysModifierCtlr&, Niflib::NiPSysModifierCtlr* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysEmitterCtlr> : VerticalTraverser<NiPSysEmitterCtlr, Randomiser>
	{
		bool operator() (NiPSysEmitterCtlr& object, File& file, std::mt19937& rng);
		bool operator() (const NiPSysEmitterCtlr&, Niflib::NiPSysEmitterCtlr* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiExtraData> : VerticalTraverser<NiExtraData, Randomiser>
	{
		bool operator() (NiExtraData& object, File& file, std::mt19937& rng);
		bool operator() (const NiExtraData&, Niflib::NiExtraData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiStringExtraData> : VerticalTraverser<NiStringExtraData, Randomiser>
	{
		bool operator() (NiStringExtraData& object, File& file, std::mt19937& rng);
		bool operator() (const NiStringExtraData&, Niflib::NiStringExtraData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiStringsExtraData> : VerticalTraverser<NiStringsExtraData, Randomiser>
	{
		bool operator() (NiStringsExtraData& object, File& file, std::mt19937& rng);
		bool operator() (const NiStringsExtraData&, Niflib::NiStringsExtraData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiTextKeyExtraData> : VerticalTraverser<NiTextKeyExtraData, Randomiser>
	{
		bool operator() (NiTextKeyExtraData& object, File& file, std::mt19937& rng);
		bool operator() (const NiTextKeyExtraData&, Niflib::NiTextKeyExtraData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSBehaviorGraphExtraData> : VerticalTraverser<BSBehaviorGraphExtraData, Randomiser>
	{
		bool operator() (BSBehaviorGraphExtraData& object, File& file, std::mt19937& rng);
		bool operator() (const BSBehaviorGraphExtraData&, Niflib::BSBehaviorGraphExtraData* native, File& file, std::mt19937& rng);
	};
}
