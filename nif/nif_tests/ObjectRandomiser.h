#pragma once
#include "nif.h"

namespace common
{
	using namespace nif;

	template<typename T, typename GeneratorType>
	void randomiseProperty(Property<T>& prop, GeneratorType& rng)
	{
		using value_type = typename util::array_traits<T>::element_type;

		T val{};
		if constexpr (std::is_same<value_type, std::string>::value) {
			for (size_t i = 0; i < util::array_traits<T>::size; i++)
				util::array_traits<T>::at(val, i) = randomString(rng);
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
	std::string randomString(GeneratorType& rng)
	{
		std::uniform_int_distribution<int> I(3, 12);
		std::uniform_int_distribution<int> C('a', 'z');
		std::string s;
		s.reserve(I(rng));
		for (int i = 0; i < s.size(); i++)
			s.push_back(static_cast<char>(C(rng)));
		return s;
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
		void operator() (T& object, const typename type_map<T>::type* native, File& file, GeneratorType& rng) {}
		//Randomise native
		template<typename GeneratorType>
		void operator() (const T& dummy, typename type_map<T>::type* native, GeneratorType& rng) {}
	};

	template<>
	struct Randomiser<NiObjectNET> : VerticalTraverser<NiObjectNET, Randomiser>
	{
		void operator() (NiObjectNET& object, const Niflib::NiObjectNET* native, File& file, std::mt19937& rng);
		void operator() (const NiObjectNET&, Niflib::NiObjectNET* native, std::mt19937& rng);
	};
}
