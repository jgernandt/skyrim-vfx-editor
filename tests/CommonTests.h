#pragma once
#include <random>
#include "CppUnitTest.h"
#include "ConnectionHandler.h"
#include "DeviceImpl.h"
#include "NodeBase.h"
#include "Mocks.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TestRoot final :
	public gui::ConnectionHandler
{
public:
	~TestRoot()
	{
		//gui::Disconnector dc;
		//accept(dc);
		//dc.execute();
	}

};

template<typename T>
T* findNode(const std::vector<std::unique_ptr<node::NodeBase>>& nodes, const nif::NiObject& obj)
{
	for (auto&& node : nodes) {
		if (T* result = dynamic_cast<T*>(node.get()); result && result->object() == obj)
			return result;
	}
	return nullptr;
}

//Test that an Assignable can in fact be assigned to.
//Requires a function object that produces objects of type T.
template<typename T, typename FactoryType>
void AssignableTest(IAssignable<T>& ass, FactoryType& factory)
{
	struct Listener : AssignableListener<T>
	{
		virtual void onAssign(T* t) 
		{
			signalled = true;
			assigned = t;
		}

		bool wasAssigned(T* t)
		{
			bool result = signalled && assigned == t;
			signalled = false;
			assigned = nullptr;
			return result;
		}

	private:
		bool signalled{ false };
		T* assigned{ nullptr };
	};

	Listener l;
	ass.addListener(l);

	std::shared_ptr<T> o1 = factory();
	Assert::IsNotNull(o1.get());
	Assert::IsFalse(ass.isAssigned(o1.get()));

	ass.assign(o1.get());
	Assert::IsTrue(ass.isAssigned(o1.get()));
	Assert::IsTrue(l.wasAssigned(o1.get()));

	//Should we require that reassigning the same object does not call the listener?

	std::shared_ptr<T> o2 = factory();
	Assert::IsNotNull(o2.get());
	Assert::IsFalse(ass.isAssigned(o2.get()));

	ass.assign(o2.get());
	Assert::IsFalse(ass.isAssigned(o1.get()));
	Assert::IsTrue(ass.isAssigned(o2.get()));
	Assert::IsTrue(l.wasAssigned(o2.get()));

	ass.assign(nullptr);
	Assert::IsFalse(ass.isAssigned(o2.get()));
	Assert::IsTrue(l.wasAssigned(nullptr));

	ass.removeListener(l);
	ass.assign(o2.get());
	Assert::IsFalse(l.wasAssigned(o2.get()));
	ass.assign(nullptr);

}

//Test that flags can indeed be set and cleared without affecting one another
template<typename T>
void FlagTest(nif::FlagSet<T>& set, std::vector<T> flags)
{
	//Require that no flags are set (not really an error, but makes the test easier)
	for (auto&& flag : flags)
		Assert::IsFalse(set.isSet(flag));

	//Raise each flag, check that no other flag is affected
	for (auto it = flags.begin(); it != flags.end(); ++it) {
		set.set(*it, true);
		Assert::IsTrue(set.isSet(*it));

		auto other = flags.begin();
		for (; other != it; ++other) {
			Assert::IsTrue(set.isSet(*other));
		}
		for (++other; other != flags.end(); ++other) {
			Assert::IsFalse(set.isSet(*other));
		}
	}

	//Clear each flag, check that no other flag is affected
	for (auto it = flags.begin(); it != flags.end(); ++it) {
		set.set(*it, false);
		Assert::IsFalse(set.isSet(*it));

		auto other = flags.begin();
		for (; other != it; ++other) {
			Assert::IsFalse(set.isSet(*other));
		}
		for (++other; other != flags.end(); ++other) {
			Assert::IsTrue(set.isSet(*other));
		}
	}
}

//Test that a Property can in fact be read from and written to
template<typename T, typename GeneratorType, typename PropertyType = IProperty<T>>
void PropertyTest(
	PropertyType& prop,
	GeneratorType& g, 
	typename util::array_traits<T>::element_type relTol = typename util::array_traits<T>::element_type(), 
	bool positive = true, 
	int n_tests = 3)
{
	using ElementType = typename util::array_traits<T>::element_type;

	struct Listener : IListener<PropertyType>
	{
		virtual void onSet(const T& t) override
		{
			m_signalled = true;
		}

		bool wasSet()
		{
			//Only check if we were called, until I can be bothered to deal with the elementwise comparison with error tolerance
			bool result = m_signalled;
			m_signalled = false;
			return result;
		}

	private:
		bool m_signalled{ false };
	};

	Listener l;
	prop.addListener(l);
	Assert::IsTrue(l.wasSet());

	if constexpr (std::is_same<ElementType, bool>::value) {

		if (util::array_traits<T>::size == 1) {
			//Just try setting both ways
			bool start = util::property_traits<PropertyType>::get(prop);

			util::property_traits<PropertyType>::set(prop, !start);
			Assert::IsTrue((util::property_traits<PropertyType>::get(prop) == !start) == positive);
			Assert::IsTrue(l.wasSet());

			util::property_traits<PropertyType>::set(prop, start);
			Assert::IsTrue((util::property_traits<PropertyType>::get(prop) == start) == positive);
			Assert::IsTrue(l.wasSet());

			//Resetting should not call listener
			util::property_traits<PropertyType>::set(prop, start);
			Assert::IsFalse(l.wasSet());
		}
		else {
			//Try random values
			std::uniform_int_distribution<int> D(0, 1);//bool not defined

			for (int i = 0; i < n_tests; i++) {

				//Populate a random T (can be an array type)
				T exp;
				for (size_t j = 0; j < util::array_traits<T>::size; j++)
					util::array_traits<T>::at(exp, j) = static_cast<bool>(D(g));

				util::property_traits<PropertyType>::set(prop, exp);

				T act = util::property_traits<PropertyType>::get(prop);
				//Compare elementwise
				for (size_t j = 0; j < util::array_traits<T>::size; j++) {
					ElementType act_j = util::array_traits<T>::at(act, j);
					ElementType exp_j = util::array_traits<T>::at(exp, j);
					Assert::IsTrue((act_j == exp_j) == positive);
				}
				Assert::IsTrue(l.wasSet());
				//Resetting should not call listener
				util::property_traits<PropertyType>::set(prop, exp);
				Assert::IsFalse(l.wasSet());
			}
		}
	}
	else if constexpr (std::is_integral<ElementType>::value) {

		//sample on [0, std::numeric_limits<ElementType>::max()]
		std::uniform_int_distribution<ElementType> D;

		for (int i = 0; i < n_tests; i++) {

			//Populate a random T (can be an array type)
			T exp;
			for (size_t j = 0; j < util::array_traits<T>::size; j++)
				util::array_traits<T>::at(exp, j) = D(g);

			util::property_traits<PropertyType>::set(prop, exp);

			//Compare elementwise
			T act = util::property_traits<PropertyType>::get(prop);
			for (size_t j = 0; j < util::array_traits<T>::size; j++) {
				ElementType act_j = util::array_traits<T>::at(act, j);
				ElementType exp_j = util::array_traits<T>::at(exp, j);
				Assert::IsTrue((act_j == exp_j) == positive);
			}
			Assert::IsTrue(l.wasSet());
			//Resetting should not call listener
			util::property_traits<PropertyType>::set(prop, exp);
			Assert::IsFalse(l.wasSet());
		}
	}
	else if constexpr (std::is_floating_point<ElementType>::value) {

		//sample on [0, 1)
		std::uniform_real_distribution<ElementType> D;

		for (int i = 0; i < n_tests; i++) {

			//Populate a random T (can be an array type)
			T exp;
			for (size_t j = 0; j < util::array_traits<T>::size; j++)
				util::array_traits<T>::at(exp, j) = D(g);

			util::property_traits<PropertyType>::set(prop, exp);

			//Compare elementwise
			T act = util::property_traits<PropertyType>::get(prop);
			for (size_t j = 0; j < util::array_traits<T>::size; j++) {
				ElementType act_j = util::array_traits<T>::at(act, j);
				ElementType exp_j = util::array_traits<T>::at(exp, j);
				if (positive)
					Assert::AreEqual(act_j, exp_j, exp_j * relTol);
				else
					Assert::AreNotEqual(act_j, exp_j, exp_j * relTol);
			}
			Assert::IsTrue(l.wasSet());
			//Resetting should not call listener (unless the actual value was different due to rounding)
			if (exp == act) {
				util::property_traits<PropertyType>::set(prop, exp);
				Assert::IsFalse(l.wasSet());
			}
		}
	}
	else {
		static_assert(false, "PropertyTest not defined for this type");
	}

	prop.removeListener(l);
}

template<typename T>
void enumPropertyTest(IProperty<T>& prop, std::vector<T>&& values)
{
	static_assert(std::is_enum<T>::value);

	class Listener final : public PropertyListener<T>
	{
	public:
		Listener(IProperty<T>& prop) : m_prop{ prop } { m_prop.addListener(*this); }
		~Listener() { m_prop.removeListener(*this); }

		virtual void onSet(const T& t) override
		{
			m_signalled = true;
			m_last = t;
		}

		bool wasSet(T t)
		{
			bool result = m_signalled && m_last == t;
			m_signalled = false;
			m_last = T();
			return result;
		}

	private:
		IProperty<T>& m_prop;
		T m_last{ T() };
		bool m_signalled{ false };
	};

	Listener l(prop);

	for (T val : values) {
		prop.set(val);
		Assert::IsTrue(prop.get() == val);
		Assert::IsTrue(l.wasSet(val));

		//Resetting should not call listener
		prop.set(val);
		Assert::IsFalse(l.wasSet(val));
	}
}

inline void StringPropertyTest(IProperty<std::string>& p)
{
	//Good enough, I guess
	constexpr const char* test = "agoansegiersnnvksd";

	p.set(test);
	Assert::IsTrue(p.get() == test);

	p.set(std::string());
	Assert::IsTrue(p.get() == std::string());
}

//Test that a Sequence does in fact insert and erase elements in the proper order.
//Requires a function object that produces shared_ptr to objects of type T.
template<typename T, typename FactoryType>
void SequenceTest(ISequence<T>& seq, const FactoryType& factory)
{
	struct Listener : SequenceListener<T>
	{
		virtual void onInsert(const ISequence<T>&, size_t pos)
		{
			m_inserted = pos;
		}
		virtual void onErase(const ISequence<T>&, size_t pos)
		{
			m_erased = pos;
		}

		//Return the index of the last call (-1 as unsigned if no call was received)
		size_t wasInserted()
		{
			size_t result = m_inserted;
			m_inserted = -1;
			return result;
		}

		size_t wasErased()
		{
			size_t result = m_erased;
			m_erased = -1;
			return result;
		}

	private:
		size_t m_inserted{ std::numeric_limits<size_t>::max() };
		size_t m_erased{ std::numeric_limits<size_t>::max() };
	};

	Listener l;
	seq.addListener(l);

	//Our indices below assumes this. We could make it work in general, if we need to.
	Assert::IsTrue(seq.size() == 0);

	std::shared_ptr<T> o1 = factory();
	Assert::IsNotNull(o1.get());
	Assert::IsTrue(seq.find(*o1) == -1);

	Assert::IsTrue(seq.insert(-1, *o1) == 0);
	Assert::IsTrue(seq.find(*o1) == 0);
	Assert::IsTrue(seq.size() == 1);
	Assert::IsTrue(l.wasInserted() == 0);
	Assert::IsTrue(l.wasErased() == -1);

	std::shared_ptr<T> o2 = factory();
	Assert::IsNotNull(o2.get());
	Assert::IsTrue(seq.find(*o2) == -1);

	Assert::IsTrue(seq.insert(-1, *o2) == 1);
	Assert::IsTrue(seq.find(*o1) == 0);
	Assert::IsTrue(seq.find(*o2) == 1);
	Assert::IsTrue(seq.size() == 2);
	Assert::IsTrue(l.wasInserted() == 1);
	Assert::IsTrue(l.wasErased() == -1);

	std::shared_ptr<T> o3 = factory();
	Assert::IsNotNull(o3.get());
	Assert::IsTrue(seq.find(*o3) == -1);

	Assert::IsTrue(seq.insert(1, *o3) == 1);
	Assert::IsTrue(seq.find(*o1) == 0);
	Assert::IsTrue(seq.find(*o2) == 2);
	Assert::IsTrue(seq.find(*o3) == 1);
	Assert::IsTrue(seq.size() == 3);
	Assert::IsTrue(l.wasInserted() == 1);
	Assert::IsTrue(l.wasErased() == -1);

	Assert::IsTrue(seq.erase(0) == 0);
	Assert::IsTrue(seq.find(*o1) == -1);
	Assert::IsTrue(seq.find(*o2) == 1);
	Assert::IsTrue(seq.find(*o3) == 0);
	Assert::IsTrue(seq.size() == 2);
	Assert::IsTrue(l.wasInserted() == -1);
	Assert::IsTrue(l.wasErased() == 0);

	Assert::IsTrue(seq.insert(10, *o1) == 2);
	Assert::IsTrue(seq.find(*o1) == 2);
	Assert::IsTrue(seq.find(*o2) == 1);
	Assert::IsTrue(seq.find(*o3) == 0);
	Assert::IsTrue(seq.size() == 3);
	Assert::IsTrue(l.wasInserted() == 2);
	Assert::IsTrue(l.wasErased() == -1);

	//Reinserting is a no-op, even if the position is different from the current.
	Assert::IsTrue(seq.insert(0, *o2) == 1);
	Assert::IsTrue(seq.find(*o1) == 2);
	Assert::IsTrue(seq.find(*o2) == 1);
	Assert::IsTrue(seq.find(*o3) == 0);
	Assert::IsTrue(seq.size() == 3);
	Assert::IsTrue(l.wasInserted() == -1);
	Assert::IsTrue(l.wasErased() == -1);

	Assert::IsTrue(seq.erase(2) == -1);
	Assert::IsTrue(seq.size() == 2);
	Assert::IsTrue(seq.erase(1) == -1);
	Assert::IsTrue(seq.size() == 1);
	Assert::IsTrue(seq.erase(0) == -1);
	Assert::IsTrue(seq.size() == 0);

	seq.removeListener(l);
}

//Test that a Set does in fact add and remove objects passed to it.
//Requires a function object that produces shared_ptr to objects of type T.
template<typename T, typename FactoryType>
void SetTest(ISet<T>& set, const FactoryType& factory)
{
	struct Listener : SetListener<T>
	{
		virtual void onAdd(const T& t)
		{
			m_added = &t;
		}
		virtual void onRemove(const T& t)
		{
			m_removed = &t;
		}

		//Return the address of the last added/removed item, if any
		const T* wasAdded()
		{
			const T* result = m_added;
			m_added = nullptr;
			return result;
		}

		const T* wasRemoved()
		{
			const T* result = m_removed;
			m_removed = nullptr;
			return result;
		}

	private:
		const T* m_added{ nullptr };
		const T* m_removed{ nullptr };
	};

	Listener l;
	set.addListener(l);

	size_t initialSize = set.size();

	std::shared_ptr<T> o1 = factory();
	Assert::IsNotNull(o1.get());
	Assert::IsFalse(set.has(*o1));

	set.add(*o1);
	Assert::IsTrue(set.has(*o1));
	Assert::IsTrue(set.size() == initialSize + 1);
	Assert::IsTrue(l.wasAdded() == o1.get());
	Assert::IsTrue(l.wasRemoved() == nullptr);

	std::shared_ptr<T> o2 = factory();
	Assert::IsFalse(set.has(*o2));

	set.add(*o2);
	Assert::IsTrue(set.has(*o1));
	Assert::IsTrue(set.has(*o2));
	Assert::IsTrue(set.size() == initialSize + 2);
	Assert::IsTrue(l.wasAdded() == o2.get());
	Assert::IsTrue(l.wasRemoved() == nullptr);

	//Re-adding should do nothing
	set.add(*o2);
	Assert::IsTrue(set.has(*o1));
	Assert::IsTrue(set.has(*o2));
	Assert::IsTrue(set.size() == initialSize + 2);
	Assert::IsTrue(l.wasAdded() == nullptr);
	Assert::IsTrue(l.wasRemoved() == nullptr);

	set.remove(*o1);
	Assert::IsFalse(set.has(*o1));
	Assert::IsTrue(set.has(*o2));
	Assert::IsTrue(set.size() == initialSize + 1);
	Assert::IsTrue(l.wasAdded() == nullptr);
	Assert::IsTrue(l.wasRemoved() == o1.get());

	//Re-removing should do nothing
	set.remove(*o1);
	Assert::IsFalse(set.has(*o1));
	Assert::IsTrue(set.has(*o2));
	Assert::IsTrue(set.size() == initialSize + 1);
	Assert::IsTrue(l.wasAdded() == nullptr);
	Assert::IsTrue(l.wasRemoved() == nullptr);

	set.remove(*o2);
	Assert::IsFalse(set.has(*o2));
	Assert::IsTrue(set.size() == initialSize);
	Assert::IsTrue(l.wasAdded() == nullptr);
	Assert::IsTrue(l.wasRemoved() == o2.get());

	set.removeListener(l);
}


//Test a vector property. Requires an overload of operator== for type T.
//Requires a function object that generates T's for testing.
template<typename T, typename GeneratorType>
void VectorPropertyTest(IVectorProperty<T>& list, GeneratorType generator)
{
	class Listener : public VectorPropertyListener<T>
	{
	public:
		Listener(IVectorProperty<T>& list) : m_list{ list } { m_list.addListener(*this); }
		~Listener() { m_list.removeListener(*this); }

		virtual void onSet(int i, const T& t) override { m_lastI = i; m_lastT = t; }
		virtual void onInsert(int i) override { m_lastInsert = i; }
		virtual void onErase(int i) override { m_lastErase = i; }
		virtual void onDestroy() override {}

		bool wasSet(int i, const T& t)
		{
			bool result = m_lastI == i && m_lastT == t;
			m_lastT = T();
			m_lastI = -1;
			return result;
		}
		bool wasInserted(int i)
		{
			bool result = i == m_lastInsert;
			m_lastInsert = -1;
			return result;
		}
		bool wasErased(int i)
		{
			bool result = i == m_lastErase;
			m_lastErase = -1;
			return result;
		}

	private:
		IVectorProperty<T>& m_list;
		const std::vector<T>* m_lastCtnr{ nullptr };
		T m_lastT{ T() };
		int m_lastI{ -1 };
		int m_lastInsert{ -1 };
		int m_lastErase{ -1 };
	};

	class ElementListener final : public PropertyListener<T>
	{
	public:
		ElementListener() {}
		~ElementListener() = default;
		virtual void onSet(const T& t) override
		{
			m_signalled = true;
			m_last = t;
		}

		//check if any value or a given value was set
		bool wasSet()
		{
			bool result = m_signalled;
			m_signalled = false;
			m_last = T();
			return result;
		}
		bool wasSet(T t)
		{
			bool result = m_signalled && m_last == t;
			m_signalled = false;
			m_last = T();
			return result;
		}

	private:
		T m_last{ T() };
		bool m_signalled{ false };
	};

	constexpr int SIZE = 10;
	constexpr int SIZE_LARGE = 13;
	constexpr int SIZE_SMALL = 7;
	list.set(std::vector<T>(SIZE));

	Listener l(list);

	//Generate a reference container
	std::vector<T> container(SIZE);
	for (T& element : container)
		element = generator();

	//create element properties
	std::vector<typename IVectorProperty<T>::element> elements;
	std::array<ElementListener, SIZE> lsnrs;

	elements.reserve(SIZE);
	for (int i = 0; i < SIZE; i++) {
		elements.push_back(list.at(i));
		elements[i].addListener(lsnrs[i]);
		Assert::IsTrue(lsnrs[i].wasSet(list.get(i)));
	}

	//get/set element
	int i = 0;
	for (T& element : container) {
		list.set(i, element);
		Assert::IsTrue(l.wasSet(i, element));
		i++;
	}

	//Verify after setting every element, in case someone sets multiple
	Assert::IsTrue(list.get() == container);
	i = 0;
	for (T& element : container) {
		Assert::IsTrue(list.get(i) == element);
		Assert::IsTrue(elements[i].get() == element);

		Assert::IsTrue(lsnrs[i].wasSet(element));

		i++;
	}
	//resetting should not call listeners
	i = 0;
	for (T& element : container) {
		list.set(i, element);
		Assert::IsFalse(l.wasSet(i, element));
		Assert::IsFalse(lsnrs[i].wasSet(element));
		i++;
	}

	//set through element property (same procedure)
	i = 0;
	for (T& element : container) {
		element = generator();
		elements[i].set(element);
		Assert::IsTrue(l.wasSet(i, element));
		i++;
	}

	Assert::IsTrue(list.get() == container);
	i = 0;
	for (T& element : container) {
		Assert::IsTrue(list.get(i) == element);
		Assert::IsTrue(elements[i].get() == element);

		Assert::IsTrue(lsnrs[i].wasSet(element));

		i++;
	}
	//resetting should not call listeners
	i = 0;
	for (T& element : container) {
		elements[i].set(element);
		Assert::IsFalse(l.wasSet(i, element));
		Assert::IsFalse(lsnrs[i].wasSet(element));
		i++;
	}

	//insert/erase
	std::vector<IVectorProperty<T>::element> newElements;
	std::array<ElementListener, SIZE + 1> newLsnrs;

	newElements.reserve(SIZE + 1);
	typename std::vector<T>::iterator it = container.begin();
	for (int i = 0; i < SIZE + 1; i++) {
		//generate a new element
		T t = generator();

		//insert in list and reference
		int inserted = list.insert(i, t);
		it = container.insert(it, t);

		Assert::IsTrue(list.get() == container);
		Assert::IsTrue(l.wasInserted(i));
		Assert::IsTrue(inserted == i);

		newElements.push_back(list.at(inserted));
		newElements[i].addListener(newLsnrs[i]);
		Assert::IsTrue(newLsnrs[i].wasSet(newElements[i].get()));

		//and erase
		int next = list.erase(inserted);
		it = container.erase(it);

		Assert::IsTrue(list.get() == container);
		Assert::IsTrue(l.wasErased(i));
		Assert::IsTrue(next == i);

		if (it != container.end())
			++it;
	}

	//get/set larger container
	container.resize(SIZE_LARGE);
	for (T& element : container)
		element = generator();

	list.set(container);
	Assert::IsTrue(list.get() == container);
	Assert::IsTrue(l.wasInserted(SIZE_LARGE - 1));

	//element listeners should have been called
	for (int i = 0; i < SIZE; i++)
		Assert::IsTrue(lsnrs[i].wasSet(list.get(i)));

	//resetting should not call listeners
	list.set(container);
	for (int i = 0; i < SIZE; i++)
		Assert::IsFalse(lsnrs[i].wasSet(list.get(i)));

	//get/set smaller container
	container.resize(SIZE_SMALL);
	for (T& element : container)
		element = generator();

	list.set(container);
	Assert::IsTrue(list.get() == container);
	Assert::IsTrue(l.wasErased(SIZE_SMALL));

	//element listeners that were not erased should have been called
	for (int i = 0; i < SIZE_SMALL; i++)
		Assert::IsTrue(lsnrs[i].wasSet(list.get(i)));

	//erased elements should not have been called (test for any value here)
	for (int i = SIZE_SMALL; i < SIZE; i++)
		Assert::IsFalse(lsnrs[i].wasSet());
	for (int i = 0; i < SIZE + 1; i++)
		Assert::IsFalse(newLsnrs[i].wasSet());

	//and setting through them should fail silently without calling anyone
	T t = generator();
	for (int i = 0; i < SIZE + 1; i++)
		newElements[i].set(t);
	Assert::IsTrue(list.get() == container);
	for (int i = 0; i < SIZE_SMALL; i++)
		Assert::IsFalse(lsnrs[i].wasSet(list.get(i)));
	for (int i = 0; i < SIZE + 1; i++)
		Assert::IsFalse(newLsnrs[i].wasSet());//test for any value here
}


inline bool areConnected(gui::Connector* c1, gui::Connector* c2)
{
	if (!c1 || !c2)
		return false;

	std::vector<gui::Connector*> connected = c1->getConnected();

	if (std::find(connected.begin(), connected.end(), c2) == connected.end())
		return false;

	connected = c2->getConnected();

	if (std::find(connected.begin(), connected.end(), c1) == connected.end())
		return false;

	return true;
}


template<typename NodeType>
class ConnectorTester
{
private:
	template<typename RType, typename SType>
	class TestConnector : public node::Receiver<RType>, public node::Sender<SType>, public gui::SingleConnector
	{
	public:
		TestConnector(SType& ifc) : node::Sender<SType>(ifc), SingleConnector(*this, *this) {}

		virtual void onConnect(RType& signal) override { connect_signal = &signal; }
		virtual void onDisconnect(RType& signal) override { disconnect_signal = &signal; }

		RType* connect_signal{ nullptr };
		RType* disconnect_signal{ nullptr };
	};

	template<typename SType>
	class TestConnector<void, SType> : public node::Receiver<void>, public node::Sender<SType>, public gui::SingleConnector
	{
	public:
		TestConnector(SType& ifc) : node::Sender<SType>(ifc), SingleConnector(*this, *this) {}

		void* connect_signal{ nullptr };
		void* disconnect_signal{ nullptr };
	};

	template<typename RType>
	class TestConnector<RType, void> : public node::Receiver<RType>, public node::Sender<void>, public gui::SingleConnector
	{
	public:
		TestConnector() : SingleConnector(*this, *this) {}

		virtual void onConnect(RType& signal) override { connect_signal = &signal; }
		virtual void onDisconnect(RType& signal) override { disconnect_signal = &signal; }

		RType* connect_signal{ nullptr };
		RType* disconnect_signal{ nullptr };
	};

public:
	ConnectorTester(std::unique_ptr<NodeType>&& node)
	{
		m_node = node.get();
		m_root.addChild(std::move(node));
	}

	~ConnectorTester() 
	{
		for (auto&& target : m_connectors)
			target.second->disconnect();
	}

	//Attempt to connect a Receiver<RType>, Sender<SType> to the given field.
	//If successful, sends the given target to the field and returns the interface exposed by it.
	//Tests single- or multi connectivity.
	//Multi is kind of broken, though. You need to manually call the function multiple times.
	template<typename RType, typename SType = void>
	RType* tryConnect(const std::string& field, bool multi, SType* target)
	{
		//Require unique targets
		Assert::IsTrue(m_connectors.find(target) == m_connectors.end());

		//Locate the connector
		gui::Connector* c1 = nullptr;
		if (node::Field* f = m_node ? m_node->getField(field) : nullptr)
			c1 = f->connector;
		Assert::IsNotNull(c1);

		//Make note of its current connections
		auto preConnect = c1->getConnected();

		//Add a new target and connect it
		TestConnector<RType, SType>* c2;
		if constexpr (std::is_same<SType, void>::value) {
			c2 = m_root.newChild<TestConnector<RType, SType>>();
		}
		else {
			Assert::IsNotNull(target);
			c2 = m_root.newChild<TestConnector<RType, SType>>(*target);
			m_connectors.insert({ target, c2 });
		}

		c2->onClick();
		c1->onRelease();

		//Test for success
		Assert::IsTrue(areConnected(c1, c2));

		//I wanted to assert on the returned interface, but not all devices may send one. Should they?
		/*if constexpr (!std::is_same<RType, void>::value) {
			Assert::IsTrue(c2->connect_signal != nullptr);
			Assert::IsNull(c2->disconnect_signal);
		}*/

		//Test for correct single/multi behaviour
		for (auto&& c : preConnect) {
			Assert::IsTrue(areConnected(c1, c) == multi);

			//See previous
			/*if constexpr (!std::is_same<RType, void>::value) {
				//it's safe to assume that the previous connectors are of the same type
				Assert::IsTrue((static_cast<TestConnector<RType, SType>*>(c)->disconnect_signal == nullptr) == multi);
			}*/
		}

		return c2->connect_signal;
	}

	template<typename SType>
	void disconnect(SType* target)
	{
		//Disconnect and remove the connector
		if (auto it = m_connectors.find(target); it != m_connectors.end()) {
			it->second->disconnect();
			m_root.removeChild(it->second);
			m_connectors.erase(it);
		}
	}

	NodeType* getNode() const { return m_node; }

private:
	TestRoot m_root;
	NodeType* m_node;
	std::map<void*, gui::Connector*> m_connectors;
};

template<typename NodeType, typename AssType>
void AssignableReceiverTest(const std::string& field, bool multi, std::unique_ptr<NodeType>&& node, AssType* exp)
{
	MockAssignable<AssType> target1;
	MockAssignable<AssType> target2;
	ConnectorTester<NodeType> tester(std::move(node));

	tester.tryConnect<void, IAssignable<AssType>>(field, multi, &target1);
	tester.tryConnect<void, IAssignable<AssType>>(field, multi, &target2);

	Assert::IsTrue(target1.isAssigned(exp) == multi);
	Assert::IsTrue(target2.isAssigned(exp));

	if (multi) {
		tester.disconnect<IAssignable<AssType>>(&target1);
		Assert::IsFalse(target1.isAssigned(exp));
	}

	tester.disconnect<IAssignable<AssType>>(&target2);
	Assert::IsFalse(target2.isAssigned(exp));
}

template<typename NodeType, typename AssType, typename FactoryType>
void AssignableSenderTest(const std::string& field, bool multi, std::unique_ptr<NodeType>&& node, IAssignable<AssType>& target, const FactoryType& factory)
{
	ConnectorTester<NodeType> tester(std::move(node));
	tester.tryConnect<IAssignable<AssType>, void>(field, multi, nullptr);
	auto ifc = tester.tryConnect<IAssignable<AssType>, void>(field, multi, nullptr);
	Assert::IsNotNull(ifc);

	std::shared_ptr<AssType> obj1 = factory();
	ifc->assign(obj1.get());
	Assert::IsTrue(target.isAssigned(obj1.get()));

	std::shared_ptr<AssType> obj2 = factory();
	ifc->assign(obj2.get());
	Assert::IsFalse(target.isAssigned(obj1.get()));
	Assert::IsTrue(target.isAssigned(obj2.get()));

	ifc->assign(nullptr);
	Assert::IsFalse(target.isAssigned(obj2.get()));
}

template<typename NodeType, typename SetType>
void SetReceiverTest(const std::string& field, bool multi, std::unique_ptr<NodeType>&& node, SetType& exp)
{
	MockSet<SetType> target1;
	MockSet<SetType> target2;
	ConnectorTester<NodeType> tester(std::move(node));

	tester.tryConnect<void, ISet<SetType>>(field, multi, &target1);
	tester.tryConnect<void, ISet<SetType>>(field, multi, &target2);

	Assert::IsTrue(target1.has(exp) == multi);
	Assert::IsTrue(target2.has(exp));

	if (multi) {
		tester.disconnect<ISet<SetType>>(&target1);
		Assert::IsFalse(target1.has(exp));
	}

	tester.disconnect<ISet<SetType>>(&target2);
	Assert::IsFalse(target2.has(exp));
}

template<typename NodeType, typename SetType, typename FactoryType>
void SetSenderTest(
	const std::string& field, 
	bool multi, 
	std::unique_ptr<NodeType>&& node, 
	ISet<SetType>& target, 
	const FactoryType& factory)
{
	ConnectorTester<NodeType> tester(std::move(node));
	tester.tryConnect<ISet<SetType>, void>(field, multi, nullptr);
	auto ifc = tester.tryConnect<ISet<SetType>, void>(field, multi, nullptr);
	Assert::IsNotNull(ifc);

	std::shared_ptr<SetType> obj1 = factory();
	Assert::IsNotNull(obj1.get());
	ifc->add(*obj1);
	Assert::IsTrue(target.has(*obj1));

	std::shared_ptr<SetType> obj2 = factory();
	Assert::IsNotNull(obj2.get());
	ifc->add(*obj2);
	Assert::IsTrue(target.has(*obj1));
	Assert::IsTrue(target.has(*obj2));

	ifc->remove(*obj1);
	Assert::IsFalse(target.has(*obj1));
	Assert::IsTrue(target.has(*obj2));

	ifc->remove(*obj2);
	Assert::IsFalse(target.has(*obj2));
}