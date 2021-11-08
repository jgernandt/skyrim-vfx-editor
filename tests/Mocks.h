#pragma once
#include "node_concepts.h"

template<typename T>
class MockAssignable final : public IAssignable<T>
{
public:
	virtual void assign(T* t) override 
	{ 
		m_assigned = t;
		for (AssignableListener<T>* l : m_obsImpl.getListeners()) {
			assert(l);
			l->onAssign(t);
		}
	}
	virtual bool isAssigned(T* t) const override { return t == m_assigned; }

	virtual void addListener(AssignableListener<T>& l) final override { m_obsImpl.addListener(l); }
	virtual void removeListener(AssignableListener<T>& l) final override { m_obsImpl.removeListener(l); }

	T* assigned() const { return m_assigned; }

private:
	T* m_assigned{ nullptr };
	ObservableImpl<IAssignable<T>> m_obsImpl;
};

template<typename T>
class MockProperty final : public IProperty<T>
{
public:
	virtual T get() const override { return m_value; }
	virtual void set(const T& t) override 
	{ 
		m_value = t;
		for (PropertyListener<T>* l : m_obsImpl.getListeners()) {
			assert(l);
			l->onSet(t);
		}
	}

	virtual void addListener(PropertyListener<T>& l) final override
	{
		m_obsImpl.addListener(l);
		l.onSet(this->get());
	}
	virtual void removeListener(PropertyListener<T>& l) final override { m_obsImpl.removeListener(l); }

private:
	T m_value{ T() };
	ObservableImpl<IProperty<T>> m_obsImpl;
};

//This is only meant for class types
template<typename T>
class MockSet final : public ISet<T>
{
public:
	virtual void add(const T& t) override { m_set.insert(&t); }
	virtual void remove(const T& t) override { m_set.erase(&t); }
	virtual bool has(const T& t) const override { return m_set.find(&t) != m_set.end(); }

	virtual void addListener(SetListener<T>& l) final override { m_obsImpl.addListener(l); }
	virtual void removeListener(SetListener<T>& l) final override { m_obsImpl.removeListener(l); }

	virtual size_t size() const override { return m_set.size(); }

private:
	std::set<const T*> m_set;
	ObservableImpl<ISet<T>> m_obsImpl;
};

template<typename T>
class MockSequence final : public ISequence<T>
{
public:
	virtual size_t insert(size_t pos, const T& t) override
	{
		size_t result = std::min(pos, m_seq.size());
		if (result == pos)
			m_seq.insert(m_seq.begin() + pos, &t);
		else
			m_seq.push_back(&t);

		for (SequenceListener<T>* l : m_obsImpl.getListeners()) {
			assert(l);
			l->onInsert(*this, pos);
		}

		return result;
	}
	virtual size_t erase(size_t pos) override
	{
		assert(pos < m_seq.size());

		m_seq.erase(m_seq.begin() + pos);
		size_t result = pos < m_seq.size() ? pos : -1;

		for (SequenceListener<T>* l : m_obsImpl.getListeners()) {
			assert(l);
			l->onErase(*this, pos);
		}

		return result;
	}
	virtual size_t find(const T& t) const override
	{
		size_t result = -1;
		for (size_t i = 0; i < m_seq.size(); i++)
			if (m_seq[i] == &t) {
				result = i;
				break;
			}
		return result;
	}

	virtual void addListener(SequenceListener<T>& l) final override { m_obsImpl.addListener(l); }
	virtual void removeListener(SequenceListener<T>& l) final override { m_obsImpl.removeListener(l); }

	virtual size_t size() const override { return m_seq.size(); }

private:
	std::vector<const T*> m_seq;
	ObservableImpl<ISequence<T>> m_obsImpl;
};

template<typename T>
class MockListener final : public IListener<T>
{
public:
	virtual void call(const T& t) override
	{
		m_signalled = true;
		m_result = t;
	}

	bool isSignalled() const { return m_signalled; }
	T result() const { return m_result; }
	void reset() { m_result = T(); m_signalled = false; }

private:
	T m_result{ T() };
	bool m_signalled{ false };
};

template<typename T>
class MockPropertyListener final : public PropertyListener<T>
{
public:
	virtual void onSet(const T& t) override
	{
		m_signalled = true;
		m_result = t;
	}

	bool isSignalled() const { return m_signalled; }
	T result() const { return m_result; }
	void reset() { m_result = T(); m_signalled = false; }

private:
	T m_result{ T() };
	bool m_signalled{ false };
};

template<typename T>
class MockObservable final : public IObservable<T>
{
public:
	virtual void addListener(IListener<T>& l) override { m_listeners.push_back(&l); }
	virtual void removeListener(IListener<T>& l) override
	{
		if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it != m_listeners.end())
			m_listeners.erase(it);
	}

	size_t size() const { return m_listeners.size(); }
	void signal(const T& t)
	{
		for (IListener<T>* l : m_listeners)
			l->call(t);
	}

private:
	std::vector<IListener<T>*> m_listeners;
};

#include "Modifier.h"
class MockRequirementsSet : public ISet<node::Modifier::Requirement>
{
public:
	virtual void add(const node::Modifier::Requirement& t) override { m_set.insert(t); }
	virtual void remove(const node::Modifier::Requirement& t) override { m_set.erase(t); }
	virtual bool has(const node::Modifier::Requirement& t) const override { return m_set.find(t) != m_set.end(); }

	virtual void addListener(SetListener<node::Modifier::Requirement>& l) final override { m_obsImpl.addListener(l); }
	virtual void removeListener(SetListener<node::Modifier::Requirement>& l) final override { m_obsImpl.removeListener(l); }

	size_t count(node::Modifier::Requirement req) const { return m_set.count(req); }
	virtual size_t size() const override { return m_set.size(); }

private:
	std::multiset<node::Modifier::Requirement> m_set;
	ObservableImpl<ISet<node::Modifier::Requirement>> m_obsImpl;
};
struct MockModifiable : node::IModifiable
{
	virtual MockSequence<nif::NiPSysModifier>& modifiers() override { return m_mods; }
	virtual MockSequence<nif::NiTimeController>& controllers() override { return m_ctlrs; }
	virtual MockRequirementsSet& requirements() override { return m_reqs; }

private:
	MockSequence<nif::NiPSysModifier> m_mods;
	MockSequence<nif::NiTimeController> m_ctlrs;
	MockRequirementsSet m_reqs;
};
