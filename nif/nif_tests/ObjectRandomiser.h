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
			flags.set(D(rng));
		}
		else {
			//enum
			std::uniform_int_distribution<typename std::underlying_type<T>::type> D;
			flags.set(static_cast<T>(D(rng)));
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
		std::uniform_int_distribution<int> D(1, 5);
		seq.clear();
		int size = D(rng);
		for (int i = 0; i < size; i++)
			seq.insert(seq.size(), file.create<T>());
	}

	template<typename T, typename GeneratorType>
	void randomiseSet(Set<T>& set, File& file, GeneratorType& rng)
	{
		std::uniform_int_distribution<int> D(1, 5);
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
		void operator() (T& object, File& file, GeneratorType& rng) {}
		//Randomise native
		template<typename GeneratorType>
		void operator() (const T& dummy, typename type_map<T>::type* native, File& file, GeneratorType& rng) {}
	};

	template<>
	struct Randomiser<NiObjectNET> : VerticalTraverser<NiObjectNET, Randomiser>
	{
		void operator() (NiObjectNET& object, File& file, std::mt19937& rng);
		void operator() (const NiObjectNET&, Niflib::NiObjectNET* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiAVObject> : VerticalTraverser<NiAVObject, Randomiser>
	{
		void operator() (NiAVObject& object, File& file, std::mt19937& rng);
		void operator() (const NiAVObject&, Niflib::NiAVObject* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiNode> : VerticalTraverser<NiNode, Randomiser>
	{
		void operator() (NiNode& object, File& file, std::mt19937& rng);
		void operator() (const NiNode&, Niflib::NiNode* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiAlphaProperty> : VerticalTraverser<NiAlphaProperty, Randomiser>
	{
		void operator() (NiAlphaProperty& object, File& file, std::mt19937& rng);
		void operator() (const NiAlphaProperty&, Niflib::NiAlphaProperty* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSEffectShaderProperty> : VerticalTraverser<BSEffectShaderProperty, Randomiser>
	{
		void operator() (BSEffectShaderProperty& object, File& file, std::mt19937& rng);
		void operator() (const BSEffectShaderProperty&, Niflib::BSEffectShaderProperty* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiBoolData> : VerticalTraverser<NiBoolData, Randomiser>
	{
		void operator() (NiBoolData& object, File& file, std::mt19937& rng);
		void operator() (const NiBoolData&, Niflib::NiBoolData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiFloatData> : VerticalTraverser<NiFloatData, Randomiser>
	{
		void operator() (NiFloatData& object, File& file, std::mt19937& rng);
		void operator() (const NiFloatData&, Niflib::NiFloatData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiBoolInterpolator> : VerticalTraverser<NiBoolInterpolator, Randomiser>
	{
		void operator() (NiBoolInterpolator& object, File& file, std::mt19937& rng);
		void operator() (const NiBoolInterpolator&, Niflib::NiBoolInterpolator* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiFloatInterpolator> : VerticalTraverser<NiFloatInterpolator, Randomiser>
	{
		void operator() (NiFloatInterpolator& object, File& file, std::mt19937& rng);
		void operator() (const NiFloatInterpolator&, Niflib::NiFloatInterpolator* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiTimeController> : VerticalTraverser<NiTimeController, Randomiser>
	{
		void operator() (NiTimeController& object, File& file, std::mt19937& rng);
		void operator() (const NiTimeController&, Niflib::NiTimeController* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiSingleInterpController> : VerticalTraverser<NiSingleInterpController, Randomiser>
	{
		void operator() (NiSingleInterpController& object, File& file, std::mt19937& rng);
		void operator() (const NiSingleInterpController&, Niflib::NiSingleInterpController* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiParticleSystem> : VerticalTraverser<NiParticleSystem, Randomiser>
	{
		void operator() (NiParticleSystem& object, File& file, std::mt19937& rng);
		void operator() (const NiParticleSystem&, Niflib::NiParticleSystem* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysData> : VerticalTraverser<NiPSysData, Randomiser>
	{
		void operator() (NiPSysData& object, File& file, std::mt19937& rng);
		void operator() (const NiPSysData&, Niflib::NiPSysData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysModifier> : VerticalTraverser<NiPSysModifier, Randomiser>
	{
		void operator() (NiPSysModifier& object, File& file, std::mt19937& rng);
		void operator() (const NiPSysModifier&, Niflib::NiPSysModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysGravityModifier> : VerticalTraverser<NiPSysGravityModifier, Randomiser>
	{
		void operator() (NiPSysGravityModifier& object, File& file, std::mt19937& rng);
		void operator() (const NiPSysGravityModifier&, Niflib::NiPSysGravityModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysRotationModifier> : VerticalTraverser<NiPSysRotationModifier, Randomiser>
	{
		void operator() (NiPSysRotationModifier& object, File& file, std::mt19937& rng);
		void operator() (const NiPSysRotationModifier&, Niflib::NiPSysRotationModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSPSysScaleModifier> : VerticalTraverser<BSPSysScaleModifier, Randomiser>
	{
		void operator() (BSPSysScaleModifier& object, File& file, std::mt19937& rng);
		void operator() (const BSPSysScaleModifier&, Niflib::BSPSysScaleModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<BSPSysSimpleColorModifier> : VerticalTraverser<BSPSysSimpleColorModifier, Randomiser>
	{
		void operator() (BSPSysSimpleColorModifier& object, File& file, std::mt19937& rng);
		void operator() (const BSPSysSimpleColorModifier&, Niflib::BSPSysSimpleColorModifier* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiPSysModifierCtlr> : VerticalTraverser<NiPSysModifierCtlr, Randomiser>
	{
		void operator() (NiPSysModifierCtlr& object, File& file, std::mt19937& rng);
		void operator() (const NiPSysModifierCtlr&, Niflib::NiPSysModifierCtlr* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiExtraData> : VerticalTraverser<NiExtraData, Randomiser>
	{
		void operator() (NiExtraData& object, File& file, std::mt19937& rng);
		void operator() (const NiExtraData&, Niflib::NiExtraData* native, File& file, std::mt19937& rng);
	};

	template<>
	struct Randomiser<NiStringExtraData> : VerticalTraverser<NiStringExtraData, Randomiser>
	{
		void operator() (NiStringExtraData& object, File& file, std::mt19937& rng);
		void operator() (const NiStringExtraData&, Niflib::NiStringExtraData* native, File& file, std::mt19937& rng);
	};
}
